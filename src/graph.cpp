#include "graph.h"

void data::init(char *name, graph_t type, double *heights, double xs, double xe, double xi, double ys, double ye, double yi)
{
	strcpy(title, name);
	verts = NULL;
	norms = NULL;
	indices = NULL;
	xstart = xs;
	xend = xe;
	xstep = xi;
	ystart = ys;
	yend = ye;
	ystep = yi;
	gtype = type;
	interactive = false;
	selected = false;
	show = NULL;

	int hx_size = (int)ceil((xe - xs)/xi + 1.0);
	int hy_size = (int)ceil((ye - ys)/yi + 1.0);
	x_size = hx_size < 2 ? 2 : hx_size;
	y_size = hy_size < 2 ? 2 : hy_size;

	if (x_size == 2)
	{
		xend = xstart + 0.01;
		xstep = 0.01;
		if (gtype != histograph_t)
			gtype = wireframe_t;
	}
	if (y_size == 2)
	{
		yend = ystart + 0.01;
		ystep = 0.01;
		if (gtype != histograph_t)
			gtype = wireframe_t;
	}

	if (hx_size == 1 && hy_size == 1)
		dtype = d1D_t;
	else if (hx_size == 1 || hy_size == 1)
		dtype = d2D_t;
	else
		dtype = d3D_t;

	zstart = 999999999.0;
	zend = -999999999.0;

	for (int x = 0; x < x_size; x++)
		for (int y = 0; y < y_size; y++)
		{
			zstart = m_min(zstart, heights[(x%hx_size)*hy_size + (y%hy_size)]);
			zend = m_max(zend, heights[(x%hx_size)*hy_size + (y%hy_size)]);
		}

	if (type == histograph_t)
	{
		x_size = x_size*2;
		y_size = y_size*2;
		verts = new double[x_size*y_size*3];
		norms = new double[x_size*y_size*3];
		indices = new unsigned int[x_size*y_size*2];
		int nx, ny;

		for (int x = 0; x < x_size; x++)
			for (int y = 0; y < y_size; y++)
			{
				nx = (x-1)/2;
				ny = (y-1)/2;
				verts[(x*y_size + y)*3 + 0] = (floor(float(x)/2.0) - 0.5 - xstart)/float(x_size/2-1);
				verts[(x*y_size + y)*3 + 1] = 0.0;
				if (x != 0 && y != 0 && x != x_size-1 && y != y_size-1)
					verts[(x*y_size + y)*3 + 1] += (heights[(nx%hx_size)*hy_size + (ny%hy_size)] - zstart)/(zend - zstart);
				verts[(x*y_size + y)*3 + 2] = (floor(float(y)/2.0) - 0.5 - ystart)/float(y_size/2-1);

				norms[(x*y_size + y)*3 + 0] = 0.0;
				norms[(x*y_size + y)*3 + 1] = 1.0;
				norms[(x*y_size + y)*3 + 2] = 0.0;
			}

		vec n;
		for (int x = 1; x < x_size-1; x++)
			for (int y = 1; y < y_size-1; y++)
			{
				n = vec();
				n += normvec(vec(&verts[((x-1)*y_size + y)*3]), vec(&verts[(x*y_size + y)*3]), vec(&verts[(x*y_size + (y-1))*3]));
				n += normvec(vec(&verts[(x*y_size + (y-1))*3]), vec(&verts[(x*y_size + y)*3]), vec(&verts[((x+1)*y_size + (y-1))*3]));
				n += normvec(vec(&verts[((x+1)*y_size + (y-1))*3]), vec(&verts[(x*y_size + y)*3]), vec(&verts[((x+1)*y_size + y)*3]));
				n += normvec(vec(&verts[((x+1)*y_size + y)*3]), vec(&verts[(x*y_size + y)*3]), vec(&verts[(x*y_size + (y+1))*3]));
				n += normvec(vec(&verts[((x-1)*y_size + y)*3]), vec(&verts[(x*y_size + y)*3]), vec(&verts[(x*y_size + (y-1))*3]));
				n += normvec(vec(&verts[(x*y_size + (y+1))*3]), vec(&verts[(x*y_size + y)*3]), vec(&verts[((x-1)*y_size + y)*3]));
				n /= 6.0;
				norms[(x*y_size + y)*3 + 0] = n.x;
				norms[(x*y_size + y)*3 + 1] = n.y;
				norms[(x*y_size + y)*3 + 2] = n.z;
			}

		int index = 0;
		for (int x = 1; x < x_size; x++)
		{
			for (int y = 0; y < y_size; y++)
			{
				indices[index++] = x*y_size + y;
				indices[index++] = (x-1)*y_size + y;
			}
		}
	}
	else
	{
		verts = new double[x_size*y_size*3];
		norms = new double[x_size*y_size*3];
		indices = new unsigned int[x_size*y_size*2];

		for (int x = 0; x < x_size; x++)
			for (int y = 0; y < y_size; y++)
			{
				verts[(x*y_size + y)*3 + 0] = float(x)/float(x_size-1);
				verts[(x*y_size + y)*3 + 1] = (heights[(x%hx_size)*hy_size + (y%hy_size)] - zstart)/(zend - zstart);
				verts[(x*y_size + y)*3 + 2] = float(y)/float(y_size-1);

				norms[(x*y_size + y)*3 + 0] = 0.0;
				norms[(x*y_size + y)*3 + 1] = 1.0;
				norms[(x*y_size + y)*3 + 2] = 0.0;
			}

		vec n;
		for (int x = 1; x < x_size-1; x++)
			for (int y = 1; y < y_size-1; y++)
			{
				n = vec();
				n += normvec(vec(&verts[((x-1)*y_size + y)*3]), vec(&verts[(x*y_size + y)*3]), vec(&verts[(x*y_size + (y-1))*3]));
				n += normvec(vec(&verts[(x*y_size + (y-1))*3]), vec(&verts[(x*y_size + y)*3]), vec(&verts[((x+1)*y_size + (y-1))*3]));
				n += normvec(vec(&verts[((x+1)*y_size + (y-1))*3]), vec(&verts[(x*y_size + y)*3]), vec(&verts[((x+1)*y_size + y)*3]));
				n += normvec(vec(&verts[((x+1)*y_size + y)*3]), vec(&verts[(x*y_size + y)*3]), vec(&verts[(x*y_size + (y+1))*3]));
				n += normvec(vec(&verts[((x-1)*y_size + y)*3]), vec(&verts[(x*y_size + y)*3]), vec(&verts[(x*y_size + (y-1))*3]));
				n += normvec(vec(&verts[(x*y_size + (y+1))*3]), vec(&verts[(x*y_size + y)*3]), vec(&verts[((x-1)*y_size + y)*3]));
				n /= 6.0;
				norms[(x*y_size + y)*3 + 0] = n.x;
				norms[(x*y_size + y)*3 + 1] = n.y;
				norms[(x*y_size + y)*3 + 2] = n.z;
			}

		int index = 0;
		for (int x = 1; x < x_size; x++)
		{
			for (int y = 0; y < y_size; y++)
			{
				indices[index++] = x*y_size + y;
				indices[index++] = (x-1)*y_size + y;
			}
		}
	}
}

