#include "display.h"
#include "graphics.h"
#include "standard.h"
#include "mathdef.h"

#include "document.h"
#include "distribution.h"

#ifndef object_h
#define object_h

enum otype
{
	document,
	operation,
	null
};

struct object
{
	object();
	object(char *n, void *d, otype t, vec col);
	~object();

	char name[32];
	void *data;
	otype data_type;

	vec color;

	int secid;

	object &operator=(object o);

	object *makecopy();
	void init();
	void release();
};

#endif
