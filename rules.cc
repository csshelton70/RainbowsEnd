#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

int cansee(player *p, int h)
{
	for (int i = 0; i < p->units.count; i++)
	{
		unit *u = (unit *)p->units[i];
		if (distance(u->hex, h) <= sightingdistance)
			return 1;
	}
	return 0;
}

int cansee(player *p, unit *u)
{
	return cansee(p, u->hex);
}

int cityarea(int h)
{
	return nearestcity(h) < cityseparation;
}

void refreshunits()
{
	for (int i = 0; i < units.count; i++)
	{
		unit *u = (unit *)units[i];
		u->movement = unittypes[u->type].movement;
		u->special = 1;
	}
}

static int consecutive(player *p)
{
	if (!p->units.count)
		return 1;
	unit *u = (unit *)p->units[p->units.count - 1];
	if (u->id == p->units.count)
		return 1;
	return 0;
}

static int has(player *p, int id)
{
	for (int i = 0; i < p->units.count; i++)
	{
		unit *u = (unit *)p->units[i];
		if (u->id == id)
			return 1;
	}
	return 0;
}

static void nextid(unit *u)
{
	player *p = u->player;
	if (consecutive(p))
	{
		u->id = p->units.count + 1;
		return;
	}

	int id = u->id;
	while (has(p, id))
		id++;
	u->id = id;
}

static int finddir(char *s)
{
	static char *abbrev[] =
	{
		"n",
		"ne",
		"se",
		"s",
		"sw",
		"nw",
	};

	static char *full[] =
	{
		"north",
		"northeast",
		"southeast",
		"south",
		"southwest",
		"northwest",
	};

	int i;
	for (i = 0; i < 6; i++)
		if (!stricmp(s, abbrev[i]))
			return i;

	for (i = 0; i < 6; i++)
		if (!stricmp(s, full[i]))
			return i;

	return -1;
}

static int destroys(unit *u, unit *u2)
{
	int attack = unittypes[u->type].attack;
	int defense = unittypes[u2->type].defense;
	return random() % (attack + defense) < attack;
}

void build(unit *u, char args[][wordsize])
{
	player *p = u->player;

	int type = findunittype(args[0]);
	if (!args[0][0] && u->type == u_settlers)
		type = u_city;
	if (type < 0)
	{
		event(u, "Unit type not recognized");
		return;
	}
	switch (u->type)
	{
	case u_city:
		if (type == u_city)
		{
			event(u, "Cities can only be built by settlers");
			return;
		}
		break;
	case u_settlers:
		if (type != u_city)
		{
			event(u, "Settlers can only build cities");
			return;
		}
		break;
	default:
		event(u, "Only cities and settlers can build");
		return;
	}
	if (type == u_city && cityarea(u->hex))
	{
		event(u, "Too close to an existing city");
		return;
	}

	int id = atoi(args[1]);
	if (id < 1 || id > 99999)
		id = 1;

	if (isalpha(args[1][0]) && !args[2][0])
		strcpy(args[2], args[1]);
	char *name = args[2];
	if (!name[0])
		name = unittypes[type].name;

	int cost = unittypes[type].buildcost;
	if (p->money < cost)
	{
		event(u, "Insufficient funds");
		return;
	}

	p->money -= cost;

	unit *u2 = new unit;
	u2->id = id;
	strcpy(u2->name, name);
	u2->name[0] = toupper(u2->name[0]);
	u2->type = type;
	u2->player = p;
	u2->hex = u->hex;
	nextid(u2);
	addunit(u2);

	if (u->type == u_settlers)
		removeunit(u);

	event(u, "Built", nameid(u2));
	event(u2, "Built in", nameid(u2->hex));
	event(u2->hex, namepidtype(u2), "built");
}

void drop(unit *u)
{
	removeunit(u);

	event(u, "Dropped");
	event(u->hex, namepidtype(u), "dropped");
}

void email(player *p, char args[][wordsize])
{
	char *email = args[0];
	if (!email[0])
	{
		event(p, "New email address not given");
		return;
	}

	strcpy(p->email, email);
	event(p, "Email address changed to", email);
}

