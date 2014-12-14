#include "object.h"

object::object()
{
	strcpy(name, "object");
	color = vec(1.0, 1.0, 1.0);
	data = NULL;
	data_type = null;
}

object::object(char *n, void *d, otype t, vec col)
{
	strcpy(name, n);
	data = d;
	data_type = t;
	color = col;
}

object::~object()
{
}

object &object::operator=(object o)
{
	strcpy(name, o.name);
	color = o.color;
	data = o.data;
	data_type = o.data_type;
	secid = o.secid;
	return *this;
}

object *object::makecopy()
{
	object *new_obj = new object;
	*new_obj = *this;
	new_obj->data = NULL;
	if (new_obj->data_type == document)
	{
		new_obj->data = new lddocument;
		memcpy(new_obj->data, this->data, sizeof(lddocument));
	}
	else if (new_obj->data_type == operation)
	{
		new_obj->data = new distribution;
		memcpy(new_obj->data, this->data, sizeof(distribution));
	}

	return new_obj;
}

void object::init()
{
	strcpy(name, "object");
	color = vec(1.0, 1.0, 1.0);
	data = NULL;
	data_type = null;
}

void object::release()
{
	if (data != NULL && data_type != null)
	{
		if (data_type == document)
			delete (lddocument*)data;
		else if (data_type == operation)
			delete (distribution*)data;
	}
	data = NULL;
	data_type = null;
}
