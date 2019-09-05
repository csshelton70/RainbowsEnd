enum
{
	u_city,
	u_settlers,

	u_infantry,
	u_tanks,
	u_artillery,
};

struct unittype
{
	char *name;
	int buildcost;
	int upkeepcost;
	int movement;
	int attack;
	int defense;
	int range;
};

struct unit: entity
{
	int type;
	player *player;

	int hex;

	char movement;
	char special;

	char removed;

	unit()
	{
		removed = 0;
	}
};

extern unittype unittypes[];
extern array units;

void initunits();
void addunit(unit *u);

int findunittype(char *s);
unit *findunit(player *p, int id);

char *pid(unit *u);
char *namepid(unit *u);
char *namepidtype(unit *u);

int nearestcity(int h, unit *c = 0);

void moveunit(unit *u, int h);
void removeunit(unit *u);