void fire(unit *u, char args[][wordsize])
{
	int range = unittypes[u->type].range;
	if (range == 0)
	{
		event(u, "Not an indirect fire unit");
		return;
	}
	if (!u->special)
	{
		event(u, "Already fired this turn");
		return;
	}

	int h;
	int d = finddir(args[0]);
	if (d >= 0)
	{
		h = displace(u->hex, d);
		if (h < 0)
		{
			event(u, "Target hex is off the map");
			return;
		}
	}
	else
	{
		h = findhex(args[0]);
		if (h < 0)
		{
			event(u, "Target hex not recognized");
			return;
		}
	}
	if (distance(u->hex, h) > range)
	{
		event(u, "Target hex is out of range");
		return;
	}

	int i;
	for (i = 0; i < hexes[h].units.count; i++)
	{
		unit *u2 = (unit *)hexes[h].units[i];
		if (u2->player == u->player)
			break;
		if (u->player->friendly.contains(u2->player))
			break;
	}
	if (i < hexes[h].units.count)
	{
		event(u, "Friendly units in target area - fire mission aborted");
		return;
	}

	u->special = 0;

	event(u, "Firing on", nameid(h));
	event(h, "Incoming fire from", namepidtype(u), "in", nameid(u->hex));

	if (!hexes[h].units.count)
	{
		event(u, "No units in target area");
		event(h, "No units in target area");
		return;
	}

	for (i = 0; i < hexes[h].units.count; i++)
	{
		unit *u2 = (unit *)hexes[h].units[i];

		if (u2->type == u_city)
		{
			event(u, namepidtype(u2), "survived");
			event(u2, "Incoming fire from", namepidtype(u), "in", nameid(u->hex), "- survived");
			event(h, namepidtype(u2), "survived");
			continue;
		}

		if (!destroys(u, u2))
		{
			event(u, namepidtype(u2), "survived");
			event(u2, "Incoming fire from", namepidtype(u), "in", nameid(u->hex), "- survived");
			event(h, namepidtype(u2), "survived");
			continue;
		}

		event(u, namepidtype(u2), "destroyed");
		event(u2, "Incoming fire from", namepidtype(u), "in", nameid(u->hex), "- destroyed");
		event(h, namepidtype(u2), "destroyed");

		removeunit(u2);
		i--;
	}
}

void friendly(player *p, char args[][wordsize])
{
	player *p2 = findplayer(atoi(args[0]));
	if (!p2)
	{
		event(p, "Player not recognized");
		return;
	}

	if (!p->friendly.contains(p2))
		p->friendly.add(p2);

	event(p, "Declared", nameid(p2), "as friendly");
}

void give(player *p, char args[][wordsize])
{
	player *p2 = findplayer(atoi(args[0]));
	if (!p2)
	{
		event(p, "Player not recognized");
		return;
	}
	int qty = atoi(args[1]);
	qty = max(qty, 1);
	qty = min(qty, p->money);

	p->money -= qty;
	p2->money += qty;

	itoa(qty, line, 10);
	event(p, "Gave", nameid(p2), line, "money");
	event(p2, nameid(p), "gave", line, "money");
}

void group(unit *u, char args[][wordsize])
{
}

void hostile(player *p, char args[][wordsize])
{
	player *p2 = findplayer(atoi(args[0]));
	if (!p2)
	{
		event(p, "Player not recognized");
		return;
	}

	p->friendly.remove(p2);
	event(p, "Declared", nameid(p2), "as hostile");
}

void move(unit *u, char args[][wordsize])
{
	int h;
	int d = finddir(args[0]);
	if (d >= 0)
	{
		h = displace(u->hex, d);
		if (h < 0)
		{
			event(u, "Destination is off the map");
			return;
		}
	}
	else
	{
		h = findhex(args[0]);
		if (h < 0)
		{
			event(u, "Destination not recognized");
			return;
		}
	}
	if (distance(u->hex, h) > 1)
	{
		event(u, "Destination is not an adjacent hex");
		return;
	}

	int t = hexes[h].terrain;
	if (!t)
	{
		event(u, "Destination is a water hex");
		return;
	}

	int cost = terrains[t].movementcost;
	if (u->movement < cost)
	{
		event(u, "Not enough movement points left");
		return;
	}

	int from = u->hex;

	u->movement -= cost;
	moveunit(u, h);

	sprintf(line, "Moved from %s to ", nameid(from));
	strcat(line, nameid(h));
	event(u, line);

	event(from, namepidtype(u), "moved to", idstr(h));
	event(h, namepidtype(u), "arrived from", idstr(from));
}

