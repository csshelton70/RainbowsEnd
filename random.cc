#include <time.h>
#include "main.h"

static unsigned data[31];
static int current;
static int last;

void initrandom()
{
	data[0] = time(0);
	int i;
	for (i = 1; i < 31; i++)
		data[i] = (data[i - 1] * 1103515245) + 12345;
	current = 3;
	last = 0;
	for (i = 0; i < 1000; i++)
		random();
}

int random()
{
	current = (current + 1) % 31;
	last = (last + 1) % 31;
	data[current] += data[last];
	return data[current] >> 1;
}
