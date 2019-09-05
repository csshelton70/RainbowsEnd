#include <string.h>
#include "main.h"

// The opponent AI module. It handles empires that have no human player.
// At the moment all it does is build infantry units at cities.

static void decideorders(player *p)
{
	for (int i = 0; i < p->units.count; i++)
	{
		unit *u = (unit *)p->units[i];
		if (u->type == u_city)
		{
			order *o = new order;
			o->type = o_build;
			strcpy(o->args[0], "infantry");
			o->args[1][0] = 0;
			o->args[2][0] = 0;
			u->orders.add(o);
		}
	}
}

void decideorders()
{
	for (int i = 0; i < playerscount; i++)
	{
		player *p = players[i];
		if (!p->email[0] || p->lastorders < turn - 1)
			decideorders(p);
	}
}
