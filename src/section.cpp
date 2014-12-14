#include "section.h"

section::section()
{
	border = false;
	wrap = false;
	limited_supply = false;
	tcolor = vec(1.0, 1.0, 1.0);
	thick = 0.0;
	right = 0.0;
	left = 1.0;
	top = 1.0;
	bottom = 0.0;
	hscroll = 0;
	vscroll = 0;
	strcpy(title, "section");
	contents.init();
	maxpos = 0.0;
	maxoff = 0.0;
	id = 0;
}

section::section(char *title_t, int sid, float r, float l, float t, float b, bool border_t, bool wrap_t, bool lsup, float thick_t, vec bcol, vec tcol)
{
	id = sid;
	border = border_t;
	wrap = wrap_t;
	limited_supply = lsup;
	bcolor = bcol;
	tcolor = tcol;
	thick = thick_t;
	right = r;
	left = l;
	top = t;
	bottom = b;
	hscroll = 0;
	vscroll = 0;
	maxpos = 0.0;
	maxoff = 0.0;
	strcpy(title, title_t);
	contents.init();
}

section::~section()
{
}

section &section::operator=(section s)
{
	id = s.id;
	border = s.border;
	wrap = s.wrap;
	limited_supply = s.limited_supply;
	bcolor = s.bcolor;
	tcolor = s.tcolor;
	thick = s.thick;
	right = s.right;
	left = s.left;
	top = s.top;
	bottom = s.bottom;
	hscroll = s.hscroll;
	vscroll = s.vscroll;
	strcpy(title, s.title);
	return *this;
}

bool section::in_section(int pos, int off)
{
	if (float(pos) > left-thick && float(pos) < right+thick && float(off) > bottom-thick && float(off) < top+thick)
		return true;
	else
		return false;
}

void section::put(char c, displayhdl *disp)
{
	if (wrap)
	{
		hscroll = 0;
		if (disp->pos >= right-ceil(thick))
		{
			disp->set(left+ceil(thick) - floor(hscroll*hsmult), disp->off+1);
			tempmaxoff++;
		}
	}

	switch (c)
	{
		case '\n':
			disp->set(left+ceil(thick) - floor(hscroll*hsmult), disp->off+1);
			tempmaxoff++;
			if (tempmaxpos > maxpos)
				maxpos = tempmaxpos;
			tempmaxpos = 0.0;
			break;
		default:
			if (disp->off < top-ceil(thick) && disp->pos < right-ceil(thick) && disp->off >= bottom + 2 && disp->pos >= left+ceil(thick))
				disp->put(c);
			else if (disp->off < top-ceil(thick) && disp->pos < right-ceil(thick))
				disp->set(disp->pos+1, disp->off);

			tempmaxpos++;
			break;
	}
}

void section::render(displayhdl *disp)
{
	tempmaxoff = 0.0;
	tempmaxpos = 0.0;
	maxpos = 0.0;

	glColor3f(bcolor.x, bcolor.y, bcolor.z);
	if (border)
	{
		disp->box(right+thick, left-thick, bottom+1.5, bottom-thick/2);
		disp->box(right+thick, left-thick, top+thick/2, top-thick/2);
		disp->box(right+thick, right-thick, top+thick/2, bottom-thick/2);
		disp->box(left+thick, left-thick, top+thick/2, bottom-thick/2);
	}
	glColor3f(tcolor.x, tcolor.y, tcolor.z);
	disp->set((right - left)/2 + left - strlen(title)/2, bottom);
	disp->wprint(title);

	disp->set(left+ceil(thick) - floor(hscroll*hsmult), bottom+2 - floor(vscroll*vsmult));
	lnode<object*> *curr = contents.first;
	while (curr != NULL)
	{
		glColor3f(curr->data->color.x, curr->data->color.y, curr->data->color.z);
		disp->set(disp->pos, disp->off);
		for (unsigned int x = 0; x < strlen(curr->data->name); x++)
			put(curr->data->name[x], disp);
		put('\n', disp);
		curr = curr->next;
	}

	glColor3f(1.0 - bcolor.x, 1.0 - bcolor.y, 1.0 - bcolor.z);
	disp->box(right+thick, right-thick, bottom+vscroll+5, bottom+vscroll);
	disp->box(left+hscroll+10, left+hscroll, top+thick/2, top-thick/2);

	maxoff = tempmaxoff;
	vsmult = maxoff/(top-bottom-2-ceil(thick));
	if (vsmult < 1.0)
		vsmult = 0.0;
	hsmult = maxpos/(right-left-ceil(thick)*2);
	if (hsmult < 1.0 || wrap)
		hsmult = 0.0;
}

object *section::handledown(float x, float y)
{
	mouse_x = x;
	mouse_y = y;

	if (mouse_x <= right - thick && mouse_x >= left + thick && mouse_y <= top - thick/2 && mouse_y >= bottom + 2.0)
	{
		int index = int(mouse_y - bottom - 2.0 - floor(vscroll*vsmult));
		object *ptr;
		contents.getindex(index, &ptr);

		return ptr;
	}

