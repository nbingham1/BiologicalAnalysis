#ifndef mathdef_h
#define mathdef_h

#include <math.h>
#include "vector.h"

#define m_pi 3.14159265358979323846
#define m_epsilon 1E-7

#define m_sqr(n) (n*n)
#define m_cube(n) (n*n*n)

#define m_abs(n) (n < 0.0 ? -n : n)
#define m_min(m, n) (m < n ? m : n)
#define m_max(m, n) (m < n ? n : m)

#define m_nan(n) (!(n > 0 || n <= 0))

#define m_clamp(n, a, b) m_min(m_max(n, a), b)

#define m_even(n) (int(n/2) == double(n)/2.0 ? true : false)
#define m_odd(n) (int(n/2) == double(n)/2.0 ? false : true)

#define degtorad(a) m_pi*a/180.0
#define radtodeg(a) 180.0*a/m_pi

int m_fac(int n);

int intraytri(vec orig, vec dir, vec vert0, vec vert1, vec vert2, double *t, double *u, double *v);

#endif
