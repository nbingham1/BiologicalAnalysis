#include "graphics.h"
#include "standard.h"
#include "mathdef.h"
#include "display.h"
#include "gui.h"

#ifndef graph_h
#define graph_h

enum graph_t
{
	histograph_t,
	pointcloud_t,
	wireframe_t,
	surfaceplot_t
};

enum dimension_t
{
	d1D_t,
	d2D_t,
	d3D_t,
	d4D_t
};

struct data
{
	char title[32];

	double *verts;
	double *norms;
	unsigned int *indices;

	double *basedataptr;
	int ptrindex;

	int x_size;
	int z_size;

	double xstart;
	double xend;
	double xstep;
	double ystart;
	double yend;
	double zstart;
	double zend;
	double zstep;

	vec ymin;
	vec ymax;

	vec rendcol;

	bool *show;
	bool interactive;
	bool remakeb;
	bool integer;
	bool snaptomin;
	bool snaptomax;

	graph_t gtype;
	dimension_t dtype;

	void init(char *name, graph_t type, double *heights, double xs, double xe, double xi, double zs, double ze, double zi);
	void remake(double *heights);
	void release();

	double getvalue(double x, double z);

	void render();
};

struct tracer
{
	vec position;
	vec bscale;

	double *xcon,
		   *ycon,
		   *zcon;

	bool xsel, zsel;
	bool *show;

	data *dataptr;

	void init(data *d, double *x, double *y, double *z, bool *s);
	void render();
};

struct graph
{
	llist<data*> datalist;
	llist<tracer*> tracerlist;
	float x_size;
	float y_size;
	float z_size;

	float x_angle;
	float y_angle;

	float mouse_x;
	float mouse_y;

	char title[32];

	char xlabel[16];
	char ylabel[16];
	char zlabel[16];

	int dimension;

	vec scnpos;

	bool grid;
	bool selected;

	GLdouble modelview[16];
	GLdouble projection[16];
	GLint viewport[4];

	displayhdl *disp;

	void init(char *name, displayhdl *display, float sx, float sy, float sz);
	void release();

	void adddata(data *dataptr);
	void remdata(data *dataptr);

	void addtracer(tracer *tracerptr);
	void remtracer(tracer *tracerptr);

	void render();

	void rotate(float x, float y);

	void handleclick(float x, float y);
	void handlerelease(float x, float y);
	void handledrag(float x, float y);
};

#endif
