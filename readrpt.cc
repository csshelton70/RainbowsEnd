#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

static FILE *file;

static void read()
{
	fgets(line, linesize, file);
	if (!feof(file))
		parse();
}

static void readmapsize()
{
	do
	{
		read();
		if (feof(file))
			return;
	}
	while (memcmp(line, "  Map size:", 11));
	int n = atoi(words[2]);

	if (n != mapsize || !hexes)
	{
		delete[] hexes;
		hexescount = mapsize * mapsize;
		hexes = new hex[hexescount];
	}

	for (int h = 0; h < hexescount; h++)
		hexes[h].terrain = -1;
}

static void readhexsummary()
{
	do
	{
		read();
		if (feof(file))
			return;
	}
	while (memcmp(line, "Hex summary", 11));
	read();
	read();

	for (read(); isdigit(words[0][0]); read())
	{
		int x = atoi(words[0]);
		int y = atoi(words[1]);
		if (!onmap(x, y))
			continue;
		int h = xytoh(x, y);
		int t = findterrain(words[2]);
		if (t >= 0)
			hexes[h].terrain = t;
	}
}

void readreport()
{
	file = fopen("report", "r");
	if (!file)
	{
		showmessage("Error", "Report file not found");
		return;
	}

	readmapsize();
	if (feof(file))
	{
		showmessage("Error", "Map size not found");
		goto end;
	}
	readhexsummary();
	if (feof(file))
		showmessage("Error", "Hex summary not found");

end:
	fclose(file);
	redraw();
}
