#include "distribution.h"

void distribution::init()
{
	m_table = NULL;
	s_table = NULL;
	p_table = NULL;
	ps_table = NULL;
	pf_table = NULL;
	h_table = NULL;
	points = NULL;

	max = 0.0;
	left = 0.0;
	right = 0.0;
	max_sigma = 0.0;
	num_sets = 0;
	num_points = 0;
	num_sigma = 0;

	active_document = NULL;
}

void distribution::clear()
{
	if (m_table != NULL)
		delete [] m_table;

	if (s_table != NULL)
		delete [] s_table;

	if (p_table != NULL)
		delete [] p_table;

	if (ps_table != NULL)
		delete [] ps_table;

	if (pf_table != NULL)
		delete [] pf_table;

	if (h_table != NULL)
		delete [] h_table;

	if (points != NULL)
		delete [] points;

	m_table = NULL;
	s_table = NULL;
	p_table = NULL;
	ps_table = NULL;
	pf_table = NULL;
	h_table = NULL;
	points = NULL;

	max = 0.0;
	left = 0.0;
	right = 0.0;
	max_sigma = 0.0;
	num_sets = 0;
	num_points = 0;
	num_sigma = 0;

	active_document = NULL;
}


// p values
double *distribution::luria_p(double m, int n_sets)
{
	double sum, max;

	p_table = new double[n_sets];
	if (p_table == NULL)
	{
		perror("luria_p(): alloc of p_table failed.");
		return NULL;
	}

	p_table[0] = exp(-m);
	max = p_table[0];

	for (int x = 1; x < n_sets; x++)
	{
		sum = 0.0;

		for (int y = 0; y < x; y++)
			sum += p_table[y]/double(x-y+1);

		p_table[x] = sum/double(x);

		if (p_table[x] > max)
			max = p_table[x];
	}

	return p_table;
}

double *distribution::poisson_p(double mu, int n_sets)
{
	if (active_document == NULL)
	{
		perror("normal_p(): active document is null.");
		return NULL;
	}

	//double mu;

	p_table = new double[n_sets];
	if (p_table == NULL)
	{
		perror("poisson_p(): alloc of p_table failed.");
		return NULL;
	}


	//mu = double(active_document->num_count)/double(n_sets);
	for (int x = 0; x < n_sets; x++)
		p_table[x] = exp(-mu)*pow(mu, x)/m_fac(x);


	return p_table;
}

double *distribution::normal_p(int n_sets)
{
	if (active_document == NULL)
	{
		perror("normal_p(): active document is null.");
		return NULL;
	}

	int curr;
	double mu, sig;

	p_table = new double[n_sets];
	if (p_table == NULL)
	{
		perror("normal_p(): alloc of p_table failed.");
		return NULL;
	}


	mu = double(active_document->num_count)/double(n_sets);

	sig = 0.0;
	for (int x = 0; x < n_sets; x++)
	{
		curr = active_document->counts[x];
		sig += m_sqr(mu-curr);
	}
	sig /= n_sets-1;

	for (int x= 0; x < n_sets; x++)
		p_table[x] = exp(-m_sqr(double(double(x)-mu))/(2.0*sig))/(sqrt(2.0*sig)*m_pi);

	return p_table;
}

// luria distribution
bool distribution::alloc_luria(int size, double mstart, double mstep)
{
	m_table = new double[size];
	if (m_table == NULL)
	{
		perror("alloc_table(): alloc of m_table failed.");
		return false;
	}

	h_table = new double[size];
	if (h_table == NULL)
	{
		perror("alloc_table(): alloc of h_table failed.");
		return false;
	}

	points = new double[size];
	if (points == NULL)
	{
		perror("alloc_table(): alloc of points failed.");
		return false;
	}

	p_table = new double[num_sets*size];
	if (p_table == NULL)
	{
		perror("alloc_table(): alloc of points failed.");
		return false;
	}

	for (int x = 0; x < size; x++)
	{
		m_table[x] = mstart + double(x)*mstep;
	}

	return true;
}

void distribution::release_luria()
{
	if (m_table != NULL)
		delete [] m_table;

	if (h_table != NULL)
		delete [] h_table;

	if (p_table != NULL)
		delete [] p_table;

	m_table = NULL;
	h_table = NULL;
	p_table = NULL;
}