void data::release()
{
	if (verts != NULL)
		delete [] verts;
	if (norms != NULL)
		delete [] norms;
	if (indices != NULL)
		delete [] indices;
}

double data::getvalue(double x, double y)
{
	double dx = (x - xstart)/xstep;
	double dy = (y - ystart)/ystep;

	int ix = int(dx);
	int iy = int(dy);

	if (ix < 0)
	{
		ix = 0;
		dx = double(ix);
	}
	if (ix+1 >= x_size)
	{
		ix = x_size-2;
		dx = double(ix);
	}
	if (iy < 0)
	{
		iy = 0;
		dy = double(iy);
	}
	if (iy+1 >= y_size)
	{
		iy = y_size-2;
		dy = double(iy);
	}

	double h00 = verts[(ix*y_size + iy)*3 + 1];
	double h01 = verts[((ix+1)*y_size + iy)*3 + 1];
	double h10 = verts[(ix*y_size + (iy+1))*3 + 1];
	double h11 = verts[((ix+1)*y_size + (iy+1))*3 + 1];

	double tx = dx - double(ix);
	double ty = dy - double(iy);

	double txty = tx * ty;

	return h00 * (1.0 - ty - tx + txty)
		 + h01 * (tx - txty)
		 + h11 * txty
		 + h10 * (ty - txty);
}

