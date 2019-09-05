#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

terrain terrains[] =
{
	{"water"},
	{"plain", 1},
	{"forest", 2},
	{"mountain", 3},
};

hex *hexes;
int hexescount;

static char buf[linesize];

static int offshore(int h)
{
	if (hexes[h].terrain)
		return 0;
	for (int d = 0; d < 6; d++)
	{
		int h2 = displace(h, d);
		if (h2 >= 0 && hexes[h2].terrain)
			return 1;
	}
	return 0;
}

void initmap()
{
	hexescount = mapsize * mapsize;
	hexes = new hex[hexescount];

	for (int h = 0; h < hexescount; h++)
		hexes[h].terrain = t_water;
	hexes[random() % hexescount].terrain = t_mountain;

	int *a = new int[hexescount];

	int n = hexescount / 2 - 1;
	for (int i = 0; i < n; i++)
	{
		int j = 0;
		for (int h = 0; h < hexescount; h++)
			if (offshore(h))
				a[j++] = h;
		h = a[random() % j];
		switch(random() % 4)
		{
		case 0:
			hexes[h].terrain = t_mountain;
			break;
		case 1:
			hexes[h].terrain = t_forest;
			break;
		default:
			hexes[h].terrain = t_plain;
		}
	}

	delete[] a;
}

int findterrain(char *s)
{
	for (int i = 0; i < terrainscount; i++)
		if (!stricmp(terrains[i].name, s))
			return i;
	return -1;
}

int findhex(char *s)
{
	int x = atoi(s);
	char *s2 = strchr(s, '/');
	if (!s2)
		s2 = strchr(s, ',');
	if (!s2)
		return -1;
	int y = atoi(s2 + 1);
	if (!onmap(x, y))
		return -1;
	return xytoh(x, y);
}

char *idstr(int h)
{
	static char s[40];
	int x = htox(h);
	int y = htoy(h);
	sprintf(s, "[%d/%d]", x, y);
	return s;
}

char *nameid(int h)
{
	static char s[wordsize + 20];
	terrain *t = &terrains[hexes[h].terrain];
	int x = htox(h);
	int y = htoy(h);
	sprintf(s, "%s [%d/%d]", t->name, x, y);
	s[0] = toupper(s[0]);
	return s;
}

int onmap(int x, int y)
{
	return x >= 0 && x < mapsize &&
		y >= 0 && y < mapsize;
}

int xytoh(int x, int y)
{
	return y * mapsize + x;
}

int htox(int h)
{
	return h % mapsize;
}

int htoy(int h)
{
	return h / mapsize;
}

int displace(int h, int d)
{
	int x = htox(h);
	int y = htoy(h);

	switch (d)
	{
	case 0:
		y--;
		break;
	case 1:
		if (even(x))
			y--;
		x++;
		break;
	case 2:
		if (odd(x))
			y++;
		x++;
		break;
	case 3:
		y++;
		break;
	case 4:
		if (odd(x))
			y++;
		x--;
		break;
	case 5:
		if (even(x))
			y--;
		x--;
		break;
	}

	if (!onmap(x, y))
		return -1;
	return xytoh(x, y);
}

static int htoa(int h)
{
	int x = htox(h);
	int y = htoy(h);
	return (x+1) / 2 + y;
}

static int htob(int h)
{
	int x = htox(h);
	int y = htoy(h);
	return x / 2 - y;
}

int distance(int h1, int h2)
{
	int a1 = htoa(h1);
	int b1 = htob(h1);

	int a2 = htoa(h2);
	int b2 = htob(h2);

	int da = a1 - a2;
	int db = b1 - b2;

	int s = (sign(da) == sign(db));

	da = abs(da);
	db = abs(db);

	if (s)
		return da + db;
	else
		return max(da, db);
}

void event(int h, char *s)
{
	static char buf[linesize];
	sprintf(buf, "%d: %s.", slot, s);
	hexevent *he = new hexevent;
	he->event = copy(buf);
	for (int i = 0; i < playerscount; i++)
		if (cansee(players[i], h))
			he->players.add(players[i]);
	hexes[h].events.add(he);
}

void event(int h, char *s, char *s2)
{
	sprintf(buf, "%s %s", s, s2);
	event(h, buf);
}

void event(int h, char *s, char *s2, char *s3)
{
	sprintf(buf, "%s %s %s", s, s2, s3);
	event(h, buf);
}

void event(int h, char *s, char *s2, char *s3, char *s4)
{
	sprintf(buf, "%s %s %s %s", s, s2, s3, s4);
	event(h, buf);
}

void event(int h, char *s, char *s2, char *s3, char *s4, char *s5)
{
	sprintf(buf, "%s %s %s %s %s", s, s2, s3, s4, s5);
	event(h, buf);
}
