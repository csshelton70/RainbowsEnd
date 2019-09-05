struct menu
{
	char *caption;
	void (*onclick)();
	menu *submenu;
};

void *makemenu(menu *m, array &items);
