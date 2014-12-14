#include "graphics.h"
#include "standard.h"
#include "mathdef.h"

#ifndef display_h
#define display_h

struct displayhdl
{
	int scrn_width;
	int scrn_height;

	int pos, off;

	void viewport(int w, int h);

	void set(int p, int o);
	void put(char c);
	void wprint(char *str);
	void box(float r, float l, float t, float b);
	vec tooglc(vec p);
};

#endif
