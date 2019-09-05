#include <string.h>
#include "main.h"

int turn;
int slot;

void newgame()
{
	turn = 0;
	initplayers();
	initmap();
	initunits();

	writereports();
	save();

	redraw();
}

static void playerorders()
{
	for (slot = 1; slot <= maxorders; slot++)
	{
		int si = slot - 1;
		for (int i = 0; i < playerscount; i++)
		{
			player *p = players[i];
			if (si >= p->orders.count)
				continue;
			order *o = (order *)p->orders[si];
			doorder(p, o);
		}
	}
}

static void unitorders(int phase)
{
	int unitscount = units.count;
	unit **units2 = new unit *[unitscount];
	memcpy(units2, units.data, unitscount * sizeof(unit *));
	shuffle(units2, unitscount);

	int si = slot - 1;
	for (int i = 0; i < unitscount; i++)
	{
		unit *u = units2[i];
		if (u->removed)
			continue;
		if (si >= u->orders.count)
			continue;
		order *o = (order *)u->orders[si];

		switch (o->type)
		{
		case o_build:
		case o_drop:
			if (phase != 2)
				continue;
			break;
		case o_move:
			if (phase != 1)
				continue;
			break;
		default:
			if (phase)
				continue;
		}
		doorder(u, o);
	}

	delete[] units2;
}

static void unitorders()
{
	for (slot = 1; slot <= maxorders; slot++)
	{
		unitorders(0);
		unitorders(1);
		combat();
		capture();
		unitorders(2);
	}
}

void runturn()
{
	slot = 0;
	load();
	readorders();
	decideorders();
	adjustorders();

	turn++;
	refreshunits();
	playerorders();
	unitorders();
	income();
	upkeep();

	writereports();
	removeplayers();
	save();

	redraw();
}
