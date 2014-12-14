#include "renderer.h"

void renderhdl::init()
{
	sobject = NULL;
	ssection = NULL;
	snode = NULL;
	objectlist.init();
	sectionlist.init();
	nodelist.init();

	sectionlist.add(section((char*)"Current Documents", 0, 24, -1, 20, 0, true, false, true, 1.0, vec(0.0, 0.0, 1.0), vec(1.0, 1.0, 1.0)));
	sectionlist.add(section((char*)"Available Operations", 1, 24, -1, 42, 21, true, false, false, 1.0, vec(0.0, 0.0, 1.0), vec(1.0, 1.0, 1.0)));

	open_ldf((char*)"test.ldf");
	open_ldf((char*)"3-23 data.ldf");
	open_ldf((char*)"6-15 data.ldf");
	open_ldf((char*)"8-09 data.ldf");

	add_operation(luria_d);
	add_operation(normal_d);
	add_operation(normal2_d);
	add_operation(poisson_d);
	add_operation(binomial_d);
	add_operation(luria3_d);
}

void renderhdl::release()
{
	lnode<object*> *curro = objectlist.first;
	while (curro != NULL)
	{
		curro->data->release();
		delete curro->data;
		curro->data = NULL;
		curro = curro->next;
	}

	objectlist.clear();

	lnode<section> *currs = sectionlist.first;
	while (currs != NULL)
	{
		currs->data.release();
		currs = currs->next;
	}

	sectionlist.clear();

	lnode<node> *currn = nodelist.first;
	while (currn != NULL)
	{
		currn->data.release();
		currn = currn->next;
	}

	nodelist.clear();
}

void renderhdl::displayf()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	lnode<section> *currs = sectionlist.first;
	while (currs != NULL)
	{
		currs->data.render(&display);
		currs = currs->next;
	}

	lnode<node> *currn = nodelist.first;
	while (currn != NULL)
	{
		currn->data.render(&display);
		currn = currn->next;
	}

	if (sobject != NULL)
	{
		float a, b;
		a = (2.0*double(mouse_x*9)/double(display.scrn_width) - 1.0);
		b = -(2.0*double((mouse_y-1)*15 + 15)/double(display.scrn_height) - 1.0);

		glColor3f(sobject->color.x, sobject->color.y, sobject->color.z);
		glRasterPos2f(a - 2.0*double(strlen(sobject->name)*9/2)/double(display.scrn_width), b);
		for (unsigned int x = 0; x < strlen(sobject->name); x++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, sobject->name[x]);
	}

	glutSwapBuffers();
}

void renderhdl::open_ldf(char *file)
{
	lddocument *new_doc = new lddocument;
	new_doc->init();
	new_doc->open_document(file);

	object *new_obj = new object;
	*new_obj = object(new_doc->filename, new_doc, document, vec(0.0, 1.0, 0.0));
	new_obj->secid = sectionlist.first->data.id;

	objectlist.add(new_obj);
	sectionlist.first->data.contents.add(new_obj);
}

void renderhdl::add_operation(dtype t)
{
	distribution *new_dist = new distribution;
	new_dist->init();
	new_dist->distribution = t;

	char opname[32];
	if (t == luria_d)
		strcpy(opname, "Luria Distribution");
	else if (t == normal_d)
		strcpy(opname, "Normal Distribution");
	else if (t == normal2_d)
		strcpy(opname, "2D Normal Distribution");
	else if (t == poisson_d)
		strcpy(opname, "Poisson Distribution");
	else if (t == binomial_d)
		strcpy(opname, "Binomial Distribution");
	else if (t == luria3_d)
		strcpy(opname, "3D Luria Distribution");

	object *new_obj = new object;
	*new_obj = object(opname, new_dist, operation, vec(1.0, 1.0, 0.0));
	new_obj->secid = sectionlist.last->data.id;

	objectlist.add(new_obj);
	sectionlist.last->data.contents.add(new_obj);
}

section *renderhdl::findsection(float x, float y)
{
	lnode<section> *curr = sectionlist.first;
	while (curr != NULL)
	{
		if (curr->data.in_section(x, y))
			return &curr->data;

		curr = curr->next;
	}

	return NULL;
}

node *renderhdl::findnode(float x, float y)
{
	lnode<node> *curr = nodelist.first;
	while (curr != NULL)
	{
		if (curr->data.in_node(x, y))
			return &curr->data;

		curr = curr->next;
	}

	return NULL;
}