	return NULL;
}

void section::handleup(float x, float y)
{
	mouse_x = x;
	mouse_y = y;
}

object *section::handledrag(float x, float y)
{
	if (mouse_x <= right + thick && mouse_x >= right - thick && mouse_y <= bottom+vscroll+5 && mouse_y >= bottom+vscroll)
	{
		vscroll = m_clamp(mouse_y - bottom - 2.5, 0.0, top-bottom - 5.0);
		if (y - bottom - 2.5 == m_clamp(y - bottom - 2.5, 0.0, top-bottom - 5.0))
			mouse_y = y;
	}
	else if (mouse_x <= left+hscroll+10 && mouse_x >= left+hscroll && mouse_y <= top+thick/2 && mouse_y >= top-thick/2)
	{
		hscroll = m_clamp(mouse_x - left - 5.0, 0.0, right-left - 10.0);
		if (x - left - 5.0 == m_clamp(x - left - 5.0, 0.0, right-left - 10.0))
			mouse_x = x;
	}
	else
	{
		mouse_x = x;
		mouse_y = y;
	}

	if (mouse_x <= right - thick && mouse_x >= left + thick && mouse_y <= top - thick/2 && mouse_y >= bottom + 2.0)
	{
		int index = int(mouse_y - bottom - 2.0 - floor(vscroll*vsmult));
		object *ptr;
		contents.getindex(index, &ptr);

		return ptr;
	}

	return NULL;
}

void section::release()
{
	contents.clear();
}

node::node()
{
	contents = NULL;
	selected = false;
	open = false;
	border = true;
	wrap = false;
	thick = 1.0;
	pos = 0;
	off = 0;
	init();
}

node::node(object *o, vec pos)
{
	contents = o;
	if (contents->data_type == document)
	{
		int maxpos = 0, maxoff = 0;
		int temppos = 0;
		for (int x = 0; x < strlen(((lddocument*)contents->data)->contents); x++)
		{
			if (((lddocument*)contents->data)->contents[x] == '\n')
			{
				maxoff++;
				if (temppos > maxpos)
					maxpos = temppos;
				temppos = 0;
			}
			else if (((lddocument*)contents->data)->contents[x] == '\r')
			{
			}
			else if (((lddocument*)contents->data)->contents[x] == '\t')
				temppos+=5;
			else
				temppos++;
		}

		width = float(maxpos+2) + 2*thick;
		height = float(maxoff+1) + thick;
	}
	position = pos;
	selected = false;
	open = false;
	border = true;
	wrap = false;
	thick = 1.0;
	this->pos = 0;
	off = 0;
	init();
}

node::~node()
{
}

node &node::operator=(node n)
{
	contents = n.contents;
	if (contents->data_type == document)
	{
		int maxpos = 0, maxoff = 0;
		int temppos = 0;
		for (int x = 0; x < strlen(((lddocument*)contents->data)->contents); x++)
		{
			if (((lddocument*)contents->data)->contents[x] == '\n')
			{
				maxoff++;
				if (temppos > maxpos)
					maxpos = temppos;
				temppos = 0;
			}
			else if (((lddocument*)contents->data)->contents[x] == '\r')
			{
			}
			else if (((lddocument*)contents->data)->contents[x] == '\t')
				temppos+=5;
			else
				temppos++;
		}

		width = float(maxpos+2) + 2*thick;
		height = float(maxoff+1) + thick;
	}
	position = n.position;
	selected = n.selected;
	border = n.border;
	wrap = n.wrap;
	thick = n.thick;
	open = n.open;
	init();
	return *this;
}

void node::init()
{
	outgoing.init();
	incoming.init();
}

void node::release()
{
	lnode<node*> *curr = outgoing.first;
	lnode<node*> *curr2;
	while (curr != NULL)
	{
		curr2 = curr->data->incoming.first;
		while (curr2 != NULL && curr2->data != this)
			curr2 = curr2->next;

		curr->data->incoming.rem(curr2);

		curr = curr->next;
	}
	outgoing.clear();

	curr = incoming.first;
	curr2 = NULL;
	while (curr != NULL)
	{
		curr2 = curr->data->outgoing.first;
		while (curr2 != NULL && curr2->data != this)
			curr2 = curr2->next;

		curr->data->outgoing.rem(curr2);

		curr = curr->next;
	}
	incoming.clear();
}

void node::put(char c, displayhdl *disp)
{
	vec rpos;
	switch (c)
	{
		case '\n':
			off++;
			pos = 0;
			rpos = disp->tooglc(position + vec(pos - width/2 + thick*2, off - height/2 + thick*1.8));
			glRasterPos2f(rpos.x, rpos.y);
			break;
		case '\r':
			break;
		case '\t':
			for (int x = 0; x < 5; x++)
				put(' ', disp);
			break;
		default:
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
			pos++;
			break;
	}
}

