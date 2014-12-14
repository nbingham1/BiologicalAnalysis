#include "display.h"

#include "distribution.h"
#include "document.h"
#include "graph.h"

#ifndef renderer_h
#define renderer_h

struct renderhdl
{
	displayhdl display;

	lddocument document;

	graph fitgraph;
	graph errorgraph;

	bool luriapoisson;

	llist<checkbox> boxlist;

	float mouse_x;
	float mouse_y;
	int mouse_b;

	void init();
	void release();

	void displayf();
};

#endif
