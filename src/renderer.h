#include "display.h"

#include "distribution.h"
#include "document.h"
#include "section.h"

#ifndef renderer_h
#define renderer_h

struct renderhdl
{
	displayhdl display;

	llist<object*> objectlist;
	object *sobject;
	llist<section> sectionlist;
	section *ssection;
	llist<node> nodelist;
	node *snode;

	float mouse_x;
	float mouse_y;
	int mouse_b;

	void init();
	void release();

	void displayf();

	void open_ldf(char *file);
	void add_operation(dtype t);

	section *findsection(float x, float y);
	node *findnode(float x, float y);
};

#endif
