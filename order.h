const int maxorders = 100;
const int maxargs = 3;

enum
{
	o_build,
	o_drop,
	o_email,
	o_fire,
	o_friendly,
	o_give,
	o_group,
	o_hostile,
	o_move,
	o_name,
	o_null,
	o_quit,
	o_ungroup,
	o_wait,
};

struct order
{
	int type;
	char args[maxargs][wordsize];
};

void readorders();
void adjustorders();

void doorder(player *p, order *o);
void doorder(unit *u, order *o);
