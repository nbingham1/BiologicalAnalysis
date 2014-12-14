#include "renderer.h"

void renderhdl::init()
{
	fitgraph.init("Data and Fit Graph", &display, 2.0, 2.0, 2.0);
	errorgraph.init("Error Analysis Graph", &display, 1.0, 1.0, 1.0);
	fitgraph.scnpos = vec(-0.9, 0.0, 0.0);
	errorgraph.scnpos = vec(1.45, 0.0, 0.0);

	document.init();
	document.open_document("test.ldf");

	fitgraph.adddata(document.countsdata);
	fitgraph.adddata(document.luria);
	fitgraph.adddata(document.poisson);
	fitgraph.adddata(document.normal);


	errorgraph.adddata(document.luriae);
	errorgraph.adddata(document.poissone);
	errorgraph.adddata(document.luria3e);
	errorgraph.adddata(document.normale);
	errorgraph.adddata(document.normal2e);
	//errorgraph.adddata(document.binomiale);

	lnode<data*> *curr1 = fitgraph.datalist.first;
	double x = 1.0;
	while (curr1 != NULL)
	{
		boxlist.add(checkbox(curr1->data->title, vec(0.0, x++, 0.0), curr1->data->rendcol));
		curr1->data->show = &boxlist.last->data.status;
		boxlist.add(checkbox("Trace", vec(0.0, x++, 0.0), curr1->data->rendcol));
		tracer *t = new tracer;
		t->init(curr1->data, NULL, NULL, NULL, &boxlist.last->data.status);
		fitgraph.addtracer(t);

		x++;
		curr1 = curr1->next;
	}

	curr1 = errorgraph.datalist.first;
	x += 4.0;
	while (curr1 != NULL)
	{
		boxlist.add(checkbox(curr1->data->title, vec(0.0, x++, 0.0), curr1->data->rendcol));
		curr1->data->show = &boxlist.last->data.status;

		boxlist.add(checkbox("Trace", vec(0.0, x++, 0.0), curr1->data->rendcol));
		tracer *t = new tracer;
		if (curr1->data == document.luria3e)
		{
			t->init(document.luria3e, &document.mmu, NULL, &document.mml, &boxlist.last->data.status);
			boxlist.last->data.status = true;
		}
		else if (curr1->data == document.luriae)
		{
			t->init(document.luriae, NULL, NULL, &document.mml, &boxlist.last->data.status);
			boxlist.last->data.status = true;
		}
		else if (curr1->data == document.poissone)
		{
			t->init(document.poissone, &document.mmu, NULL, NULL, &boxlist.last->data.status);
			boxlist.last->data.status = true;
		}
		else if (curr1->data == document.normal2e)
		{
			t->init(document.normale, NULL, NULL, &document.mml, &boxlist.last->data.status);
			boxlist.last->data.status = true;
		}
		else
			t->init(curr1->data, NULL, NULL, NULL, &boxlist.last->data.status);
		errorgraph.addtracer(t);

		x++;

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

	fitgraph.render();
	errorgraph.render();

	lnode<checkbox> *curr = boxlist.first;
	while (curr != NULL)
	{
		curr->data.render(&display);
		curr = curr->next;
	}

	bool remade = false;
	lnode<data*> *currd = fitgraph.datalist.first;
	while (currd != NULL)
	{
		if (currd->data->remakeb)
		{
			currd->data->remakeb = false;
			if (!remade)
				document.remakedataset();
			remade = true;
		}
		currd = currd->next;
	}

	currd = errorgraph.datalist.first;
	while (currd != NULL && !remade)
	{
		if (currd->data->remakeb)
		{
			currd->data->remakeb = false;
			if (!remade)
				document.remakedataset();
			remade = true;
		}
		currd = currd->next;
	}

	glutSwapBuffers();
}