void distribution::pval_luria(int r)
{
	if (r == 0)
		for (int x = 0; x < num_points; x++)
			p_table[x] = exp(-m_table[x]);
	else
	{
		for (int x = 0; x < num_points; x++)
			h_table[x] = 0.0;

		for (int x = 0; x < r; x++)
			for (int y = 0; y < num_points; y++)
				h_table[y] += p_table[x*num_points + y]/double(r-x+1);

		for (int x = 0; x < num_points; x++)
			p_table[r*num_points + x] = h_table[x]*m_table[x]/double(r);
	}
}

double *distribution::distribution_luria(double mstart, double mend, double mstep, int n_sets)
{
	int size;

	size = (int)ceil((mend - mstart)/mstep);
	num_points = size;
	num_sets = n_sets;

	if (!alloc_luria(num_points, mstart, mstep))
	{
		return NULL;
	}

	for (int x = 0; x < num_sets; x++)
		pval_luria(x);

	computepoints_luria();
	release_luria();

	return points;
}

void distribution::computepoints_luria()
{
	if (active_document == NULL)
	{
		perror("computepoints_normal(): active document is null.");
		return;
	}

	double sum;
	int zero, maxi;
	double maxd, delta;

	maxd = -10000;
	maxi = 0;

	for (int x = 0; x < num_points; x++)
	{
		sum = 0.0;
		zero = 0;

		for (int y = 0; y < num_sets; y++)
		{
			if (p_table[y*num_points + x] != 0.0)
				sum += log(p_table[y*num_points + x])*active_document->counts[y];
			else
				zero = 1;
		}

		if (!zero)
			points[x] = sum;
		else
			points[x] = 1.0;

		if (points[x] < 1.0 && points[x] > maxd)
		{
			maxd = points[x];
			max = m_table[x];
			maxi = x;
		}
	}
	if (maxd == -10000)
		return;

	for (int x = 0; x < num_points; x++)
	{
		if (points[x] < 1.0)
		{
			points[x] -= maxd;
			points[x] = exp(points[x]);
		}
		else
			points[x] = 0.0;
	}

	delta = 0.1;
	left = m_table[0];
	right = m_table[num_points-1];
	for (int x = 0; x < num_points; x++)
	{
		if (points[x] < 0.5+delta && points[x] > 0.5-delta)
		{
			if (maxi > x)
				left = m_table[x];
			else
				right = m_table[x];
		}
	}
}

// 2D normal distribution
bool distribution::alloc_normal(int msize, double mstart, double mstep, int ssize, double sstart, double sstep)
{
	m_table = new double[msize];
	if (m_table == NULL)
	{
		perror("alloc_table(): alloc of m_table failed.");
		return false;
	}

	s_table = new double[ssize];
	if (s_table == NULL)
	{
		perror("alloc_table(): alloc of s_table failed.");
		return false;
	}

	points = new double[msize*ssize];
	if (points == NULL)
	{
		perror("alloc_table(): alloc of points failed.");
		return false;
	}

	p_table = new double[num_sets*msize*ssize];
	if (p_table == NULL)
	{
		perror("alloc_table(): alloc of points failed.");
		return false;
	}

	for (int x = 0; x < msize; x++)
		m_table[x] = mstart + x*mstep;

	for (int x = 0; x < ssize; x++)
		s_table[x] = sstart + x*sstep;

	return true;
}

void distribution::release_normal()
{
	if (m_table != NULL)
		delete [] m_table;

	if (s_table != NULL)
		delete [] s_table;

	if (p_table != NULL)
		delete [] p_table;

	m_table = NULL;
	s_table = NULL;
	p_table = NULL;
}

void distribution::pval_normal()
{
	for (int x = 0; x < num_sets; x++)
		for (int y = 0; y < num_sigma; y++)
			for (int z = 0; z < num_points; z++)
				p_table[x*num_sigma*num_points + y*num_points + z] = exp(-m_sqr(m_table[z]-x)/(2*m_sqr(s_table[y])))/(sqrt(2)*m_pi*s_table[y]);
}