void name(entity *e, char args[][wordsize])
{
	char *name = args[0];
	if (!name[0])
	{
		event(e, "New name not given");
		return;
	}

	strcpy(e->name, name);
	e->name[0] = toupper(e->name[0]);

	event(e, "Name changed to", name);
}

void quit(player *p)
{
	p->email[0] = 0;
}

void ungroup(unit *u, char args[][wordsize])
{
}

static int cantarget(unit *u, unit *u2)
{
	if (u2->type == u_city)
		return 0;
	if (u->player == u2->player)
		return 0;
	if (u->player->friendly.contains(u2))
		return 0;
	return 1;
}

static unit *findtarget(unit *u)
{
	if (!unittypes[u->type].attack)
		return 0;

	hex *h = &hexes[u->hex];
	int n = 0;
	int i;
	for (i = 0; i < h->units.count; i++)
	{
		unit *u2 = (unit *)h->units[i];
		if (cantarget(u, u2))
			n++;
	}
	if (!n)
		return 0;

	n = random() % n;
	for (i = 0;; i++)
	{
		unit *u2 = (unit *)h->units[i];
		if (!cantarget(u, u2))
			continue;

		if (!n)
			return u2;
		n--;
	}
}

static void combat(int h)
{
	int more;
	do
	{
		more = 0;
		shuffle(hexes[h].units.data, hexes[h].units.count);
		for (int i = 0; i < hexes[h].units.count; i++)
		{
			unit *u = (unit *)hexes[h].units[i];
			unit *u2 = findtarget(u);
			if (!u2)
				continue;
			more = 1;

			if (!destroys(u, u2))
			{
				event(u, "Attacking", namepidtype(u2), "- survived");
				event(u2, "Attacked by", namepidtype(u), "- survived");

				strcpy(line, namepidtype(u));
				event(h, line, "attacks", namepidtype(u2), "- survived");
				continue;
			}

			event(u, "Attacking", namepidtype(u2), "- destroyed");
			event(u2, "Attacked by", namepidtype(u), "- destroyed");

			strcpy(line, namepidtype(u));
			event(h, line, "attacks", namepidtype(u2), "- destroyed");

			removeunit(u2);
			if (hexes[h].units[i] != u)
				i--;
		}
	}
	while (more);
}

void combat()
{
	for (int h = 0; h < hexescount; h++)
		if (hexes[h].units.count > 1)
			combat(h);
}

static void capture(int h)
{
	unit *c = 0;
	int i;
	for (i = 0; i < hexes[h].units.count; i++)
	{
		unit *u = (unit *)hexes[h].units[i];
		if (u->type != u_city)
			continue;

		c = u;
		break;
	}
	if (!c)
		return;

	shuffle(hexes[h].units.data, hexes[h].units.count);
	for (i = 0; i < hexes[h].units.count; i++)
	{
		unit *u = (unit *)hexes[h].units[i];
		if (!unittypes[u->type].attack)
			continue;
		if (u->player == c->player)
			continue;
		if (u->player->friendly.contains(c->player))
			continue;

		unit *c2 = new unit;
		c2->id = 1;
		strcpy(c2->name, c->name);
		c2->type = u_city;
		c2->player = u->player;
		c2->hex = h;
		nextid(c2);
		addunit(c2);

		event(u, "Captured", namepid(c));
		event(c, "Captured by", namepid(u));
		event(c2, "Captured by", namepid(u));
		event(h, namepidtype(u), "captured", namepid(c), "- now designated", pid(c2));

		removeunit(c);
		break;
	}
}

void capture()
{
	for (int h = 0; h < hexescount; h++)
		if (hexes[h].units.count > 1)
			capture(h);
}

void income()
{
	for (int i = 0; i < units.count; i++)
	{
		unit *u = (unit *)units[i];
		int income = -unittypes[u->type].upkeepcost;
		if (income >= 0)
			u->player->money += income;
	}
}

void upkeep()
{
	for (int i = 0; i < playerscount; i++)
	{
		player *p = players[i];
		for (int j = 0; j < p->units.count; j++)
		{
			unit *u = (unit *)p->units[j];
			int upkeep = unittypes[u->type].upkeepcost;
			if (upkeep < 0)
				continue;

			if (p->money >= upkeep)
				p->money -= upkeep;
			else
				removeunit(u);
		}
	}
}
