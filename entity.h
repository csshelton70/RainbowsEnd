struct entity
{
	int id;
	char name[wordsize];

	array orders;
	array events;
};

char *nameid(entity *e);

void quote(entity *e, char *s, char *s2);
void quote(entity *e, char *s, char args[][wordsize]);

void event(entity *e, char *s);
void event(entity *e, char *s, char *s2);
void event(entity *e, char *s, char *s2, char *s3);
void event(entity *e, char *s, char *s2, char *s3, char *s4);
void event(entity *e, char *s, char *s2, char *s3, char *s4, char *s5);
