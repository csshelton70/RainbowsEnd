struct player: entity
{
	char email[wordsize];

	int money;
	int lastorders;
	array friendly;
	array units;

	array removedunits;
};

extern player **players;
extern player **players2;
extern int playerscount;

void initplayers();
player *findplayer(int id);
void removeplayers();
