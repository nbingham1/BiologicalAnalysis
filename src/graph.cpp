#include "graph.h"

void data::init(char *name, graph_t type, double *heights, double xs, double xe, double xi, double zs, double ze, double zi)
{
	strcpy(title, name);
	verts = NULL;
	norms = NULL;
	indices = NULL;
	xstart = xs;
	xend = xe;
	xstep = xi;
	zstart = zs;
	zend = ze;
	zstep = zi;
	gtype = type;
	interactive = false;
	basedataptr = heights;
	ptrindex = -1;
	show = NULL;
	remakeb = false;
	integer = false;
	snaptomin = false;
	snaptomax = false;

	int hx_size = (int)ceil((xe - xs)/xi + 1.0);
	int hz_size = (int)ceil((ze - zs)/zi + 1.0);
	x_size = hx_size < 2 ? 2 : hx_size;
	z_size = hz_size < 2 ? 2 : hz_size;

	if (x_size == 2)
	{
		xend = xstart + 0.01;
		xstep = 0.01;
		if (gtype != histograph_t)
			gtype = wireframe_t;
	}
	if (z_size == 2)
	{
		zend = zstart + 0.01;
		zstep = 0.01;
		if (gtype != histograph_t)
			gtype = wireframe_t;
	}

	if (hx_size == 1 && hz_size == 1)
		dtype = d1D_t;
	else if (hx_size == 1 || hz_size == 1)
		dtype = d2D_t;
	else
		dtype = d3D_t;

	ystart = 999999999.0;
	yend = -999999999.0;

	double tempmin;
	double tempmax;

	for (int x = 0; x < x_size; x++)
		for (int z = 0; z < z_size; z++)
		{
			tempmin = ystart;
			tempmax = yend;

			ystart = m_min(ystart, heights[(x%hx_size)*hz_size + (z%hz_size)]);
			yend = m_max(yend, heights[(x%hx_size)*hz_size + (z%hz_size)]);

			if (tempmin != ystart)
				ymin = vec((xend - xstart)*(double)x/(double)(x_size-1) + xstart, ystart, (zend - zstart)*(double)z/(double)(z_size-1) + zstart);

			if (tempmax != yend)
				ymax = vec((xend - xstart)*(double)x/(double)(x_size-1) + xstart, yend, (zend - zstart)*(double)z/(double)(z_size-1) + zstart);
		}

	if (type == histograph_t)
	{
		ystart = 0.0;
		x_size = x_size*2;
		z_size = z_size*2;
		verts = new double[x_size*z_size*3];
		norms = new double[x_size*z_size*3];
		indices = new unsigned int[x_size*z_size*2];
		int nx, nz;

		for (int x = 0; x < x_size; x++)
			for (int z = 0; z < z_size; z++)
			{
				nx = (x-1)/2;
				nz = (z-1)/2;
				verts[(x*z_size + z)*3 + 0] = (floor(float(x)/2.0) - 0.5 - xstart)/float(x_size/2-1);
				verts[(x*z_size + z)*3 + 1] = 0.0;
				if (x != 0 && z != 0 && x != x_size-1 && z != z_size-1)
					verts[(x*z_size + z)*3 + 1] += (heights[(nx%hx_size)*hz_size + (nz%hz_size)] - ystart)/(yend - ystart);
				verts[(x*z_size + z)*3 + 2] = (floor(float(z)/2.0) - 0.5 - zstart)/float(z_size/2-1);

				norms[(x*z_size + z)*3 + 0] = 0.0;
				norms[(x*z_size + z)*3 + 1] = 1.0;
				norms[(x*z_size + z)*3 + 2] = 0.0;
			}

		vec n;
		for (int x = 1; x < x_size-1; x++)
			for (int z = 1; z < z_size-1; z++)
			{
				n = vec();
				n += normvec(vec(&verts[((x-1)*z_size + z)*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[(x*z_size + (z-1))*3]));
				n += normvec(vec(&verts[(x*z_size + (z-1))*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[((x+1)*z_size + (z-1))*3]));
				n += normvec(vec(&verts[((x+1)*z_size + (z-1))*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[((x+1)*z_size + z)*3]));
				n += normvec(vec(&verts[((x+1)*z_size + z)*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[(x*z_size + (z+1))*3]));
				n += normvec(vec(&verts[((x-1)*z_size + z)*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[(x*z_size + (z-1))*3]));
				n += normvec(vec(&verts[(x*z_size + (z+1))*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[((x-1)*z_size + z)*3]));
				n /= 6.0;
				norms[(x*z_size + z)*3 + 0] = n.x;
				norms[(x*z_size + z)*3 + 1] = n.y;
				norms[(x*z_size + z)*3 + 2] = n.z;
			}

		int index = 0;
		for (int x = 1; x < x_size; x++)
		{
			for (int z = 0; z < z_size; z++)
			{
				indices[index++] = x*z_size + z;
				indices[index++] = (x-1)*z_size + z;
			}
		}
	}
	else
	{
		verts = new double[x_size*z_size*3];
		norms = new double[x_size*z_size*3];
		indices = new unsigned int[x_size*z_size*2];

		for (int x = 0; x < x_size; x++)
			for (int z = 0; z < z_size; z++)
			{
				verts[(x*z_size + z)*3 + 0] = float(x)/float(x_size-1);
				verts[(x*z_size + z)*3 + 1] = (heights[(x%hx_size)*hz_size + (z%hz_size)] - ystart)/(yend - ystart);
				verts[(x*z_size + z)*3 + 2] = float(z)/float(z_size-1);

				norms[(x*z_size + z)*3 + 0] = 0.0;
				norms[(x*z_size + z)*3 + 1] = 1.0;
				norms[(x*z_size + z)*3 + 2] = 0.0;
			}

		vec n;
		for (int x = 1; x < x_size-1; x++)
			for (int z = 1; z < z_size-1; z++)
			{
				n = vec();
				n += normvec(vec(&verts[((x-1)*z_size + z)*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[(x*z_size + (z-1))*3]));
				n += normvec(vec(&verts[(x*z_size + (z-1))*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[((x+1)*z_size + (z-1))*3]));
				n += normvec(vec(&verts[((x+1)*z_size + (z-1))*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[((x+1)*z_size + z)*3]));
				n += normvec(vec(&verts[((x+1)*z_size + z)*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[(x*z_size + (z+1))*3]));
				n += normvec(vec(&verts[((x-1)*z_size + z)*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[(x*z_size + (z-1))*3]));
				n += normvec(vec(&verts[(x*z_size + (z+1))*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[((x-1)*z_size + z)*3]));
				n /= 6.0;
				norms[(x*z_size + z)*3 + 0] = n.x;
				norms[(x*z_size + z)*3 + 1] = n.y;
				norms[(x*z_size + z)*3 + 2] = n.z;
			}

		int index = 0;
		for (int x = 1; x < x_size; x++)
		{
			for (int z = 0; z < z_size; z++)
			{
				indices[index++] = x*z_size + z;
				indices[index++] = (x-1)*z_size + z;
			}
		}
	}
}

void data::remake(double *heights)
{
	int hx_size = (int)ceil((xend - xstart)/xstep + 1.0);
	int hz_size = (int)ceil((zend - zstart)/zstep + 1.0);

	if (x_size == 2 || (x_size == 4 && gtype == histograph_t))
		hx_size = 1;
	if (z_size == 2 || (z_size == 4 && gtype == histograph_t))
		hz_size = 1;

	ystart = 999999999.0;
	yend = -999999999.0;

	double tempmin;
	double tempmax;

	for (int x = 0; x < x_size; x++)
		for (int z = 0; z < z_size; z++)
		{
			tempmin = ystart;
			tempmax = yend;
			ystart = m_min(ystart, heights[(x%hx_size)*hz_size + (z%hz_size)]);
			yend = m_max(yend, heights[(x%hx_size)*hz_size + (z%hz_size)]);

			if (tempmin != ystart)
				ymin = vec((xend - xstart)*(double)x/(double)(x_size-1) + xstart, ystart, (zend - zstart)*(double)z/(double)(z_size-1) + zstart);

			if (tempmax != yend)
				ymax = vec((xend - xstart)*(double)x/(double)(x_size-1) + xstart, yend, (zend - zstart)*(double)z/(double)(z_size-1) + zstart);
		}

	if (gtype == histograph_t)
	{
		ystart = 0.0;
		int nx, nz;
		for (int x = 0; x < x_size; x++)
			for (int z = 0; z < z_size; z++)
			{
				nx = (x-1)/2;
				nz = (z-1)/2;
				verts[(x*z_size + z)*3 + 1] = 0.0;
				if (x != 0 && z != 0 && x != x_size-1 && z != z_size-1)
					verts[(x*z_size + z)*3 + 1] += (heights[(nx%hx_size)*hz_size + (nz%hz_size)] - ystart)/(yend - ystart);
			}
	}
	else
	{
		for (int x = 0; x < x_size; x++)
			for (int z = 0; z < z_size; z++)
			{
				verts[(x*z_size + z)*3 + 1] = (heights[(x%hx_size)*hz_size + (z%hz_size)] - ystart)/(yend - ystart);

				norms[(x*z_size + z)*3 + 0] = 0.0;
				norms[(x*z_size + z)*3 + 1] = 1.0;
				norms[(x*z_size + z)*3 + 2] = 0.0;
			}

		vec n;
		for (int x = 1; x < x_size-1; x++)
			for (int z = 1; z < z_size-1; z++)
			{
				n = vec();
				n += normvec(vec(&verts[((x-1)*z_size + z)*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[(x*z_size + (z-1))*3]));
				n += normvec(vec(&verts[(x*z_size + (z-1))*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[((x+1)*z_size + (z-1))*3]));
				n += normvec(vec(&verts[((x+1)*z_size + (z-1))*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[((x+1)*z_size + z)*3]));
				n += normvec(vec(&verts[((x+1)*z_size + z)*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[(x*z_size + (z+1))*3]));
				n += normvec(vec(&verts[((x-1)*z_size + z)*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[(x*z_size + (z-1))*3]));
				n += normvec(vec(&verts[(x*z_size + (z+1))*3]), vec(&verts[(x*z_size + z)*3]), vec(&verts[((x-1)*z_size + z)*3]));
				n /= 6.0;
				norms[(x*z_size + z)*3 + 0] = n.x;
				norms[(x*z_size + z)*3 + 1] = n.y;
				norms[(x*z_size + z)*3 + 2] = n.z;
			}
	}
}

void data::release()
{
	if (verts != NULL)
	{
		delete [] verts;
		verts = NULL;
	}
	if (norms != NULL)
	{
		delete [] norms;
		norms = NULL;
	}
	if (indices != NULL)
	{
		delete [] indices;
		indices = NULL;
	}
}

double data::getvalue(double x, double z)
{
	double dx = (x - xstart)/xstep;
	double dz = (z - zstart)/zstep;

	int ix = int(dx);
	int iz = int(dz);

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
	if (iz < 0)
	{
		iz = 0;
		dz = double(iz);
	}
	if (iz+1 >= z_size)
	{
		iz = z_size-2;
		dz = double(iz);
	}

	if (gtype == histograph_t)
	{
		ix = int(dx + 0.5);
		iz = int(dz + 0.5);
		return verts[((ix*2+1)*z_size + (iz*2+1))*3 + 1]*(yend - ystart) + ystart;
	}

	double h00 = verts[(ix*z_size + iz)*3 + 1];
	double h01 = verts[((ix+1)*z_size + iz)*3 + 1];
	double h10 = verts[(ix*z_size + (iz+1))*3 + 1];
	double h11 = verts[((ix+1)*z_size + (iz+1))*3 + 1];

	double tx = dx - double(ix);
	double tz = dz - double(iz);

	double txtz = tx * tz;

	return (h00 * (1.0 - tz - tx + txtz)
		 + h01 * (tx - txtz)
		 + h11 * txtz
		 + h10 * (tz - txtz))*(yend - ystart) + ystart;
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
	glTranslatef(xstart, ystart, zstart);
	glScalef(xend-xstart < 0.01 ? 0.01 : xend - xstart, yend-ystart, zend-zstart < 0.01 ? 0.01 : zend - zstart);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, verts);
	glNormalPointer(GL_DOUBLE, 0, norms);
	for (int x = 0; x < x_size-1; x++)
		glDrawElements(GL_QUAD_STRIP, 2*z_size, GL_UNSIGNED_INT, &indices[x*z_size*2]);

	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(rendcol.x*0.25, rendcol.y*0.25, rendcol.z*0.25);
	for (int x = 0; x < x_size-1; x++)
		glDrawElements(GL_QUAD_STRIP, 2*z_size, GL_UNSIGNED_INT, &indices[x*z_size*2]);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);

	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void tracer::init(data *d, double *x, double *y, double *z, bool *s)
{
	dataptr = d;
	xcon = x;
	ycon = y;
	zcon = z;

	xsel = false;
	zsel = false;
	show = s;
	bscale = vec(1.0, 1.0, 1.0);
}

void tracer::render()
{
	if (xcon != NULL)
		position.x = *xcon;
	if (ycon != NULL)
		*ycon = position.y;
	if (zcon != NULL)
		position.z = *zcon;


	if (dataptr != NULL)
	{
		if (dataptr->x_size > 2 && dataptr->z_size > 2)
			position.y = dataptr->getvalue(position.x, position.z);
		else if (dataptr->x_size == 2)
		{
			position.y = dataptr->getvalue(0.0, position.z);
			position.x = 0.0;
		}
		else if (dataptr->z_size == 2)
		{
			position.y = dataptr->getvalue(position.x, 0.0);
			position.z = 0.0;
		}
	}

	double yoff = 0.0;
	if (dataptr->dtype == d3D_t)
		yoff = 0.001;

	glBegin(GL_LINES);
	glVertex3f(position.x, yoff, 0.0);
	glVertex3f(position.x, yoff, dataptr->zend);

	glVertex3f(position.x, yoff, position.z);
	glVertex3f(position.x, dataptr->yend, position.z);

	glVertex3f(0.0, yoff, position.z);
	glVertex3f(dataptr->xend, yoff, position.z);
	glEnd();

	glPushMatrix();
	glTranslatef(position.x, yoff, 0.0);
	glScalef(bscale.x, bscale.y, bscale.z);
	glutSolidSphere(0.1, 4, 4);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, yoff, position.z);
	glScalef(bscale.x, bscale.y, bscale.z);
	glutSolidSphere(0.1, 4, 4);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(position.x, yoff, dataptr->zend);
	glScalef(bscale.x, bscale.y, bscale.z);
	glutSolidSphere(0.1, 4, 4);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(dataptr->xend, yoff, position.z);
	glScalef(bscale.x, bscale.y, bscale.z);
	glutSolidSphere(0.1, 4, 4);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glScalef(bscale.x, bscale.y, bscale.z);
	glutSolidSphere(0.1, 4, 4);
	glPopMatrix();

	char num[32];
	int l;

	if (dataptr != NULL && (dataptr->x_size == 2 || dataptr->z_size == 2 || (dataptr->gtype == histograph_t && (dataptr->x_size == 4 || dataptr->z_size == 4))))
	{
		if (dataptr->x_size == 2 || (dataptr->gtype == histograph_t && dataptr->x_size == 4))
		{
			sprintf(num, "%f", position.z);
			l = strlen(num);
			glRasterPos3f(position.x, yoff*5.0, dataptr->zend + 0.1);
			for (unsigned int x = 0; x < strlen(num); x++)
				glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);
		}
		else
		{
			sprintf(num, "%f", position.x);
			l = strlen(num);
			glRasterPos3f(dataptr->xend + 0.1, yoff*5.0, position.z);
			for (unsigned int x = 0; x < strlen(num); x++)
				glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);
		}
	}
	else
	{
		sprintf(num, "%f", position.x);
		l = strlen(num);
		glRasterPos3f(position.x, yoff*5.0, dataptr->zend + 0.1);
		for (unsigned int x = 0; x < strlen(num); x++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);

		sprintf(num, "%f", position.z);
		l = strlen(num);
		glRasterPos3f(dataptr->xend + 0.1, yoff*5.0, position.z);
		for (unsigned int x = 0; x < strlen(num); x++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);
	}

	sprintf(num, "%f", position.y);
	l = strlen(num);
	glRasterPos3f(position.x, dataptr->yend, position.z);
	for (unsigned int x = 0; x < strlen(num); x++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);
}

void graph::init(char *name, displayhdl *display, float sx, float sy, float sz)
{
	disp = display;

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

	lnode<tracer*> *tcurr = tracerlist.first;
	while (tcurr != NULL)
	{
		delete tcurr->data;
		tcurr = tcurr->next;
	}

	tracerlist.clear();

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

	datalist.add(dataptr);
}

void graph::remdata(data *dataptr)
{
	lnode<data*> *curr = datalist.first;
	while (curr != NULL && curr->data != dataptr)
		curr = curr->next;

	curr->data->release();
	delete curr->data;

	datalist.rem(curr);
}

void graph::addtracer(tracer *tracerptr)
{
	if (tracerptr == NULL)
		return;

	tracerlist.add(tracerptr);
}

void graph::remtracer(tracer *tracerptr)
{
	lnode<tracer*> *curr = tracerlist.first;
	while (curr != NULL && curr->data != tracerptr)
		curr = curr->next;

	delete curr->data;

	tracerlist.rem(curr);
}

void graph::render()
{
	lnode<data*> *curr = datalist.first;
	dimension = 0;
	bool xa = false;

	bool rendx = false, rendz = false;

	x_size = 0.0;
	y_size = 0.0;
	z_size = 0.0;

	while (curr != NULL)
	{
		if ((curr->data->show == NULL || *curr->data->show) && curr->data->dtype == d3D_t)
			dimension = 3;
		else if ((curr->data->show == NULL || *curr->data->show) && dimension <= 2)
		{
			dimension = 2;

			if (curr->data->xend - curr->data->xstart == curr->data->xstep)
				xa = true;
		}

		if (curr->data->show == NULL || *curr->data->show)
		{
			if (curr->data->xend > x_size)
				x_size = curr->data->xend;

			if (curr->data->yend > y_size)
				y_size = curr->data->yend;

			if (curr->data->zend > z_size)
				z_size = curr->data->zend;

			if (curr->data->xend - curr->data->xstart > curr->data->xstep)
				rendx = true;
			if (curr->data->zend - curr->data->zstart > curr->data->zstep)
				rendz = true;
		}

		curr = curr->next;
	}

	if (rendx && rendz)
		dimension = 3;

	if (!rendx && !rendz)
		rendx = true;

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

	if (rendz && !rendx)
		glRotatef(90.0, 0.0, 1.0, 0.0);

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

	int l;
	char num[32];

	if (rendx)
	{
		sprintf(num, "%d", int(x_size));
		l = strlen(num) + 4;
		sprintf(num, "%f", x_size);
		num[l] = '\0';
		glRasterPos3f(2.00, -0.06, -0.06);
		for (unsigned int x = 0; x < strlen(num); x++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);

		glRasterPos3f(1.9, -0.06, -0.06);
		for (unsigned int x = 0; x < strlen(xlabel); x++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, xlabel[x]);
	}
	else
	{
		sprintf(num, "%d", 0);
		l = strlen(num) + 4;
		sprintf(num, "%f", 0.0);
		num[l] = '\0';
		glRasterPos3f(2.06, -0.06, -0.06);
		for (unsigned int x = 0; x < strlen(num); x++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);
	}

	sprintf(num, "%d", int(y_size));
	l = strlen(num) + 4;
	sprintf(num, "%f", y_size);
	num[l] = '\0';
	glRasterPos3f(-0.06, 2.06, -0.06);
	for (unsigned int x = 0; x < strlen(num); x++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);

	glRasterPos3f(0.0, 2.01, 0.0);
	for (unsigned int x = 0; x < strlen(ylabel); x++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ylabel[x]);

	if (rendz)
	{
		sprintf(num, "%d", int(z_size));
		l = strlen(num) + 4;
		sprintf(num, "%f", z_size);
		num[l] = '\0';
		glRasterPos3f(-0.06, -0.06, 2.00);
		for (unsigned int x = 0; x < strlen(num); x++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);

		glRasterPos3f(-0.06, -0.06, 1.9);
		for (unsigned int x = 0; x < strlen(zlabel); x++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, zlabel[x]);
	}
	else
	{
		sprintf(num, "%d", 0);
		l = strlen(num) + 4;
		sprintf(num, "%f", 0.0);
		num[l] = '\0';
		glRasterPos3f(-0.06, -0.06, 2.06);
		for (unsigned int x = 0; x < strlen(num); x++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, num[x]);
	}

	glScalef(2.0/x_size, 2.0/y_size, 2.0/z_size);

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	lnode<tracer*> *tcurr = tracerlist.first;
	while (tcurr != NULL)
	{
		tcurr->data->bscale = vec(x_size/10.0, y_size/10.0, z_size/10.0);
		if ((tcurr->data->show == NULL || *tcurr->data->show) && tcurr->data != NULL && tcurr->data->dataptr != NULL && (tcurr->data->dataptr->show == NULL || *tcurr->data->dataptr->show))
			tcurr->data->render();

		tcurr = tcurr->next;
	}

	curr = datalist.first;
	while (curr != NULL)
	{
		if (curr->data != NULL && (curr->data->show == NULL || *curr->data->show))
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

void graph::handleclick(float x, float y)
{
	GLfloat z;
	glReadPixels((int)x, (int)y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
	vec pos;
	gluUnProject(x, y, z, modelview, projection, viewport, &pos.x, &pos.y, &pos.z);

	if (x_size <= 0.05)
		pos.x = 0.0;
	if (z_size <= 0.05)
		pos.z = 0.0;

	float sx = 2.2*x/(float)disp->scrn_height - 1.1*(float)disp->scrn_width/(float)disp->scrn_height;
	float sy = 2.2*(disp->scrn_height - y)/disp->scrn_height - 1.1;

	if (sx - scnpos.x <= 1.35 && sx - scnpos.x >= -1.35 && sy - scnpos.y <= 1.35 && sy - scnpos.y >= -1.35)
	{
		float dist;

		lnode<data*> *curr = datalist.first;
		while (curr != NULL)
		{
			if (curr->data->interactive && (curr->data->show == NULL || *curr->data->show))
			{
				curr->data->ptrindex = int((pos.x - curr->data->xstart + .5)/curr->data->xstep);
				if (curr->data->ptrindex < 0 || curr->data->ptrindex >= curr->data->x_size)
				{
					curr->data->ptrindex = -1;
					break;
				}
				dist = curr->data->basedataptr[curr->data->ptrindex] - pos.y;
				dist = dist < 0.0 ? -dist : dist;
				if (dist >= 0.02*y_size)
					curr->data->ptrindex = -1;
				selected = !selected;
			}
			curr = curr->next;
		}

		vec v;
		bool b = !selected;
		lnode<tracer*> *currt = tracerlist.first;
		while (currt != NULL)
		{
			if (currt->data->show == NULL || *currt->data->show)
			{
				v = abs(pos - currt->data->position);
				if (v.x < 0.2)
				{
					currt->data->xsel = !currt->data->xsel;
					selected = b;
				}
				if (v.z < 0.2)
				{
					currt->data->zsel = !currt->data->zsel;
					selected = b;
				}
			}
			currt = currt->next;
		}

		selected = !selected;
		mouse_y = sy;
		mouse_x = sx;
	}
}

void graph::handlerelease(float x, float y)
{
	lnode<data*> *curr = datalist.first;
	while (curr != NULL)
	{
		curr->data->ptrindex = -1;
		curr = curr->next;
	}

	lnode<tracer*> *currt = tracerlist.first;
	while (currt != NULL)
	{
		currt->data->xsel = false;
		currt->data->zsel = false;
		currt = currt->next;
	}
	selected = false;
}

void graph::handledrag(float x, float y)
{
	GLfloat z;

	glReadPixels((int)x, (int)y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
	z = z > 0.7 ? 0.7 : z;

	vec pos;
	gluUnProject(x, y, z, modelview, projection, viewport, &pos.x, &pos.y, &pos.z);

	float sx = 2.2*x/(float)disp->scrn_height - 1.1*(float)disp->scrn_width/(float)disp->scrn_height;
	float sy = 2.2*(disp->scrn_height - y)/disp->scrn_height - 1.1;

	rotate(sx, sy);

	if (x_size <= 0.05)
		pos.x = 0.0;
	if (z_size <= 0.05)
		pos.z = 0.0;

	lnode<data*> *curr = datalist.first;
	while (curr != NULL)
	{
		if (curr->data->ptrindex != -1)
		{
			curr->data->basedataptr[curr->data->ptrindex] = pos.y < 0.0 ? 0.0 : pos.y;
			if (curr->data->integer)
				curr->data->basedataptr[curr->data->ptrindex] = pos.y < 0.0 ? 0.0 : ceil(pos.y);
			curr->data->remakeb = true;
		}
		curr = curr->next;
	}

	double dist;

	lnode<tracer*> *currt = tracerlist.first;
	while (currt != NULL)
	{
		if ((currt->data->show == NULL || *currt->data->show) && (currt->data->dataptr->show == NULL || *currt->data->dataptr->show))
		{
			if (currt->data->xsel && !m_nan(pos.x) &&  (pos.x >= 0.0 && pos.x <= x_size && pos.z >= 0.0 && pos.z <= z_size && pos.y >= -0.1 && pos.y <= y_size))
			{
				currt->data->position.x = pos.x;
				dist = currt->data->position.x - currt->data->dataptr->ymax.x;
				dist = dist < 0.0 ? -dist : dist;

				if (currt->data->dataptr->snaptomax && dist < x_size/50.0)
					currt->data->position.x = currt->data->dataptr->ymax.x;

				dist = currt->data->position.x - currt->data->dataptr->ymin.x;
				dist = dist < 0.0 ? -dist : dist;

				if (currt->data->dataptr->snaptomin && dist < x_size/50.0)
					currt->data->position.x = currt->data->dataptr->ymin.x;

				if (currt->data->xcon)
				{
					*currt->data->xcon = currt->data->position.x;
					currt->data->dataptr->remakeb = true;
				}
			}
			if (currt->data->zsel && !m_nan(pos.z) &&  (pos.x >= 0.0 && pos.x <= x_size && pos.z >= 0.0 && pos.z <= z_size && pos.y >= -0.1 && pos.y <= y_size))
			{
				currt->data->position.z = pos.z;
				dist = currt->data->position.z - currt->data->dataptr->ymax.z;
				dist = dist < 0.0 ? -dist : dist;

				if (currt->data->dataptr->snaptomax && dist < double(z_size)/50.0)
					currt->data->position.z = currt->data->dataptr->ymax.z;

				dist = currt->data->position.z - currt->data->dataptr->ymin.z;
				dist = dist < 0.0 ? -dist : dist;

				if (currt->data->dataptr->snaptomin && dist < double(z_size)/50.0)
					currt->data->position.z = currt->data->dataptr->ymin.z;

				if (currt->data->zcon)
				{
					*currt->data->zcon = currt->data->position.z;
					currt->data->dataptr->remakeb = true;
				}
			}

			if (currt->data->ycon != NULL)
				currt->data->dataptr->remakeb = true;
		}

		currt = currt->next;
	}
}