void data::render()
{
	if (gtype == pointcloud_t)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	else if (gtype == wireframe_t)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (gtype == surfaceplot_t)
	{
		glEnable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (gtype == histograph_t)
	{
		if (dtype == d3D_t)
		{
			glEnable(GL_LIGHTING);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	glPushMatrix();
	glColor3f(rendcol.x, rendcol.y, rendcol.z);
	glTranslatef(xstart, zstart, ystart);
	glScalef(xend-xstart < 0.01 ? 0.01 : xend - xstart, zend-zstart, yend-ystart < 0.01 ? 0.01 : yend - ystart);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, verts);
	glNormalPointer(GL_DOUBLE, 0, norms);
	for (int x = 0; x < x_size-1; x++)
		glDrawElements(GL_QUAD_STRIP, 2*y_size, GL_UNSIGNED_INT, &indices[x*y_size*2]);

	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(rendcol.x*0.25, rendcol.y*0.25, rendcol.z*0.25);
	for (int x = 0; x < x_size-1; x++)
		glDrawElements(GL_QUAD_STRIP, 2*y_size, GL_UNSIGNED_INT, &indices[x*y_size*2]);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);

	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void data::combine(data *d1, data *d2)
{

}

void graph::init(char *name, float sx, float sy, float sz)
{
	datalist.init();
	x_size = sx;
	y_size = sy;
	z_size = sz;

	grid = true;
	selected = false;

	dimension = 0;

	strcpy(title, name);

	strcpy(xlabel, "x\0");
	strcpy(ylabel, "y\0");
	strcpy(zlabel, "z\0");

	x_angle = 30.0;
	y_angle = -45.0;
}

void graph::release()
{
	lnode<data*> *curr = datalist.first;
	while (curr != NULL)
	{
		curr->data->release();
		delete curr->data;
		curr = curr->next;
	}

	datalist.clear();

	dimension = 0;
}

void graph::adddata(data *dataptr)
{
	if (dataptr == NULL)
		return;

	if (dataptr->dtype == d1D_t && dimension < 1)
		dimension = 1;
	else if (dataptr->dtype == d2D_t && dimension < 2)
		dimension = 2;
	else if (dataptr->dtype == d3D_t && dimension < 3)
		dimension = 3;
	else if (dataptr->dtype == d4D_t && dimension < 4)
		dimension = 4;

	if (-dataptr->xstart > x_size)
		x_size = -dataptr->xstart;
	if (dataptr->xend > x_size)
		x_size = dataptr->xend;

	if (-dataptr->ystart > z_size)
		z_size = -dataptr->ystart;
	if (dataptr->yend > z_size)
		z_size = dataptr->yend;

	if (-dataptr->zstart > y_size)
		y_size = -dataptr->zstart;
	if (dataptr->zend > y_size)
		y_size = dataptr->zend;

	printf("%f\n", dataptr->yend);

	datalist.add(dataptr);
}

void graph::remdata(data *dataptr)
{
	lnode<data*> *curr = datalist.first;
	while (curr != NULL && curr->data != dataptr)
	{
		curr->data->release();
		delete curr->data;
		curr = curr->next;
	}
	datalist.rem(curr);
}

void graph::render(displayhdl *disp)
{
	lnode<data*> *curr = datalist.first;
	dimension = 0;
	bool xa = false, ya = false;
	while (curr != NULL)
	{
		if ((curr->data->show == NULL || *curr->data->show) && curr->data->dtype == d3D_t)
			dimension = 3;
		else if ((curr->data->show == NULL || *curr->data->show) && dimension <= 2)
		{
			dimension = 2;

			if (curr->data->xend - curr->data->xstart == curr->data->xstep)
				xa = true;
			if (curr->data->yend - curr->data->ystart == curr->data->ystep)
				ya = true;
		}

		curr = curr->next;
	}

	glPushMatrix();

	glTranslatef(scnpos.x, scnpos.y, scnpos.z);
	glRasterPos3f(-0.4, 1.05, 0.0);
	for (unsigned int x = 0; x < strlen(title); x++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, title[x]);

	glPopMatrix();

	glPushMatrix();

	if (dimension <= 2)
	{
		glTranslatef(scnpos.x - 1.0, scnpos.y - 1.0, scnpos.z - 1.0);
	}
	else if (dimension == 3)
	{
		glTranslatef(scnpos.x*1.8, scnpos.y*1.8, scnpos.z*1.8 - 0.5);
		glRotatef(x_angle, 1.0, 0.0, 1.15*scnpos.x/((float)disp->scrn_width/(float)disp->scrn_height));
		glRotatef(y_angle, 0.0, 1.0, 0.0);
		glScalef(1.4, 1.4, 1.4);
		glTranslatef(-1.0, -1.0, -1.0);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glFrustum(-(float)disp->scrn_width/(float)disp->scrn_height, (float)disp->scrn_width/(float)disp->scrn_height, -1.0, 1.0, 2.0, 9999);
		glMatrixMode(GL_MODELVIEW);
	}

	if (grid)
	{
		glColor3f(0.25, 0.25, 0.25);
		glBegin(GL_LINES);

		float density = 0.25;
		for (float x = 0.0; x <= 1.001; x+=density/x_size)
		{
			if (x != 0.0)
			{
				glVertex3f(x*2, 0.0, 0.0);
				glVertex3f(x*2, 2.0, 0.0);

				glVertex3f(x*2, 0.0, 0.0);
				glVertex3f(x*2, 0.0, 2.0);
			}
		}

		for (float x = 0.0; x <= 1.001; x+=density/y_size)
		{
			if (x != 0.0)
			{
				glVertex3f(0.0, x*2, 0.0);
				glVertex3f(2.0, x*2, 0.0);

				glVertex3f(0.0, x*2, 0.0);
				glVertex3f(0.0, x*2, 2.0);
			}
		}

		if (dimension == 3)
		{
			for (float x = 0.0; x <= 1.001; x+=density/z_size)
			{
				if (x != 0.0)
				{
					glVertex3f(0.0, 0.0, x*2);
					glVertex3f(2.0, 0.0, x*2);

					glVertex3f(0.0, 0.0, x*2);
					glVertex3f(0.0, 2.0, x*2);
				}
			}
		}
		glEnd();
		if (dimension <= 2)
			glClear(GL_DEPTH_BUFFER_BIT);

		glColor3f(1.0, 1.0, 1.0);
	}

	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(2.0, 0.0, 0.0);

		for (float x = 0.0; x <= 2.0; x+=2.0/x_size)
		{
			glVertex3f(x, -0.025, 0.0);
			glVertex3f(x, 0.025, 0.0);
			glVertex3f(x, 0.0, -0.025);
			glVertex3f(x, 0.0, 0.025);
		}

		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 2.0, 0.0);

		for (float x = 0.0; x <= 2.0; x+=2.0/y_size)
		{
			glVertex3f(-0.025, x, 0.0);
			glVertex3f(0.025, x, 0.0);
			glVertex3f(0.0, x, -0.025);
			glVertex3f(0.0, x, 0.025);
		}

		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 2.0);

		for (float x = 0.0; x <= 2.0; x+=2.0/z_size)
		{
			glVertex3f(-0.025, 0.0, x);
			glVertex3f(0.025, 0.0, x);
			glVertex3f(0.0, -0.025, x);
			glVertex3f(0.0, 0.025, x);
		}
	glEnd();

	if (dimension == 2 && xa)
		glRotatef(90.0, 0.0, 1.0, 0.0);

	glRasterPos3f(2.01, 0.0, 0.0);
	for (unsigned int x = 0; x < strlen(xlabel); x++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, xlabel[x]);

	glRasterPos3f(0.0, 2.01, 0.0);
	for (unsigned int x = 0; x < strlen(ylabel); x++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ylabel[x]);

	glRasterPos3f(0.0, 0.0, 2.01);
	for (unsigned int x = 0; x < strlen(zlabel); x++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, zlabel[x]);

	int l;
	char num[32];
	sprintf(num, "%d\0", int(x_size));
	l = strlen(num) + 2;
	sprintf(num, "%f\0", x_size);
	num[l] = '\0';
	glRasterPos3f(2.06, -0.06, -0.06);
	for (unsigned int x = 0; x < strlen(num); x++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);

	sprintf(num, "%d\0", int(y_size));
	l = strlen(num) + 2;
	sprintf(num, "%f\0", y_size);
	num[l] = '\0';
	glRasterPos3f(-0.16, 2.06, -0.16);
	for (unsigned int x = 0; x < strlen(num); x++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);

	if (dimension == 3)
	{
		sprintf(num, "%d\0", int(z_size));
		l = strlen(num) + 2;
		sprintf(num, "%f\0", z_size);
		num[l] = '\0';
		glRasterPos3f(-0.06, -0.06, 2.16);
		for (unsigned int x = 0; x < strlen(num); x++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);
	}
	else
	{
		sprintf(num, "%d\0", 0);
		l = strlen(num) + 2;
		sprintf(num, "%f\0", 0.0);
		num[l] = '\0';
		glRasterPos3f(-0.06, -0.06, 2.16);
		for (unsigned int x = 0; x < strlen(num); x++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);
	}

	glScalef(2.0/x_size, 2.0/y_size, 2.0/z_size);

	curr = datalist.first;
	while (curr != NULL)
	{
		if (curr->data && (curr->data->show == NULL || *curr->data->show))
			curr->data->render();
		curr = curr->next;
	}

	glPopMatrix();

	if (dimension == 3)
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
}

