/*
 *  display.cpp
 *  Game
 *
 *  Created by Ned Bingham on 9/13/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "display.h"

void displayhdl::viewport(int w, int h)
{
	scrn_width = w;
	scrn_height = h;
}

void displayhdl::set(int p, int o)
{
	glRasterPos2f((2.2*double(p*9)/double(scrn_height) - 1.1*double(scrn_width)/double(scrn_height)), -(2.2*double(o*15 + 15)/double(scrn_height) - 1.1));
	pos = p;
	off = o;
}

void displayhdl::put(char c)
{
	switch (c)
	{
		case '\n':
			set(0, off+1);
			break;
		case '\t':
			for (int x = 0; x < 5; x++)
				put(' ');
			break;
		default:
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
			pos++;
			break;
	}
}

void displayhdl::wprint(char *str)
{
	for(unsigned int i = 0; i < strlen(str); i++)
		put(str[i]);
}

void displayhdl::box(float r, float l, float t, float b)
{
	glBegin(GL_QUADS);
		glVertex3f((2.2*double(l*9)/double(scrn_height) - 1.1*double(scrn_width)/double(scrn_height)), -(2.2*double((b-1)*15 + 15)/double(scrn_height) - 1.1), 0.5);
		glVertex3f((2.2*double(l*9)/double(scrn_height) - 1.1*double(scrn_width)/double(scrn_height)), -(2.2*double((t-1)*15 + 15)/double(scrn_height) - 1.1), 0.5);
		glVertex3f((2.2*double(r*9)/double(scrn_height) - 1.1*double(scrn_width)/double(scrn_height)), -(2.2*double((t-1)*15 + 15)/double(scrn_height) - 1.1), 0.5);
		glVertex3f((2.2*double(r*9)/double(scrn_height) - 1.1*double(scrn_width)/double(scrn_height)), -(2.2*double((b-1)*15 + 15)/double(scrn_height) - 1.1), 0.5);
	glEnd();
}

vec displayhdl::tooglc(vec p)
{
	return vec((2.2*double(p.x*9)/double(scrn_height) - 1.1*double(scrn_width)/double(scrn_height)),
			   -(2.2*double((p.y-1)*15 + 15)/double(scrn_height) - 1.1));
}
