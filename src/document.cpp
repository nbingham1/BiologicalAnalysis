#include "document.h"
#include "distribution.h"

lddocument::lddocument()
{
	init();
}

lddocument::~lddocument()
{
	release();
	init();
}

void lddocument::init()
{
	// calculation
	range_start = 0.0;
	range_end = 4.0;
	step_size = 0.1;
	srange_start = 0.0f;
	srange_end = 2.0f;
	sstep_size = 0.1;
	sigma = 1.0;
	binomial_prob = 0.5;

	// file
	file_read = 0;
	num_sets = 0;
	num_count = 0;
	max_height = 0;

	mmu = 0.0;
	mml = 0.0;

	for (int x = 0; x < 150; x++)
	{
		counts[x] = 0.0;
		rcounts[x] = 0.0;
	}

	countsdata = NULL;
	luria = NULL;
	poisson = NULL;
	normal = NULL;
	luriae = NULL;
	normale = NULL;
	normal2e = NULL;
	poissone = NULL;
	binomiale = NULL;
	luria3e = NULL;
}

void lddocument::release()
{
	if (countsdata != NULL)
	{
		countsdata->release();
		delete countsdata;
		countsdata = NULL;
	}

	if (luria != NULL)
	{
		luria->release();
		delete luria;
		luria = NULL;
	}

	if (poisson != NULL)
	{
		poisson->release();
		delete poisson;
		poisson = NULL;
	}

	if (normal != NULL)
	{
		normal->release();
		delete normal;
		normal = NULL;
	}

	if (luriae != NULL)
	{
		luriae->release();
		delete luriae;
		luriae = NULL;
	}

	if (normale != NULL)
	{
		normale->release();
		delete normale;
		normale = NULL;
	}

	if (normal2e != NULL)
	{
		normal2e->release();
		delete normal2e;
		normal2e = NULL;
	}

	if (poissone != NULL)
	{
		poissone->release();
		delete poissone;
		poissone = NULL;
	}

	if (binomiale != NULL)
	{
		binomiale->release();
		delete binomiale;
		binomiale = NULL;
	}

	if (luria3e != NULL)
	{
		luria3e->release();
		delete luria3e;
		luria3e = NULL;
	}
}

void lddocument::remakedataset()
{
	double *ptr;
	distribution analyze;
	analyze.init();
	analyze.active_document = this;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	countsdata->remake(counts);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ptr = analyze.luria_p(mml, num_sets);

	luria->remake(ptr);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ptr = analyze.poisson_p(mmu, num_sets);

	poisson->remake(ptr);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ptr = analyze.normal_p(num_sets);

	normal->remake(ptr);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ptr = analyze.distribution_luria(0.0, 10.125, 0.125, num_sets);

	luriae->remake(ptr);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ptr = analyze.distribution_normal(0.0, 10.25, 0.25, 0.0, 10.1, 0.25, num_sets);

	normal2e->remake(ptr);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ptr = analyze.distribution_normal2(0.0, 10.25, 0.25, mml, num_sets);

	normale->remake(ptr);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ptr = analyze.distribution_poisson(0.0, 10.125, 0.125, num_sets);

	poissone->remake(ptr);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*	ptr = analyze.distribution_binomial(0.0, 1.01, 0.01, 0.5, num_sets);

	binomiale->remake(ptr);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ptr = analyze.distribution_luria3(0.0, 10.25, 0.25, 0.0, 10.25, 0.25, num_sets);

	luria3e->remake(ptr);

	analyze.clear();
}

