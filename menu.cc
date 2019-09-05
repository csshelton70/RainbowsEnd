#include <windows.h>
#include "main.h"

static int nextid;

static void *makesubmenu(menu *m, array &items)
{
	HMENU M = CreatePopupMenu();
	for (menu *m2 = m->submenu; m2->caption; m2++)
		if (m2->caption[0] == 0)
			AppendMenu(M, MF_SEPARATOR, 0, 0);
		else if (!m2->submenu)
		{
			AppendMenu(M, MF_STRING, nextid++, m2->caption);
			items.add(m2);
		}
		else
			AppendMenu(M, MF_POPUP | MF_STRING, (UINT)makesubmenu(m2, items), m2->caption);
	return M;
}

void *makemenu(menu *m, array &items)
{
	HMENU M = CreateMenu();
	for (; m->caption; m++)
		AppendMenu(M, MF_POPUP | MF_STRING, (UINT)makesubmenu(m, items), m->caption);
	return M;
}
