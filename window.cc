#include <math.h>
#include <windows.h>
#include "main.h"

void *wnd;

static int clientwidth;
static int clientheight;

const int scrollx = 0;
const int scrolly = 0;
static int scrollwidth;
static int scrollheight;

//const int mapx = 20;
const int mapx = 1;
const int mapy = mapx;
static int mapwidth;
static int mapheight;

static int textx;
const int texty = 0;
//const int textwidth = 150;
const int textwidth = 1;
static int textheight;

static int overviewx;
static int overviewy;
const int overviewwidth = textwidth;
const int overviewheight = overviewwidth * 4 / 3;

static int hexwidth;
static int hexheight;
static POINT points[6];

static void quit()
{
	PostQuitMessage(0);
}

static menu filemenu[] =
{
	{"&New game", newgame},
	{"&Run turn", runturn},
	{"&Show map", readreport},
	{""},
	{"E&xit", quit},
	0
};

static menu helpmenu[] =
{
	{"&Readme", helpreadme},
	{"R&ules", helprules},
	{"&History", helphistory},
	{"&License", helplicense},
	{""},
	{"&About Rainbow's End", helpabout},
	0
};

static menu mainmenu[] =
{
	{"&File", 0, filemenu},
	{"&Help", 0, helpmenu},
	0
};

static array menuitems;

static void drawline(HDC dc, int x1, int y1, int x2, int y2)
{
	MoveToEx(dc, x1, y1, 0);
	LineTo(dc, x2, y2);
}

static void paint()
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(wnd, &ps);

	// Outline

	HPEN oldpen = SelectObject(dc, GetStockObject(BLACK_PEN));
	drawline(dc, 0, 0, clientwidth, 0);
	drawline(dc, scrollwidth, 0, scrollwidth, clientheight);
	SelectObject(dc, oldpen);

	// Map area

	RECT rect;
	rect.left = mapx;
	rect.top = mapy;
	rect.right = mapx + mapwidth;
	rect.bottom = mapy + mapheight;
	FillRect(dc, &rect, GetStockObject(BLACK_BRUSH));

	// Hexes

	if (!hexes)
	{
		EndPaint(wnd, &ps);
		return;
	}

	static COLORREF terraincolors[] =
	{
		0xbf0000,
		0x00ffff,
		0x007f00,
		0x7f7fbf,
	};

	HBRUSH oldbrush = 0;
	HBRUSH brushes[terrainscount];
	int i;
	for (i = 0; i < terrainscount; i++)
		brushes[i] = CreateSolidBrush(terraincolors[i]);
	int b = -1;

	SelectObject(dc, GetStockObject(BLACK_PEN));

	for (int y = 0; y < mapsize; y++)
		for (int x = 0; x < mapsize; x++)
		{
			int h = xytoh(x, y);
			int t = hexes[h].terrain;
			if (t < 0)
				continue;
			if (b != t)
			{
				HBRUSH brush = SelectObject(dc, brushes[t]);
				if (!oldbrush)
					oldbrush = brush;
				b = t;
			}

			POINT p[6];
			int dy = (x % 2) * (hexheight / 2);
			for (i = 0; i < 6; i++)
			{
				p[i].x = points[i].x + x * hexwidth;
				p[i].y = points[i].y + y * hexheight + dy;
			}
			Polygon(dc, p, 6);
		}

	SelectObject(dc, oldpen);

	if (oldbrush)
		SelectObject(dc, oldbrush);
	for (i = 0; i < terrainscount; i++)
		DeleteObject(brushes[i]);

	EndPaint(wnd, &ps);
}

static LRESULT CALLBACK wndproc(HWND wnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_COMMAND:
		int id;
		id = LOWORD(wparam);
		menu *m;
		m = (menu *)menuitems[id];
		if (m->onclick)
			m->onclick();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		paint();
		return 0;

	case WM_SIZE:
		clientwidth = LOWORD(lparam);
		clientheight = HIWORD(lparam);

		textheight = clientheight - overviewheight;

		scrollwidth = clientwidth - textwidth - 1;
		scrollheight = clientheight;

		mapwidth = scrollwidth - mapx * 2;
		mapheight = scrollheight - mapy * 2;

		textx = scrollwidth;
		overviewx = textx;
		overviewy = textheight;

		redraw();
		return 0;
	}

	return DefWindowProc(wnd, message, wparam, lparam);
}

void initwindow(void *instance)
{
	// Hex dimensions and points

	const int r = 12;
	int y = (int)(sqrt(.75) * r);

	hexwidth = r * 3 / 2;
	hexheight = y * 2;

	points[0].x = r / 2;
	points[0].y = -y;

	points[1].x = r;

	points[2].x = r / 2;
	points[2].y = y;

	points[3].x = -r / 2;
	points[3].y = y;

	points[4].x = -r;

	points[5].x = -r / 2;
	points[5].y = -y;

	for (int i = 0; i < 6; i++)
	{
		points[i].x += mapx + r;
		points[i].y += mapy + y;
	}

	// Window

	static WNDCLASS wndclass;
	wndclass.lpfnWndProc = wndproc;
	wndclass.hInstance = instance;
	wndclass.hIcon = LoadIcon(instance, MAKEINTRESOURCE(10000));
	wndclass.hCursor = LoadCursor(0, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wndclass.lpszClassName = "rainbow";
	RegisterClass(&wndclass);

	void *menu = makemenu(mainmenu, menuitems);
	wnd = CreateWindow("rainbow", "Rainbow's End", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, 0, menu, instance, 0);

	int screenwidth = GetSystemMetrics(SM_CXSCREEN);
	int screenheight = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(wnd, 0, 0, screenwidth / 2, screenheight / 2, 0);

	ShowWindow(wnd, SW_SHOWMAXIMIZED);
	UpdateWindow(wnd);
}

void redraw()
{
	InvalidateRect(wnd, 0, 1);
}
