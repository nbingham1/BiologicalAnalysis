#include "graphics.h"
#include "standard.h"
#include "mathdef.h"

#include "document.h"

#ifndef distribution_h
#define distribution_h

enum dtype
{
	luria_d,
	normal_d,
	normal2_d,
	poisson_d,
	binomial_d,
	luria3_d
};

struct distribution
{
	distribution();
	~distribution();

	double *m_table;
	double *s_table;
	double *p_table;
	double *ps_table;
	double *pf_table;
	double *h_table;
	double *points;
	double max;
	double left;
	double right;
	double max_sigma;
	int    num_sets;
	int	   num_points;
	int	   num_sigma;

	bool   error_bars;
	bool   grid;
	bool   graph;

	dtype distribution_type;

	lddocument *active_document;

	void init();
	void clear();

	// p values
	double *luria_p(double m, int n_sets);
	double *poisson_p(double mu, int n_sets);
	double *normal_p(int n_sets);

	// luria distribution
	bool alloc_luria(int size, double mstart, double mstep);
	void release_luria();

	void pval_luria(int r);
	double *distribution_luria(double mstart, double mend, double mstep, int n_sets);
	void computepoints_luria();

	// 2D normal distribution
	bool alloc_normal(int msize, double mstart, double mstep, int ssize, double sstart, double sstep);
	void release_normal();

	void pval_normal();
	double *distribution_normal(double mstart, double mend, double mstep, double sstart, double send, double sstep, int n_sets);
	void computepoints_normal();
	double *distribution_normal2(double mstart, double mend, double mstep, double sigma, int n_sets);

	// 2D poisson distribution
	void pval_poisson();
	double *distribution_poisson(double mstart, double mend, double mstep, int n_sets);

	// 2D binomial distribution
	void pval_binomial(double prob, int total);
	double *distribution_binomial(double mstart, double mend, double mstep, double prob, int n_sets);

	// 3D luria distribution
	bool alloc_luria3(int msize, double mstart, double mstep, int ssize, double sstart, double sstep);
	void release_luria3();

	void pval_poisson3();
	void pval_luria3();
	double *distribution_luria3(double mstart, double mend, double mstep, double sstart, double send, double sstep, int n_sets);
	void computepoints_luria3();
};

#endif
