#include "graphics.h"
#include "standard.h"
#include "mathdef.h"
#include "graph.h"

#ifndef document_h
#define document_h

#define	LURIA					0
#define	LURIAPOISSON			1
#define	POISSON					2
#define	NORMAL					3

struct lddocument
{
	lddocument();
	~lddocument();

	double range_start;
	double range_end;
	double step_size;
	double srange_start;
	double srange_end;
	double sstep_size;
	double sigma;
	double binomial_prob;
	double mselect;
	double mselect_max;
	double mselect_left;
	double mselect_right;

	double mmu;
	double mml;

	char filename[64];
	char contents[1024];
	int file_read;
	int num_sets;
	int num_count;
	int max_height;

	double counts[151];
	double rcounts[151];

	data *countsdata;
	data *luria;
	data *poisson;
	data *normal;
	data *luriae;
	data *normale;
	data *normal2e;
	data *poissone;
	data *binomiale;
	data *luria3e;

	void init();
	void release();

	void remakedataset();
	void open_document(const char *fname);
	void save_document(const char *fname);
};

#endif
