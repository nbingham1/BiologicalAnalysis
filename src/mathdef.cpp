#include "mathdef.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int m_fac(int n)
{
	int r = 1;

	for (int x = 1; x <= n; x++)
		r *= x;

	return r;
}

int intraytri(vec orig, vec dir, vec vert0, vec vert1, vec vert2, double *t, double *u, double *v)
{
	bool cull = true;
	vec edge1, edge2, tvec, pvec, qvec;
	double det , inv_det;
	edge1 = vert1 - vert0;
	edge2 = vert2 - vert0;
	pvec = cross(dir, edge2);
	det = dot(edge1, pvec);

	if (cull)
	{
		if (det < m_epsilon)
			return 0;
		tvec = orig - vert0;
		*u = dot(tvec, pvec);
		if (*u < 0.0 || *u > det)
			return 0;
		qvec = cross(tvec, edge1);
		*v = dot(dir, qvec);
		if (*v < 0.0 || *u + *v > det)
			return 0;
		*t = dot(edge2, qvec);
		inv_det = 1.0 / det;
		*t *= inv_det;
		*u *= inv_det;
		*v *= inv_det;
	}
	else
	{
		if (det > -m_epsilon && det < m_epsilon)
			return 0;
		inv_det = 1.0 / det;
		tvec = orig - vert0;
		*u = dot(tvec, pvec) * inv_det;
		if (*u < -m_epsilon || *u > 1.0 + m_epsilon)
			return 0;
		qvec = cross(tvec, edge1);
		*v = dot(dir, qvec) * inv_det;
		if (*v < -m_epsilon || *u + *v > 1.0 + m_epsilon)
			return 0;
		*t = dot(edge2, qvec) * inv_det;
	}
	return 1;
}

