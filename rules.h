int cansee(player *p, int h);
int cansee(player *p, unit *u);
int cityarea(int h);

void refreshunits();

void build(unit *u, char args[][wordsize]);
void drop(unit *u);
void email(player *p, char args[][wordsize]);
void fire(unit *u, char args[][wordsize]);
void friendly(player *p, char args[][wordsize]);
void give(player *p, char args[][wordsize]);
void group(unit *u, char args[][wordsize]);
void hostile(player *p, char args[][wordsize]);
void move(unit *u, char args[][wordsize]);
void name(entity *e, char args[][wordsize]);
void quit(player *p);
void ungroup(unit *u, char args[][wordsize]);

void combat();
void capture();

void income();
void upkeep();
