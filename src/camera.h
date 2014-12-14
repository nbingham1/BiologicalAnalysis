#include "vector.h"

#ifndef camera_h
#define camera_h

struct camerahdl;
struct cameraref;

struct camerahdl
{
	vec position;
	vec orientation;
	double scale;

	double speed;
	unsigned int person;

	double *framerate;

	void init(double *rateptr);

	void move(vec v);
	void forward(double mag);
	void backward(double mag);
	void right(double mag);
	void left(double mag);
	void up(double mag);
	void down(double mag);

	void rotate(vec v);

	void setproj();
};

#endif