bool node::in_node(float x, float y)
{
	if (!open)
	{
		if (m_sqr(float(position.x - x)*9.0/15.0) + m_sqr(float(position.y - y)) <= 1.0)
			return true;
		else
			return false;
	}
	else
	{
		if (x >= position.x - thick - width/2 && x <= position.x + thick + width/2 && y >= position.y - thick/2 - height/2 && y <= position.y + height/2 + thick/2)
			return true;
		else
			return false;
	}
}

void node::handleclick(float x, float y)
{
	mouse_x = x;
	mouse_y = y;
	selected = !selected;
}

void node::handledrag(float x, float y)
{
	position.x += x - mouse_x;
	position.y += y - mouse_y;
	mouse_x = x;
	mouse_y = y;
}

void node::render(displayhdl *disp)
{
	vec rpos = disp->tooglc(position);
	vec tpos = disp->tooglc(position - vec(float(strlen(contents->name))/2, -2.0, 0.0));
	vec tpos2 = disp->tooglc(position - vec(float(strlen(contents->name))/2, -height/2 - thick*2, 0.0));
	if (!open)
	{
		if (selected)
			glColor3f(1.0, 0.0, 0.0);
		else
			glColor3f(contents->color.x, contents->color.y, contents->color.z);
		glBegin(GL_POLYGON);
		for (float x = 0; x < 2*m_pi; x+=m_pi/6)
			glVertex2f(cos(x)*30.0/(float)disp->scrn_width + rpos.x, sin(x)*30.0/(float)disp->scrn_height + rpos.y);
		glEnd();

		if (contents != NULL)
		{
			glRasterPos2f(tpos.x, tpos.y);
			for (int x = 0; x < strlen(contents->name); x++)
				glutBitmapCharacter(GLUT_BITMAP_9_BY_15, contents->name[x]);
		}
	}
	else
	{
		if (selected)
			glColor3f(1.0, 0.0, 0.0);
		else
			glColor3f(contents->color.x, contents->color.y, contents->color.z);
		if (border)
		{
			disp->box(width/2 + thick + position.x, -width/2 - thick + position.x, -height/2 + thick/2 + position.y, -height/2 - thick/2 + position.y);
			disp->box(width/2 + thick + position.x, -width/2 - thick + position.x, height/2 + thick/2 + position.y, height/2 - thick/2 + position.y);
			disp->box(width/2 + thick + position.x, width/2 - thick + position.x, height/2 + thick/2 + position.y, -height/2 - thick/2 + position.y);
			disp->box(-width/2 + thick + position.x, -width/2 - thick + position.x, height/2 + thick/2 + position.y, -height/2 - thick/2 + position.y);
		}

		if (contents != NULL)
		{
			glRasterPos2f(tpos2.x, tpos2.y);
			for (unsigned int x = 0; x < strlen(contents->name); x++)
				glutBitmapCharacter(GLUT_BITMAP_9_BY_15, contents->name[x]);

			glColor3f(contents->color.x, contents->color.y, contents->color.z);
			if (contents->data_type == document)
			{
				off = -1;
				pos = 0;
				put('\n', disp);
				for (unsigned int x = 0; x < strlen(((lddocument*)contents->data)->contents); x++)
					put(((lddocument*)contents->data)->contents[x], disp);
			}
		}
	}

	vec rpos2;
	glBegin(GL_LINES);
	lnode<node*> *curr = outgoing.first;
	while (curr != NULL)
	{
		rpos2 = disp->tooglc(curr->data->position);
		glVertex2f(rpos.x, rpos.y);
		glVertex2f(rpos2.x, rpos2.y);
		curr = curr->next;
	}
	glEnd();
}

void node::connect(node *n)
{
	outgoing.add(n);
	n->incoming.add(this);

	distribution *distptr;
	if (n->contents->data_type == operation && contents->data_type == document)
	{
		distptr = ((distribution*)n->contents->data);
		((distribution*)n->contents->data)->active_document = ((lddocument*)contents->data);
	}
	else if (n->contents->data_type == document && contents->data_type == operation)
	{
		distptr = ((distribution*)contents->data);
		((distribution*)contents->data)->active_document = ((lddocument*)n->contents->data);
	}

	if (distptr != NULL)
	{
		double *ptr;
		switch (distptr->distribution)
		{
			case luria_d:
				//distptr->distribution_luria(mstart, mend, mstep, nsets, ptr);
				break;
			case normal_d:
				//distptr->distribution_normal(mstart, mend, mstep, sstart, send, sstep, nsets, ptr);
				break;
			case normal2_d:
				//distptr->distribution_normal2(mstart, mend, mstep, sigma, nsets, ptr);
				break;
			case poisson_d:
				//distptr->distribution_poisson(mstart, mend, mstep, nsets, ptr);
				break;
			case binomial_d:
				//distptr->distribution_binomial(mstart, mend, mstep, prob, nsets, ptr);
				break;
			case luria3_d:
				//distptr->distribution_luria3(mstart, mend, mstep, sstart, send, sstep, nsets, ptr);
				break;
		}
	}
}
