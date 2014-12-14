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

	int x_size;
	int y_size;

	double xstart;
	double xend;
	double xstep;
	double ystart;
	double yend;
	double ystep;
	double zstart;
	double zend;

	vec rendcol;

	bool *show;
	bool interactive;
	bool selected;
	int sindex;
	int sindex2;

	graph_t gtype;
	dimension_t dtype;

	void init(char *name, graph_t type, double *heights, double xs, double xe, double xi, double ys, double ye, double yi);
	void release();

	double getvalue(double x, double y);

	void render();

	void combine(data *d1, data *d2);
};

struct graph
{
	llist<data*> datalist;
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

	void init(char *name, float sx, float sy, float sz);
	void release();

	void adddata(data *dataptr);
	void remdata(data *dataptr);

	void render(displayhdl *disp);

	void handleclick(float x, float y);
	void rotate(float x, float y);
	void handledrag(float x, float y);
};

#endif
