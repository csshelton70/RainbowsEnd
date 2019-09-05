#include <stdio.h>
#include <string.h>
#include "main.h"

player **players;
player **players2;
int playerscount;

static FILE *file;

static void countplayers()
{
	file = fopen("players", "r");
	if (file == 0)
		error("Players file not found");

	humanplayers = 0;
	for (;;)
	{
		fgets(line, linesize, file);
		if (feof(file))
			break;
		parse();
		if (words[0][0])
			humanplayers++;
	}

	fclose(file);
}

static void readplayers()
{
	file = fopen("players", "r");
	if (file == 0)
		error("Unable to open players file");

	int i = 0;
	for (;;)
	{
		fgets(line, linesize, file);
		if (feof(file))
			break;
		parse();
		if (words[0][0] == 0)
			continue;
		player *p = players[i];

		p->id = i + 1;
		strcpy(p->name, "Player");
		strcpy(p->email, words[0]);
		p->money = startingmoney;
		p->lastorders = 0;
		i++;
	}

	fclose(file);
}

void initplayers()
{
	countplayers();
	playerscount = humanplayers + computerplayers;
	players = new player *[playerscount];
	players2 = new player *[playerscount];
	for (int i = 0; i < playerscount; i++)
		players[i] = new player;
	readplayers();
}

player *findplayer(int id)
{
	for (int i = 0; i < playerscount; i++)
	{
		player *p = players[i];
		if (p->id == id)
			return p;
	}
	return 0;
}

void removeplayers()
{
	for (int i = 0; i < playerscount; i++)
	{
		player *p = players[i];
		if (p->units.count)
			continue;

		for (int j = 0; j < playerscount; j++)
			players[j]->friendly.remove(p);
		memmove(players + i, players + i + 1, (playerscount - i - 1) * sizeof(player *));
		playerscount--;
		i--;
	}
}
