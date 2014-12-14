#include "document.h"

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

	// flags
	distribution = LURIA;
	error_bars = 0;
	current_prob = 1;
	update_status = 1;
	mmu = 0.0;
	mml = 0.0;
	luria = 1;
	normal = 0;
	poisson = 0;
	binomial = 0;
	grid = 0;
	draw_select = 0;
	update_select = 0;
	draw_poisson = 0;
	draw_normal = 0;
	mselect = 0.0f;
	mselect_max = 0.0f;
	mselect_left = 0.0f;
	mselect_right = 0.0f;
	linear = 1;
	left = 0;
	right = 0;
	current = 0;

	container.init();
	reset_container.init();
}

void lddocument::release()
{
	container.clear();
	reset_container.clear();
}

void lddocument::open_document(char *fname)
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

	container.init();
	reset_container.init();

	while ((ret = fscanf(lpfile, "%d %d", &colnum, &setheight)) != 0 && ret != EOF)
	{
		if (colnum < 151)
		{
			while (setcount < colnum)
			{
				container.add(0);
				reset_container.add(0);
				setcount++;
			}

			container.add(setheight);
			reset_container.add(setheight);
			setcount++;
			count += setheight;
		}
		else
		{
			while (setcount < 151)
			{
				container.add(0);
				reset_container.add(0);
				setcount++;
			}

			lnode<int> *curr = container.last;
			int height = curr->data + setheight;
			curr->data = height;
			setheight = height;
		}

		if (setheight > max)
			max = setheight;
	}

	for (int x = setcount; x < 151; x++)
	{
		container.add(0);
		reset_container.add(0);
	}

	if (setcount > 20)
		num_sets = setcount;
	else
		num_sets = 20;

	num_count = count;
	max_height = max;

	file_read = 1;

	fclose(lpfile);
}

void lddocument::save_document(char *fname)
{
}
