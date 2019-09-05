#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

static FILE *file;

static int __cdecl cmp(const void *_p1, const void *_p2)
{
	player *p1 = *(player **)_p1;
	player *p2 = *(player **)_p2;
	int n = p2->units.count - p1->units.count;
	if (n)
		return n;
	n = p2->money - p1->money;
	if (n)
		return n;
	return p1->id - p2->id;
}

static void write(int n)
{
	fprintf(file, "%d", n);
}

static void write(int n, char c)
{
	for (int i = 0; i < n; i++)
		fputc(c, file);
}

static void write(char *s)
{
	fprintf(file, "%s", s);
}

static void space(int n)
{
	write(n, ' ');
}

static void newline()
{
	fputc('\n', file);
}

static void write(char *s, int width)
{
	write(s);
	space(width - strlen(s));
}

static void write(int n, int width)
{
	char s[20];
	sprintf(s, "%d", n);
	write(s, width);
}

static void right(char *s, int width)
{
	space(width - strlen(s));
	write(s);
}

static void right(int n, int width)
{
	char s[20];
	sprintf(s, "%d", n);
	right(s, width);
}

static int wrap(char *s, int indent)
{
	int i = indent;
	for (;;)
	{
		// Find next word break

		int j = 0;
		while (s[j] && s[j] != ' ')
			j++;

		// If we'll go over the 70 column mark
		// and are not already at the start of a line
		// then go to a new line

		if (i > indent && i + 1 + j > 70)
		{
			newline();
			space(indent);
			i = indent;
		}

		// If we're not at the start of a new line
		// then put a space before this word

		if (i > indent)
		{
			space(1);
			i++;
		}

		// Print this word

		i += j;
		while (j)
		{
			fputc(*s++, file);
			j--;
		}

		// If we're at the end of the string
		// then we're done

		if (!*s)
			return i;

		// Move on to the next word

		s++;
	}
}

static void writeline(char *s)
{
	write(s);
	newline();
}

static void underline(char *s)
{
	writeline(s);
	write(strlen(s), '-');
	newline();
	newline();
}

static void item(char *caption, int width, char *s)
{
	space(2);
	write(caption);
	write(":");
	space(width + 2 - strlen(caption));
	writeline(s);
}

static void item(char *caption, int width, int n)
{
	char s[20];
	sprintf(s, "%d", n);
	item(caption, width, s);
}

static int countcities(array &units)
{
	int n = 0;
	for (int i = 0; i < units.count; i++)
	{
		unit *u = (unit *)units[i];
		if (u->type == u_city)
			n++;
	}
	return n;
}

static void reportevent(char *s)
{
	space(2);
	int i = strchr(s, ' ') - s + 1;
	for (int j = 0; j < i; j++)
		write(1, s[j]);
	wrap(s + i, 2 + i);
	newline();
}

static void reportevents(entity *e)
{
	for (int i = 0; i < e->events.count; i++)
		reportevent((char *)e->events[i]);
	if (e->events.count)
		newline();
}

static void reportmsg()
{
	FILE *msgfile = fopen("message", "r");
	if (msgfile)
	{
		for (;;)
		{
			int c = fgetc(msgfile);
			if (c == EOF)
				break;
			fputc(c, file);
		}
		fclose(msgfile);
	}
}

static void reportheader(player *p)
{
	write("Rainbow's End Turn ");
	write(turn);
	newline();
	writeline(nameid(p));
	newline();
}

static void reporttotals()
{
	int money = 0;
	for (int i = 0; i < playerscount; i++)
		money += players[i]->money;
	int cities = countcities(units);

	writeline("Game totals");
	item("Players", 11, playerscount);
	item("Map size", 11, mapsize);
	item("Money", 11, money);
	item("Cities", 11, cities);
	item("Other units", 11, units.count - cities);
	newline();
}

static void reportgeneralorders(player *p)
{
	if (!p->events.count)
		return;
	writeline("General orders");
	reportevents(p);
}

