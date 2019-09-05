#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "main.h"

char *copy(char *s)
{
	char *cs = new char[strlen(s) + 1];
	strcpy(cs, s);
	return cs;
}

void error(char *s)
{
	MessageBox(0, s, "Error", MB_ICONHAND);
	exit(EXIT_FAILURE);
}

void showmessage(char *caption, char *s)
{
	MessageBox(0, s, caption, MB_ICONINFORMATION);
}

void shuffle(void *a, int n)
{
	void **a1 = (void **)a;
	for (int i = n - 1; i > 0; i--)
	{
		int j = random() % (i + 1);
		void *t = a1[i];
		a1[i] = a1[j];
		a1[j] = t;
	}
}
