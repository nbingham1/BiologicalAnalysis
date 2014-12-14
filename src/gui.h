#include "standard.h"
#include "mathdef.h"
#include "graphics.h"
#include "display.h"

#ifndef gui_h
#define gui_h

struct checkbox
{
	checkbox();
	checkbox(char name[32], vec pos, vec col);
	~checkbox();

	char title[32];
	vec position;
	vec color;

	bool status;

	checkbox &operator=(checkbox box);

	void init(char name[32], vec pos, vec col);
	bool in_box(vec coord);
	void render(displayhdl *disp);
};

#endif
