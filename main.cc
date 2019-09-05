#include <new.h>
#include <windows.h>
#include "main.h"

static int __cdecl newhandler(size_t bytes)
{
	error("Out of memory");
	return 0;
}

static int messageloop()
{
	MSG msg;
	int status;
	while ((status = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		if (status == -1)
			return -1;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, char *cmdParam, int cmdShow)
{
	_set_new_handler(newhandler);
	initrandom();
	initwindow(instance);
	return messageloop();
}
