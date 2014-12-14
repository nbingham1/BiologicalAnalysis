#include "gui.h"

checkbox::checkbox()
{
}

checkbox::checkbox(char name[32], vec pos, vec col)
{
	strcpy(title, name);
	position = pos;
	color = col;
	status = false;
}

checkbox::~checkbox()
{
}

checkbox &checkbox::operator=(checkbox box)
{
	strcpy(title, box.title);
	position = box.position;
	color = box.color;
	status = false;
}

void checkbox::init(char name[32], vec pos, vec col)
{
	strcpy(title, name);
	position = pos;
	color = col;
	status = false;
}

bool checkbox::in_box(vec coord)
{
	if ((floor(coord.x) == position.x || floor(coord.x) == position.x+1.0) && floor(coord.y) == position.y)
	{
		status = !status;
		return true;
	}
	else
		return false;
}

void checkbox::render(displayhdl *disp)
{
	if (status)
	{
		glColor3f(0.0, 1.0, 0.0);
		disp->box(position.x+2.0, position.x, position.y + 1.0, position.y);
	}
	else
	{
		glColor3f(1.0, 0.0, 0.0);
		disp->box(position.x+2.0, position.x, position.y + 1.0, position.y);
	}

	glColor3f(color.x, color.y, color.z);
	disp->set(position.x + 3, position.y);
	disp->wprint(title);
	glColor3f(1.0, 1.0, 1.0);
}