double *distribution::distribution_normal(double mstart, double mend, double mstep, double sstart, double send, double sstep, int n_sets)
{
	if (sstart < 0 || mstep == 0 || sstep == 0)
	{
		return NULL;
	}

	num_points = (int)ceil((mend - mstart)/mstep);
	num_sigma = (int)ceil((send - sstart)/sstep);

	num_sets = n_sets;

	if (!alloc_normal(num_points, mstart, mstep, num_sigma, sstart, sstep))
	{
		return NULL;
	}

	pval_normal();
	computepoints_normal();
	return points;
}

void distribution::computepoints_normal()
{
	if (active_document == NULL)
	{
		perror("computepoints_normal(): active document is null.");
		return;
	}

	double sum;
	int zero;
	double maxd;

	maxd = -10000;

	for (int x = 0; x < num_sigma; x++)
	{
		for (int y = 0; y < num_points; y++)
		{
			sum = 0.0;
			zero = 0;

			for (int z = 0; z < num_sets; z++)
			{
				if (p_table[z*num_sigma*num_points + x*num_points + y] != 0)
					sum += log(p_table[z*num_sigma*num_points + x*num_points + y])*active_document->counts[z];
				else
					zero = 1;
			}

			if (!zero)
				points[x*num_points + y] = sum;
			else
				points[x*num_points + y] = 1.0;

			if (points[x*num_points + y] < 1.0 && points[x*num_points + y] > maxd)
			{
				maxd = points[x*num_points + y];
				max = m_table[y];
				max_sigma = s_table[x];
			}
		}
	}

	if (maxd == -10000)
		return;

	for (int x = 0; x < num_sigma; x++)
	{
		for (int y = 0; y < num_points; y++)
		{
			if (points[x*num_points + y] < 1.0)
			{
				points[x*num_points + y] -= maxd;
				points[x*num_points + y] = exp(points[x*num_points + y]);
			}
			else
				points[x*num_points + y] = 0.0;
		}
	}
}

double *distribution::distribution_normal2(double mstart, double mend, double mstep, double sigma, int n_sets)
{
	num_points = (int)ceil((mend - mstart)/mstep);
	num_sigma = 1;

	num_sets = n_sets;

	if (!alloc_normal(num_points, mstart, mstep, num_sigma, 1.0, 1.0))
	{
		return NULL;
	}

	pval_normal();
	computepoints_normal();
	release_normal();

	return points;
}

// 2D poisson distribution
void distribution::pval_poisson()
{
	for (int x = 0; x < num_sets; x++)
		for (int y = 0; y < num_points; y++)
			p_table[x*num_points + y] = exp(-m_table[y])*pow(m_table[y], x)/double(m_fac(x));
}

double *distribution::distribution_poisson(double mstart, double mend, double mstep, int n_sets)
{
	int size;

	size = (int)ceil((mend - mstart)/mstep);
	num_points = size;
	num_sets = n_sets;

	if (!alloc_luria(num_points, mstart, mstep))
	{
		return NULL;
	}

	pval_poisson();
	computepoints_luria();
	release_luria();

	return points;
}

// 2D binomial distribution
void distribution::pval_binomial(double prob, int total)
{
	for (int x = 0; x < num_sets; x++)
		for (int y = 0; y < num_points; x++)
			p_table[x*num_points + y] = 1.0;
}

double *distribution::distribution_binomial(double mstart, double mend, double mstep, double prob, int n_sets)
{
	int size;

	size = (int)ceil((mend - mstart)/mstep);
	num_points = size;
	num_sets = n_sets;

	if (!alloc_luria(num_points, mstart, mstep))
	{
		return NULL;
	}

	pval_binomial(prob, n_sets);
	computepoints_luria();
	release_luria();

	return points;
}

