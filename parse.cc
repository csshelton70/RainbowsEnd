#include <ctype.h>
#include <string.h>
#include "main.h"

const int wordsmax = 10;

char line[linesize];
char words[wordsmax][wordsize];

static int issep(int c)
{
	if (!c)
		return 0;
	if (c < 33 || c > 126)
		return 1;
	if (c == '[' || c == ']' ||
		c == '<' || c == '>' ||
		c == '_')
		return 1;
	return 0;
}

static void clean(char *s)
{
	int i = 0;
	int j = 0;
	for (;;)
	{
		while (issep(s[i]))
			i++;
		if (!s[i] && j)
			j--;
		while (s[i] && !issep(s[i]))
			s[j++] = s[i++];
		if (!s[i])
		{
			s[j] = 0;
			break;
		}
		s[j++] = ' ';
	}
}

void parse()
{
	int i = strlen(line);
	if (i >= 4 && !strcmp(line + i - 4, "=20\n"))
		line[i - 4] = 0;

	int w = 0;

	i = 0;
	while (w < wordsmax)
	{
		int j = 0;
		while (isspace(line[i]))
			i++;
		if (!line[i] || line[i] == '#')
			break;
		if (line[i] == '"')
		{
			i++;
			while (line[i] && line[i] != '"')
			{
				if (j >= wordsize - 1)
					break;
				words[w][j++] = line[i++];
			}
			i++;
		}
		else
			while (line[i] && !isspace(line[i]))
			{
				if (j >= wordsize - 1)
					break;
				words[w][j++] = line[i++];
			}
		words[w][j] = 0;
		clean(words[w]);
		w++;
	}

	while (w < wordsmax)
		words[w++][0] = 0;
}
