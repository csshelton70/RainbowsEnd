#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

static FILE *file;

static char *ordertypes[] =
{
	"build",
	"drop",
	"email",
	"fire",
	"friendly",
	"give",
	"group",
	"hostile",
	"move",
	"name",
	"null",
	"quit",
	"ungroup",
	"wait",
};

static void read()
{
	do
	{
		fgets(line, linesize, file);
		if (feof(file))
			break;
		parse();
	}
	while (words[0][0] == 0);
}

static int findtype(char *s)
{
	if (strlen(s) < 3)
		return -1;

	for (int i = 0; i < sizeof ordertypes / sizeof(char *); i++)
		if (!memicmp(ordertypes[i], s, 3))
			return i;
	return -1;
}

static void addorder(entity *e)
{
	int type = findtype(words[0]);
	if (type < 0)
	{
		quote(e, words[0], words + 1);
		event(e, "Order not recognized");
		return;
	}

	if (e->orders.count >= maxorders)
	{
		event(e, "Maximum number of orders reached");
		return;
	}

	order *o = new order;
	o->type = type;
	for (int i = 0; i < maxargs; i++)
		strcpy(o->args[i], words[i + 1]);
	e->orders.add(o);
}

void readorders()
{
	file = fopen("orders", "r");
	if (!file)
		error("Orders file not found");
	read();

	for (;;)
	{
		while (stricmp(words[0], "player"))
		{
			read();
			if (feof(file))
				goto end;
		}

		player *p = findplayer(atoi(words[1]));
		if (!p)
		{
			read();
			if (feof(file))
				goto end;
			continue;
		}

		char *s = strchr(words[1], '/');
		if (s && isalpha(s[1]))
		{
			read();
			if (feof(file))
				goto end;
			continue;
		}

		p->orders.count = 0;
		for (int i = 0; i < p->units.count; i++)
		{
			unit *u = (unit *)p->units[i];
			u->orders.count = 0;
		}
		p->lastorders = turn + 1;

		for (;;)
		{
			read();
			if (feof(file))
				goto end;
			if (!stricmp(words[0], "unit") ||
				!stricmp(words[0], "end") ||
				!stricmp(words[0], "player"))
				break;
			addorder(p);
		}

		while (!stricmp(words[0], "unit"))
		{
			unit *u = findunit(p, atoi(words[1]));
			if (!u)
			{
				quote(p, "unit", words[1]);
				event(p, "You have no such unit");

				do
				{
					read();
					if (feof(file))
						goto end;
				}
				while (stricmp(words[0], "unit") &&
					stricmp(words[0], "end") &&
					stricmp(words[0], "player"));
				continue;
			}

			for (;;)
			{
				read();
				if (feof(file))
					goto end;
				if (!stricmp(words[0], "unit") ||
					!stricmp(words[0], "end") ||
					!stricmp(words[0], "player"))
					break;
				addorder(u);
			}
		}
	}

end:
	fclose(file);
}

static void adjustorders(entity *e)
{
	for (int i = 0; i < e->orders.count; i++)
	{
		order *o = (order *)e->orders[i];
		if (o->type != o_wait)
			continue;

		o->type = o_null;
		while (e->orders.count < maxorders)
		{
			o = new order;
			o->type = o_null;
			e->orders.insert(i + 1, o);
		}
	}
}

void adjustorders()
{
	int i;
	for (i = 0; i < playerscount; i++)
		adjustorders(players[i]);
	for (i = 0; i < units.count; i++)
		adjustorders((unit *)units[i]);
}

void doorder(player *p, order *o)
{
	int type = o->type;
	if (type == o_null)
		return;
	quote(p, ordertypes[type], o->args);

	switch (type)
	{
	case o_email:
		email(p, o->args);
		break;
	case o_friendly:
		friendly(p, o->args);
		break;
	case o_give:
		give(p, o->args);
		break;
	case o_hostile:
		hostile(p, o->args);
		break;
	case o_name:
		name(p, o->args);
		break;
	case o_quit:
		quit(p);
		break;
	default:
		event(p, "That order must be issued for a specific unit");
	}
}

void doorder(unit *u, order *o)
{
	int type = o->type;
	if (type == o_null)
		return;
	quote(u, ordertypes[type], o->args);

	switch (type)
	{
	case o_build:
		build(u, o->args);
		break;
	case o_drop:
		drop(u);
		break;
	case o_email:
		email(u->player, o->args);
		break;
	case o_fire:
		fire(u, o->args);
		break;
	case o_group:
		group(u, o->args);
		break;
	case o_move:
		move(u, o->args);
		break;
	case o_name:
		name(u, o->args);
		break;
	case o_quit:
		quit(u->player);
		break;
	case o_ungroup:
		ungroup(u, o->args);
		break;
	default:
		event(u, "That order must be issued for your empire as a whole");
	}
}