// 3D luria distribution
bool distribution::alloc_luria3(int msize, double mstart, double mstep, int ssize, double sstart, double sstep)
{
	m_table = new double[msize];
	if (m_table == NULL)
	{
		perror("alloc_table(): alloc of m_table failed.");
		return false;
	}

	s_table = new double[ssize];
	if (s_table == NULL)
	{
		perror("alloc_table(): alloc of s_table failed.");
		return false;
	}

	points = new double[msize*ssize];
	if (points == NULL)
	{
		perror("alloc_table(): alloc of points failed.");
		return false;
	}

	p_table = new double[num_sets*msize];
	if (p_table == NULL)
	{
		perror("alloc_table(): alloc of p_table failed.");
		return false;
	}

	ps_table = new double[num_sets*ssize];
	if (ps_table == NULL)
	{
		perror("alloc_table(): alloc of ps_table failed.");
		return false;
	}

	pf_table = new double[num_sets*msize*ssize];
	if (pf_table == NULL)
	{
		perror("alloc_table(): alloc of pf_table failed.");
		return false;
	}

	h_table = new double[msize];
	if (h_table == NULL)
	{
		perror("alloc_table(): alloc of h_table failed.");
		return false;
	}

	for (int x = 0; x < msize; x++)
		m_table[x] = mstart + x*mstep;

	for (int x = 0; x < ssize; x++)
		s_table[x] = sstart + x*sstep;

	return true;
}

void distribution::release_luria3()
{
	if (m_table != NULL)
		delete [] m_table;

	if (s_table != NULL)
		delete [] s_table;

	if (p_table != NULL)
		delete [] p_table;

	if (ps_table != NULL)
		delete [] ps_table;

	if (pf_table != NULL)
		delete [] pf_table;

	if (h_table != NULL)
		delete [] h_table;


	m_table = NULL;
	s_table = NULL;
	p_table = NULL;
	ps_table = NULL;
	pf_table = NULL;
	h_table = NULL;
}

void distribution::pval_poisson3()
{
	for (int x = 0; x < num_sets; x++)
		for (int y = 0; y < num_sigma; y++)
			ps_table[x*num_sigma + y] = exp(-s_table[y])*pow(s_table[y], x)/m_fac(x);
}

void distribution::pval_luria3()
{
	double sum;

	for (int x = 0; x < num_sets; x++)
		pval_luria(x);

	pval_poisson3();

	for (int x = 0; x < num_sets; x++)
		for (int y = 0; y < num_sigma; y++)
			for (int z = 0; z < num_points; z++)
			{
				if (x == 0)
					pf_table[x*num_sigma*num_points + y*num_points + z] = p_table[z]*ps_table[y];
				else
				{
					sum = 0.0;
					for (int w = 0; w < x; w++)
						sum += p_table[w*num_points + z]*ps_table[(x-1-w)*num_sigma + y];
					pf_table[x*num_sigma*num_points + y*num_points + z] = sum;
				}
			}
}

double *distribution::distribution_luria3(double mstart, double mend, double mstep, double sstart, double send, double sstep, int n_sets)
{
	if (sstart < 0 || mstep == 0 || sstep == 0)
	{
		return NULL;
	}

	num_points = (int)ceil((mend - mstart)/mstep);
	num_sigma = (int)ceil((send - sstart)/sstep);

	num_sets = n_sets;

	if (!alloc_luria3(num_points, mstart, mstep, num_sigma, sstart, sstep))
	{
		return NULL;
	}

	pval_luria3();
	computepoints_luria3();
	release_luria3();

	return points;
}

void distribution::computepoints_luria3()
{
	if (active_document == NULL)
	{
		perror("computepoints_normal(): active document is null.");
		return;
	}

	double sum;
	int zero;
	double maxd;

	maxd = -10000.0;

	for (int x = 0; x < num_sigma; x++)
		for (int y = 0; y < num_points; y++)
		{
			sum = 0.0;
			zero = 0;

			for (int z = 0; z < num_sets; z++)
			{
				if (pf_table[z*num_sigma*num_points + x*num_points + y] != 0)
					sum += log(pf_table[z*num_sigma*num_points + x*num_points + y])*active_document->counts[z];
				else
					zero = 1;
			}

			if (!zero)
				points[x*num_points + y] = sum;
			else
				points[x*num_points + y] = 1.0;

			if (points[x*num_points + y] < 1.0 && points[x*num_points + y] > maxd)
			{
				maxd = points[x*num_points + y];
				max = m_table[y];
				max_sigma = s_table[x];
			}
		}

	if (maxd == -10000)
		return;

	for (int x = 0; x < num_sigma; x++)
		for (int y = 0; y < num_points; y++)
		{
			if (points[x*num_points + y] < 1.0)
			{
				points[x*num_points + y] -= maxd;
				points[x*num_points + y] = exp(points[x*num_points + y]);
			}
			else
				points[x*num_points + y] = 0.0;
		}
}
