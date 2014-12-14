#include "renderer.h"

void renderhdl::init()
{
	fitgraph.init("Data and Fit Graph", 0.0, 0.0, 0.0);
	errorgraph.init("Error Analysis Graph", 0.0, 2.0, 0.0);
	fitgraph.scnpos = vec(-0.9, 0.0, 0.0);
	errorgraph.scnpos = vec(1.45, 0.0, 0.0);

	document.init();
	document.open_document("test.ldf");

	const int arrsize = document.container.num_nodes;
	float datasize = 9.0;
	double fitdata[arrsize];

	lnode<int> *curr = document.container.first;
	for (int x = 0; x < arrsize && curr != NULL; x++)
	{
		fitdata[x] = double(curr->data);
		curr = curr->next;
	}

	data *d = new data;
	d->init("Experimental Data", histograph_t, fitdata, 0.0, datasize, 1.0, 0.0, 0.0, 1.0);
	d->rendcol = vec(1.0, 1.0, 0.0);

	fitgraph.adddata(d);

	distribution distrib;
	distrib.init();
	distrib.active_document = &document;

	data *d2 = new data;

	double *ptr = distrib.luria_p(0.0, datasize);
	for (int x = 0; x < (int)datasize; x++)
		ptr[x] *= 10.0;

	d2->init("Luria Fit Curve", wireframe_t, ptr, 0.0, datasize, 1.0, 0.0, 0.0, 1.0);
	d2->interactive = true;
	d2->rendcol = vec(1.0, 0.0, 0.0);
	fitgraph.adddata(d2);
	distrib.clear();

	distrib.init();
	distrib.active_document = &document;

	data *d3 = new data;

	ptr = distrib.poisson_p(datasize);
	for (int x = 0; x < (int)datasize; x++)
		ptr[x] *= 10.0;

	d3->init("Poisson Fit Curve", wireframe_t, ptr, 0.0, datasize, 1.0, 0.0, 0.0, 1.0);
	d3->rendcol = vec(0.0, 0.0, 1.0);
	fitgraph.adddata(d3);
	distrib.clear();

	distrib.init();
	distrib.active_document = &document;

	data *d4 = new data;

	ptr = distrib.normal_p(datasize);
	for (int x = 0; x < (int)datasize; x++)
		ptr[x] *= 10.0;

	d4->init("Normal Fit Curve", wireframe_t, ptr, 0.0, datasize, 1.0, 0.0, 0.0, 1.0);
	d4->rendcol = vec(0.0, 1.0, 0.0);
	fitgraph.adddata(d4);
	distrib.clear();

	distrib.init();
	distrib.active_document = &document;

	data *d6 = new data;
	ptr = distrib.distribution_luria(0.0, datasize + 0.1, 0.1, (int)datasize);

	d6->init("Luria Error", surfaceplot_t, ptr, 0.0, datasize, 0.1, 0.0, 0.0, 0.1);
	d6->rendcol = vec(1.0, 0.0, 0.0);
	errorgraph.adddata(d6);
	distrib.clear();

	distrib.init();
	distrib.active_document = &document;

	data *d7 = new data;
	ptr = distrib.distribution_poisson(0.0, datasize + 0.1, 0.1, (int)datasize);

	d7->init("Poisson Error", surfaceplot_t, ptr, 0.0, 0.0, 0.1, 0.0, datasize, 0.1);
	d7->rendcol = vec(0.0, 0.0, 1.0);
	errorgraph.adddata(d7);
	distrib.clear();

	distrib.init();
	distrib.active_document = &document;

	data *d5 = new data;
	ptr = distrib.distribution_luria3(0.0, datasize + 0.1, 0.1, 0.0, datasize + 0.1, 0.1, (int)datasize);

	d5->init("Luria-Poisson Error", surfaceplot_t, ptr, 0.0, datasize, 0.1, 0.0, datasize, 0.1);
	d5->rendcol = vec(1.0, 0.0, 1.0);
	errorgraph.adddata(d5);
	distrib.clear();

	lnode<data*> *curr1 = fitgraph.datalist.first;
	double x = 1.0;
	while (curr1 != NULL)
	{
		boxlist.add(checkbox(curr1->data->title, vec(0.0, x++, 0.0), curr1->data->rendcol));
		curr1->data->show = &boxlist.last->data.status;
		curr1 = curr1->next;
	}

	curr1 = errorgraph.datalist.first;
	lnode<checkbox> *currc = boxlist.first->next;
	while (curr1 != NULL)
	{
		curr1->data->show = &currc->data.status;
		if (currc->next == NULL)
			curr1->data->show = &luriapoisson;
		currc = currc->next;
		curr1 = curr1->next;
	}
}

void renderhdl::release()
{
	fitgraph.release();
	errorgraph.release();
}

void renderhdl::displayf()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glScalef(1.0, 1.0, 1.0);
	glTranslatef(0.0, 0.0, -4.0);

	fitgraph.render(&display);
	errorgraph.render(&display);

	luriapoisson = boxlist.first->next->data.status && boxlist.first->next->next->data.status;

	lnode<checkbox> *curr = boxlist.first;
	while (curr != NULL)
	{
		curr->data.render(&display);
		curr = curr->next;
	}

	glutSwapBuffers();
}
