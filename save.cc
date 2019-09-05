#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

static FILE *file;

static void write(char *s)
{
	fprintf(file, "\"%s\"", s);
}

static void write(int n)
{
	fprintf(file, "%d", n);
}

static void space(int n = 1)
{
	for (int i = 0; i < n; i++)
		fputc(' ', file);
}

static void newline()
{
	fputc('\n', file);
}

static void comment(char *co)
{
	fprintf(file, "# %s\n", co);
}

static void write(int n, char *co)
{
	write(n);
	space(2);
	comment(co);
}

void save()
{
	file = fopen("game", "w");
	if (!file)
		error("Unable to create save game file");

	write(turn, "turn");

	newline();

	write(playerscount, "players");
	comment("number, name, email, money, last orders");
	int i;
	for (i = 0; i < playerscount; i++)
	{
		player *p = players[i];
		write(p->id);
		space();
		write(p->name);
		space();
		write(p->email);
		space();
		write(p->money);
		space();
		write(p->lastorders);
		newline();
	}

	newline();

	write(mapsize, "map size");
	comment("terrain");
	for (int h = 0; h < hexescount; h++)
	{
		write(hexes[h].terrain);
		newline();
	}

	newline();

	write(units.count, "units");
	comment("number, name, type, player, hex");
	for (i = 0; i < units.count; i++)
	{
		unit *u = (unit *)units[i];
		write(u->id);
		space();
		write(u->name);
		space();
		write(u->type);
		space();
		write(u->player->id);
		space();
		write(u->hex);
		newline();
	}

	newline();

	int n = 0;
	for (i = 0; i < playerscount; i++)
		n += players[i]->friendly.count;
	write(n, "friendly");
	comment("player, other");
	for (i = 0; i < playerscount; i++)
	{
		player *p = players[i];
		for (int j = 0; j < p->friendly.count; j++)
		{
			player *p2 = (player *)p->friendly[j];
			write(p->id);
			space();
			write(p2->id);
			newline();
		}
	}

	fclose(file);
}

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

void load()
{
	file = fopen("game", "r");
	if (!file)
		error("Save game file not found");

	read();
	turn = atoi(words[0]);

	read();
	playerscount = atoi(words[0]);
	players = new player *[playerscount];
	players2 = new player *[playerscount];
	int i;
	for (i = 0; i < playerscount; i++)
	{
		read();
		player *p = new player;
		p->id = atoi(words[0]);
		strcpy(p->name, words[1]);
		strcpy(p->email, words[2]);
		p->money = atoi(words[3]);
		p->lastorders = atoi(words[4]);
		players[i] = p;
	}

	read();
	mapsize = atoi(words[0]);
	hexescount = mapsize * mapsize;
	hexes = new hex[hexescount];
	for (int h = 0; h < hexescount; h++)
	{
		read();
		hexes[h].terrain = atoi(words[0]);
	}

	read();
	int n = atoi(words[0]);
	for (i = 0; i < n; i++)
	{
		read();
		unit *u = new unit;
		u->id = atoi(words[0]);
		strcpy(u->name, words[1]);
		u->type = atoi(words[2]);
		u->player = findplayer(atoi(words[3]));
		u->hex = atoi(words[4]);
		addunit(u);
	}

	read();
	n = atoi(words[0]);
	for (i = 0; i < n; i++)
	{
		read();
		player *p = findplayer(atoi(words[0]));
		player *p2 = findplayer(atoi(words[1]));
		p->friendly.add(p2);
	}

	fclose(file);
}
