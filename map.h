/*
	A note on coordinate systems.
	The game uses a total of four coordinate systems in different contexts.
	These are:

	H: A number from 0 to hexescount - 1.
	Useful for iterating through all hexes
	or referring to an arbitrary one irrespective of location.

	X,Y: The usual Cartesian coordinates.

	A,B: Axes inclined at 60 degrees rather than 90.
	This system makes the distance formula simpler.

	D: A direction from 0 to 5 counting clockwise from north.
*/

enum
{
	t_water,
	t_plain,
	t_forest,
	t_mountain,

	terrainscount
};

struct terrain
{
	char *name;
	int movementcost;
};

struct hex
{
	int terrain;
	array units;
	array events;
};

struct hexevent
{
	char *event;
	array players;
};

extern terrain terrains[];

extern hex *hexes;
extern int hexescount;

void initmap();

int findterrain(char *s);
int findhex(char *s);

char *idstr(int h);
char *nameid(int h);

int onmap(int x, int y);
int xytoh(int x, int y);
int htox(int h);
int htoy(int h);

int displace(int h, int d);
int distance(int h1, int h2);

void event(int h, char *s);
void event(int h, char *s, char *s2);
void event(int h, char *s, char *s2, char *s3);
void event(int h, char *s, char *s2, char *s3, char *s4);
void event(int h, char *s, char *s2, char *s3, char *s4, char *s5);