void graph::handleclick(float x, float y)
{
	if (x - scnpos.x <= 1.0 && x - scnpos.x >= -1.0 && y - scnpos.y <= 1.0 && y - scnpos.y >= -1.0)
	{
		float xpos = (x-scnpos.x)*x_size;
		float ypos = (y-scnpos.y)*y_size;

		float val;

		lnode<data*> *curr = datalist.first;
		while (curr != NULL)
		{
			if (curr->data->interactive)
			{
				val = curr->data->verts[int((xpos - curr->data->xstart)/curr->data->xstep)*curr->data->y_size*3 + 1] - ypos*(curr->data->zend - curr->data->zstart);
				val = val < 0.0 ? -val : val;
				printf("%f\n", val);
				if (val < 0.2)
				{
					curr->data->selected = true;
					curr->data->sindex = int((xpos - curr->data->xstart)/curr->data->xstep)*curr->data->y_size*3 + 1;
					curr->data->sindex2 = (int((xpos - curr->data->xstart)/curr->data->xstep)*curr->data->y_size + 1)*3 + 1;
					selected = !selected;
					break;
				}
			}
			curr = curr->next;
		}

		selected = !selected;
		mouse_y = y;
		mouse_x = x;
	}
}

void graph::rotate(float x, float y)
{
	if (selected)
	{
		y_angle += 90.0*(x - mouse_x);
		mouse_x = x;
		x_angle += 90.0*(y - mouse_y);
		mouse_y = y;
	}
}

void graph::handledrag(float x, float y)
{
	rotate(x, y);

	float xpos = (x-scnpos.x)*x_size;
	float ypos = (y-scnpos.y)*y_size;

	lnode<data*> *curr = datalist.first;
	while (curr != NULL)
	{
		if (curr->data->selected)
		{
			curr->data->verts[curr->data->sindex] = ypos*(curr->data->zend - curr->data->zstart);
			curr->data->verts[curr->data->sindex2] = ypos*(curr->data->zend - curr->data->zstart);
			break;
		}
		curr = curr->next;
	}
}
