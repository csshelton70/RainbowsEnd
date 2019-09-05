#include <stdio.h>
#include <string.h>
#include "main.h"

static char buf[linesize];

char *nameid(entity *e)
{
	static char s[wordsize + 20];
	sprintf(s, "%s [%d]", e->name, e->id);
	return s;
}

void quote(entity *e, char *s, char *s2)
{
	sprintf(buf, "%d: > %s %s", slot, s, s2);
	e->events.add(copy(buf));
}

void quote(entity *e, char *s, char args[][wordsize])
{
	sprintf(buf, "%d: > %s", slot, s);
	for (int i = 0; i < maxargs; i++)
	{
		char *arg = args[i];
		if (arg[0])
		{
			strcat(buf, " ");
			if (strchr(arg, ' '))
			{
				strcat(buf, "\"");
				strcat(buf, arg);
				strcat(buf, "\"");
				continue;
			}
			strcat(buf, arg);
		}
	}
	e->events.add(copy(buf));
}

void event(entity *e, char *s)
{
	sprintf(buf, "%d: %s.", slot, s);
	e->events.add(copy(buf));
}

void event(entity *e, char *s, char *s2)
{
	sprintf(buf, "%d: %s %s.", slot, s, s2);
	e->events.add(copy(buf));
}

void event(entity *e, char *s, char *s2, char *s3)
{
	sprintf(buf, "%d: %s %s %s.", slot, s, s2, s3);
	e->events.add(copy(buf));
}

void event(entity *e, char *s, char *s2, char *s3, char *s4)
{
	sprintf(buf, "%d: %s %s %s %s.", slot, s, s2, s3, s4);
	e->events.add(copy(buf));
}

void event(entity *e, char *s, char *s2, char *s3, char *s4, char *s5)
{
	sprintf(buf, "%d: %s %s %s %s %s.", slot, s, s2, s3, s4, s5);
	e->events.add(copy(buf));
}