void lddocument::open_document(const char *fname)
{
	FILE *lpfile;
	int colnum, setheight;
	int setcount, ret, max;
	int count;

	strcpy(filename, fname);

	lpfile = fopen(filename, "r");
	if (lpfile == NULL)
	{
		perror("open_document(): unable to open file");
		return;
	}
	fread(contents, 1, fsize(lpfile), lpfile);
	contents[fsize(lpfile)] = '\0';
	fseek(lpfile, 0, SEEK_SET);

	setcount = 0;
	max = 0;
	count = 0;

	while ((ret = fscanf(lpfile, "%d %d", &colnum, &setheight)) != 0 && ret != EOF)
	{
		if (colnum < 151)
		{
			while (setcount < colnum)
			{
				counts[setcount] = 0.0;
				rcounts[setcount] = 0.0;
				setcount++;
			}

			counts[setcount] = setheight;
			rcounts[setcount] = setheight;
			setcount++;
			count += setheight;
		}
		else
		{
			while (setcount < 151)
			{
				counts[setcount] = 0.0;
				rcounts[setcount] = 0.0;
				setcount++;
			}

			int height = counts[150] + setheight;
			counts[150] = height;
			setheight = height;
		}

		if (setheight > max)
			max = setheight;
	}

	for (int x = setcount; x < 151; x++)
	{
		counts[x] = 0.0;
		rcounts[x] = 0.0;
	}

	num_sets = setcount;

	if (num_sets < 15)
		num_sets = 15;

	num_count = count;
	max_height = max;

	file_read = 1;

	fclose(lpfile);

	double *ptr;
	distribution analyze;
	analyze.init();
	analyze.active_document = this;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (countsdata != NULL)
		countsdata->release();

	countsdata = new data();

	countsdata->init((char*)"Experimental Data", histograph_t, counts, 0.0, float(num_sets), 1.0, 0.0, 0.0, 1.0);
	countsdata->rendcol = vec(1.0, 1.0, 0.0);
	countsdata->interactive = true;
	countsdata->integer = true;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (luria != NULL)
		luria->release();

	luria = new data();

	ptr = analyze.luria_p(mml, num_sets);

	luria->init((char*)"Luria Fit", surfaceplot_t, ptr, 0.0, float(num_sets), 1.0, 0.0, 0.0, 1.0);
	luria->rendcol = vec(1.0, 0.0, 0.0);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (poisson != NULL)
		poisson->release();

	poisson = new data();

	ptr = analyze.poisson_p(mmu, num_sets);

	poisson->init((char*)"Poisson Fit", surfaceplot_t, ptr, 0.0, float(num_sets), 1.0, 0.0, 0.0, 1.0);
	poisson->rendcol = vec(0.0, 1.0, 0.0);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (normal != NULL)
		normal->release();

	normal = new data();

	ptr = analyze.normal_p(num_sets);

	normal->init((char*)"Normal Fit", surfaceplot_t, ptr, 0.0, float(num_sets), 1.0, 0.0, 0.0, 1.0);
	normal->rendcol = vec(0.0, 1.0, 1.0);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (luriae != NULL)
		luriae->release();

	luriae = new data();

	ptr = analyze.distribution_luria(0.0, 10.125, 0.125, num_sets);

	luriae->init((char*)"Luria Error", surfaceplot_t, ptr, 0.0, 0.0, 0.1, 0.0, 10.0, 0.125);
	luriae->rendcol = vec(1.0, 0.0, 0.0);
	luriae->snaptomax = true;

	analyze.clear();
	analyze.init();
	analyze.active_document = this;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (normal2e != NULL)
		normal2e->release();

	normal2e = new data();

	ptr = analyze.distribution_normal(0.0, 10.25, 0.25, 0.0, 10.25, 0.25, num_sets);


	normal2e->init((char*)"Luria-Normal Error", surfaceplot_t, ptr, 0.0, 10.0, 0.25, 0.0, 10.0, 0.25);
	normal2e->rendcol = vec(1.0, 0.0, 1.0);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (normale != NULL)
		normale->release();

	normale = new data();

	ptr = analyze.distribution_normal2(0.0, 10.25, 0.25, mml, num_sets);

	normale->init((char*)"Normal Error", surfaceplot_t, ptr, 0.0, 10.0, 0.25, 0.0, 0.0, 0.1);
	normale->rendcol = vec(0.0, 0.0, 1.0);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (poissone != NULL)
		poissone->release();

	poissone = new data();

	ptr = analyze.distribution_poisson(0.0, 10.125, 0.125, num_sets);


	poissone->init((char*)"Poisson Error", surfaceplot_t, ptr, 0.0, 10.0, 0.125, 0.0, 0.0, 0.1);
	poissone->rendcol = vec(0.0, 1.0, 0.0);
	poissone->snaptomax = true;

	analyze.clear();
	analyze.init();
	analyze.active_document = this;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*	if (binomiale != NULL)
		binomiale->release();

	binomiale = new data();

	ptr = analyze.distribution_binomial(0.0, 1.01, 0.01, 0.5, num_sets);


	binomiale->init((char*)"Binomial Error", surfaceplot_t, ptr, 0.0, 1.0, 0.01, 0.0, 0.0, 0.01);
	binomiale->rendcol = vec(1.0, 0.0, 1.0);

	analyze.clear();
	analyze.init();
	analyze.active_document = this;*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (luria3e != NULL)
		luria3e->release();

	luria3e = new data();

	ptr = analyze.distribution_luria3(0.0, 10.25, 0.25, 0.0, 10.25, 0.25, num_sets);


	luria3e->init((char*)"Luria-Poisson Error", surfaceplot_t, ptr, 0.0, 10.0, 0.25, 0.0, 10.0, 0.25);
	luria3e->rendcol = vec(1.0, 1.0, 0.0);
	luria3e->snaptomax = true;

	analyze.clear();
}

void lddocument::save_document(const char *fname)
{
}
