#include <windows.h>
#include "main.h"

static void open(char *filename)
{
	if ((int)ShellExecute(wnd, "open", filename, 0, 0, 0) <= 32)
		error("Unable to open file");
}

static void run(char *program, char *arg)
{
	if ((int)ShellExecute(wnd, "open", program, arg, 0, SW_SHOWNORMAL) <= 32)
		error("Unable to open file");
}

void helpreadme()
{
	run("notepad.exe", "readme.txt");
}

void helprules()
{
	open("rules.html");
}

void helphistory()
{
	open("history.html");
}

void helplicense()
{
	run("notepad.exe", "license.txt");
}

void helpabout()
{
	showmessage("About Rainbow's End",
		"Rainbow's End version 1.2\n"
		"Copyright 2001 by Russell Wallace\n"
		"This program is free software.\n"
		"See license for details.");
}
