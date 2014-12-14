#include "display.h"
#include "object.h"
#include "distribution.h"
#include "document.h"

#ifndef section_h
#define section_h

struct section
{
	section();
	section(char *title_t, int sid, float r, float l, float t, float b, bool border_t, bool wrap_t, bool lsup, float thick_t, vec bcol, vec tcol);
	~section();

	bool border;
	bool wrap;
	bool limited_supply;

	vec bcolor;
	vec tcolor;

	float thick;

	float right,
		  left,
		  top,
		  bottom;

	float hscroll;
	float vscroll;
	float tempmaxpos;
	float tempmaxoff;
	float maxpos;
	float maxoff;
	float vsmult;
	float hsmult;

	int id;

	char title[256];
	llist<object*> contents;

	float mouse_x;
	float mouse_y;

	section &operator=(section s);
	bool in_section(int pos, int off);
	void put(char c, displayhdl *disp);
	void render(displayhdl *disp);

	object *handledown(float x, float y);
	void handleup(float x, float y);
	object *handledrag(float x, float y);

	void release();
};

struct node
{
	node();
	node(object *o, vec pos);
	~node();

	object *contents;

	vec position;

	bool border;
	bool wrap;
	bool selected;
	bool open;

	float thick;
	float width;
	float height;

	float mouse_x;
	float mouse_y;

	int pos, off;

	llist<node*> outgoing;
	llist<node*> incoming;

	node &operator=(node n);

	void init();
	void release();

	void put(char c, displayhdl *disp);
	bool in_node(float x, float y);
	void handleclick(float x, float y);
	void handledrag(float x, float y);
	void render(displayhdl *disp);
	void connect(node *n);
};

#endif
