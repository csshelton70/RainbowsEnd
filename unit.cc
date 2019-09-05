#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "main.h"

unittype unittypes[] =
{
	{"city", 20, -10},
	{"settlers", 10, 1, 3, 0, 2},

	{"infantry", 10, 1, 3, 4, 4},
	{"tanks", 20, 2, 6, 8, 6},
	{"artillery", 20, 2, 3, 1, 4, 2},
};

array units;

static void deletecities()
{
	for (int i = 0; i < units.count; i++)
	{
		unit *u = (unit *)units[i];
		u->player->units.count = 0;
		hexes[u->hex].units.count = 0;
		delete u;
	}
	units.count = 0;
}

static int initcities()
{
	// Create one city per player

	int i;
	for (i = 0; i < playerscount; i++)
	{
		unit *c = new unit;
		c->id = 1;
		strcpy(c->name, "City");
		c->type = u_city;
		c->player = players[i];
		do
			c->hex = random() % hexescount;
		while (!hexes[c->hex].terrain);
		addunit(c);
	}

	// Separate them as far as possible

	int more;
	do
	{
		more = 0;
		for (i = 0; i < playerscount; i++)
		{
			unit *c = (unit *)units[i];
			int r = nearestcity(c->hex, c);
			int a[6];
			int j = 0;
			for (int d = 0; d < 6; d++)
			{
				int h = displace(c->hex, d);
				if (h < 0)
					continue;
				if (!hexes[h].terrain)
					continue;

				if (nearestcity(h, c) > r)
					a[j++] = h;
			}
			if (j == 0)
				continue;

			moveunit(c, a[random() % j]);
			more = 1;
		}
	}
	while (more);

	// Check if they're now adequately separated

	for (i = 0; i < playerscount; i++)
	{
		unit *c = (unit *)units[i];
		if (nearestcity(c->hex, c) < cityseparation)
		{
			deletecities();
			return 0;
		}
	}
	return 1;
}

void initunits()
{
	for (int i = 0; i < 10; i++)
		if (initcities())
			return;
	error("Unable to place starting cities");
}

void addunit(unit *u)
{
	units.add(u);
	hexes[u->hex].units.add(u);

	player *p = u->player;
	if (!p->units.count)
	{
		p->units.add(u);
		return;
	}
	unit *u2 = (unit *)p->units[p->units.count - 1];
	if (u2->id < u->id)
	{
		p->units.add(u);
		return;
	}
	for (int i = 0;; i++)
	{
		u2 = (unit *)p->units[i];
		if (u2->id > u->id)
		{
			p->units.insert(i, u);
			return;
		}
	}
}

int findunittype(char *s)
{
	if (strlen(s) < 3)
		return -1;
	for (int i = 0; i < sizeof unittypes / sizeof(char *); i++)
		if (!memicmp(s, unittypes[i].name, 3))
			return i;
	return -1;
}

unit *findunit(player *p, int id)
{
	for (int i = 0; i < p->units.count; i++)
	{
		unit *u = (unit *)p->units[i];
		if (u->id == id)
			return u;
	}
	return 0;
}

char *pid(unit *u)
{
	static char s[40];
	sprintf(s, "[%d-%d]", u->player->id, u->id);
	return s;
}

char *namepid(unit *u)
{
	static char s[wordsize + 40];
	sprintf(s, "%s [%d-%d]", u->name, u->player->id, u->id);
	return s;
}

char *namepidtype(unit *u)
{
	static char s[wordsize + 100];
	char *type = unittypes[u->type].name;
	sprintf(s, "%s [%d-%d] (%s)", u->name, u->player->id, u->id, type);
	return s;
}

int nearestcity(int h, unit *c)
{
	int r = INT_MAX;
	for (int i = 0; i < units.count; i++)
	{
		unit *c2 = (unit *)units[i];
		if (c2->type != u_city)
			continue;
		if (c2 == c)
			continue;

		int r2 = distance(h, c2->hex);
		r = min(r, r2);
	}
	return r;
}

void moveunit(unit *u, int h)
{
	hexes[u->hex].units.remove(u);
	u->hex = h;
	hexes[h].units.add(u);
}

void removeunit(unit *u)
{
	units.remove(u);
	u->player->units.remove(u);
	u->player->removedunits.add(u);
	hexes[u->hex].units.remove(u);
	u->removed = 1;
}