static void reportplayersummary(player *p)
{
	writeline("Player summary");
	writeline("  number  relations  money  units");
	writeline("  ------  ---------  -----  -----");

	int totmoney = 0;
	int totunits = 0;

	for (int i = 0; i < playerscount; i++)
	{
		player *p2 = players2[i];

		space(2);
		write(p2->id, 8);
		if (p == p2)
			write("n/a", 9);
		else if (p->friendly.contains(p2))
			write("friendly", 9);
		else
			write("hostile", 9);
		right(p2->money, 7);
		right(p2->units.count, 7);
		newline();

		totmoney += p2->money;
		totunits += p2->units.count;
	}

	writeline("  ------  ---------  -----  -----");

	space(19);
	right(totmoney, 7);
	right(totunits, 7);
	newline();

	newline();
}

static void reportplayerdetails(player *p)
{
	underline("Player details");

	for (int i = 0; i < playerscount; i++)
	{
		player *p2 = players[i];
		int cities = countcities(p2->units);

		writeline(nameid(p2));
		item("Email", 11, p2->email[0] ? p2->email : "None");
		if (p == p2)
			item("Relations", 11, "N/A");
		else if (p->friendly.contains(p2))
			item("Relations", 11, "Friendly");
		else
			item("Relations", 11, "Hostile");
		item("Money", 11, p2->money);
		item("Cities", 11, cities);
		item("Other units", 11, p2->units.count - cities);
		newline();
	}
}

static void reportunitsummary(player *p, player *p2)
{
	int found = 0;
	int i;
	for (i = 0; i < p2->units.count; i++)
	{
		unit *u = (unit *)p2->units[i];
		if (!cansee(p, u))
			continue;

		found = 1;
		break;
	}
	if (!found)
		return;

	write("Unit summary: ");
	writeline(nameid(p2));
	writeline("  number  type        x   y  group");
	writeline("  ------  ---------  --  --  -----");

	for (i = 0; i < p2->units.count; i++)
	{
		unit *u = (unit *)p2->units[i];
		if (!cansee(p, u))
			continue;
		int h = u->hex;
		int x = htox(h);
		int y = htoy(h);

		space(2);
		write(u->id, 8);
		write(unittypes[u->type].name, 9);
		right(x, 4);
		right(y, 4);
		write("  none");
		newline();
	}

	writeline("  ------  ---------  --  --  -----");
	newline();
}

static char *uppercase(char *s)
{
	static char s2[wordsize];
	strcpy(s2, s);
	s2[0] = toupper(s2[0]);
	return s2;
}

static void reportunitdetails(player *p)
{
	underline("Unit details");

	int i = 0;
	int j = 0;
	while (i < p->units.count || j < p->removedunits.count)
	{
		unit *ui = 0;
		if (i < p->units.count)
			ui = (unit *)p->units[i];
		unit *uj = 0;
		if (j < p->removedunits.count)
			uj = (unit *)p->removedunits[j];

		unit *u;
		if (!ui)
		{
			u = uj;
			j++;
		}
		else if (!uj)
		{
			u = ui;
			i++;
		}
		else if (ui->id < uj->id)
		{
			u = ui;
			i++;
		}
		else
		{
			u = uj;
			j++;
		}

		writeline(nameid(u));
		reportevents(u);

		if (u->removed)
			continue;

		item("Type", 12, uppercase(unittypes[u->type].name));
		item("Location", 12, nameid(u->hex));
		item("Grouped with", 12, "None");
		newline();
	}
}

static void reporthexsummary(player *p)
{
	if (!p->units.count)
		return;

	writeline("Hex summary");
	writeline("   x   y  terrain   city");
	writeline("  --  --  --------  ----");

	for (int y = 0; y < mapsize; y++)
		for (int x = 0; x < mapsize; x++)
		{
			int h = xytoh(x, y);
			if (!cansee(p, h))
				continue;
			int t = hexes[h].terrain;

			right(x, 4);
			right(y, 4);
			space(2);
			write(terrains[t].name, 10);
			if (!t)
			{
				writeline("n/a");
				continue;
			}
			if (cityarea(h))
				writeline("yes");
			else
				writeline("no");
		}

	writeline("  --  --  --------  ----");
	newline();
}

