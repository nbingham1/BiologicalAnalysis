#include "camera.h"
#include "standard.h"
#include "graphics.h"
#include "mathdef.h"

void camerahdl::init(double *rateptr)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.6/10.0, 1.6/10.0, -1.0/10.0, 1.0/10.0, .2, 999999999);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	position = vec();

	//scale = 1.0;
	person = 1;
	speed = 1.0;

	framerate = rateptr;
}

void camerahdl::move(vec v)
{
	vec o2;

	//if (attachment != NULL)
		//o2 = degtorad(attachment->physics->orientation);

	vec o1 = -degtorad(orientation);

	//if (attachment != NULL && attachment->physics != NULL)
	//	attachment->physics->forces += speed*(v.x*rotx(roty(rotz(rotz(roty(rotx(vec(1.0, 0.0, 0.0), o1.x), o1.y), o1.z), o2.z), o2.y), o2.x) +
	//										  v.y*rotx(roty(rotz(rotz(roty(rotx(vec(0.0, 1.0, 0.0), o1.x), o1.y), o1.z), o2.z), o2.y), o2.x) +
	//										  v.z*rotx(roty(rotz(rotz(roty(rotx(vec(0.0, 0.0, 1.0), o1.x), o1.y), o1.z), o2.z), o2.y), o2.x));
	//else
		position += (speed/(*framerate))*(v.x*rotx(roty(rotz(rotz(roty(rotx(vec(1.0, 0.0, 0.0), o1.x), o1.y), o1.z), o2.z), o2.y), o2.x) +
						   v.y*rotx(roty(rotz(rotz(roty(rotx(vec(0.0, 1.0, 0.0), o1.x), o1.y), o1.z), o2.z), o2.y), o2.x) +
						   v.z*rotx(roty(rotz(rotz(roty(rotx(vec(0.0, 0.0, 1.0), o1.x), o1.y), o1.z), o2.z), o2.y), o2.x));
}

void camerahdl::forward(double mag)
{
	move(vec(0.0, 0.0, mag));
}

void camerahdl::backward(double mag)
{
	move(vec(0.0, 0.0, -mag));
}

void camerahdl::right(double mag)
{
	move(vec(-mag, 0.0, 0.0));
}

void camerahdl::left(double mag)
{
	move(vec(mag, 0.0, 0.0));
}

void camerahdl::up(double mag)
{
	move(vec(0.0, -mag, 0.0));
}

void camerahdl::down(double mag)
{
	move(vec(0.0, mag, 0.0));
}

void camerahdl::rotate(vec v)
{
	orientation.x += v.x;
	orientation.y += v.y;
	orientation.z += v.z;
	if (orientation.x >= 90.0)
		orientation.x = 90.0;

	else if (orientation.x <= -80.0)
		orientation.x = -80.0;
}

void camerahdl::setproj()
{
    glRotatef(orientation.x, 1.0, 0.0, 0.0);
    glRotatef(orientation.y, 0.0, 1.0, 0.0);
    glRotatef(orientation.z, 0.0, 0.0, 1.0);

    glTranslatef(position.x, position.y, position.z);
}
