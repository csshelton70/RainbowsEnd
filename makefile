# Makefile for Microsoft C++ 4.2 with GNU Make

rainbow.exe:\
	ai.obj\
	array.obj\
	entity.obj\
	game.obj\
	help.obj\
	main.obj\
	map.obj\
	menu.obj\
	misc.obj\
	options.obj\
	order.obj\
	parse.obj\
	player.obj\
	random.obj\
	readrpt.obj\
	rules.obj\
	save.obj\
	unit.obj\
	window.obj\
	writerpt.obj\
	rainbow.res
	link /release /out:rainbow.exe *.obj rainbow.res kernel32.lib user32.lib gdi32.lib shell32.lib

%.obj: %.cc
	cl /c /W3 /WX /O2 /Ob2 /G5 /Gr /Tp$*.cc

rainbow.res: rainbow.rc *.ico
	rc rainbow