static void reporthexdetails(player *p, int h)
{
	int found = 0;
	int i;
	for (i = 0; i < hexes[h].events.count; i++)
	{
		hexevent *he = (hexevent *)hexes[h].events[i];
		if (he->players.contains(p))
		{
			found = 1;
			break;
		}
	}

	if (cansee(p, h))
		for (i = 0; i < units.count; i++)
		{
			unit *u = (unit *)units[i];
			if (u->hex == h)
			{
				found = 1;
				break;
			}
		}

	if (!found)
		return;

	writeline(nameid(h));

	found = 0;
	for (i = 0; i < hexes[h].events.count; i++)
	{
		hexevent *he = (hexevent *)hexes[h].events[i];
		if (he->players.contains(p))
		{
			reportevent(he->event);
			found = 1;
		}
	}
	if (found)
		newline();

	if (!cansee(p, h))
		return;

	found = 0;
	for (i = 0; i < playerscount; i++)
	{
		player *p2 = players[i];
		for (int j = 0; j < p2->units.count; j++)
		{
			unit *u = (unit *)p2->units[j];
			if (u->hex != h)
				continue;

			space(2);
			if (p2 == p)
				write("* ");
			else
				write("- ");
			writeline(namepidtype(u));

			found = 1;
		}
	}
	if (found)
		newline();

	char *s;
	if (cityarea(h))
		s = "Yes";
	else
		s = "No";
	if (!hexes[h].terrain)
		s = "N/A";
	item("City area", 9, s);
	newline();
}

static void reporthexdetails(player *p)
{
	underline("Hex details");

	for (int h = 0; h < hexescount; h++)
		reporthexdetails(p, h);
}

static void templateitem(char *caption, int n, char *co)
{
	write(caption);
	write(" ");
	write(n);
	write("  # ");
	writeline(co);
	newline();
}

static void reporttemplate(player *p)
{
	if (!p->units.count)
		return;

	underline("Order template");

	templateitem("player", p->id, p->name);
	for (int i = 0; i < p->units.count; i++)
	{
		unit *u = (unit *)p->units[i];
		sprintf(line, "%s in %s", u->name, nameid(u->hex));
		templateitem("unit", u->id, line);
	}
	writeline("end");
}

static void report(player *p)
{
	char filename[20];
	sprintf(filename, "%d.r", p->id);
	file = fopen(filename, "w");
	if (!file)
		error("Unable to create report file");

	reportmsg();
	reportheader(p);
	reporttotals();
	reportgeneralorders(p);
	reportplayersummary(p);
	reportplayerdetails(p);
	for (int i = 0; i < playerscount; i++)
		reportunitsummary(p, players[i]);
	reportunitdetails(p);
	reporthexsummary(p);
	reporthexdetails(p);
	reporttemplate(p);
	if (!p->units.count)
		write("Unfortunately your empire has been eliminated from the game.\n"
			"Hope you enjoyed playing; condolences on your ill fortune,\n"
			"and better luck next time.\n");

	fclose(file);
}

static void send()
{
	file = fopen("send.bat", "w");
	if (!file)
		error("Unable to create send file");

	for (int i = 0; i < playerscount; i++)
	{
		player *p = players[i];
		if (p->email[0])
			fprintf(file, "sndmail -f orders@somewhere.com -s \"Rainbow's End Turn %d\" -b %d.r -r %s -D somewhere.com -X mail.somewhere.com -u orders -p password\n",
				turn, p->id, p->email);
	}

	fclose(file);
}

void writereports()
{
	memcpy(players2, players, playerscount * sizeof(player *));
	qsort(players2, playerscount, sizeof(player *), cmp);
	for (int i = 0; i < playerscount; i++)
		report(players[i]);
	send();
}
