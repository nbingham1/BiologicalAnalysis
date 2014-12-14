#include "graphics.h"
#include "standard.h"
#include "mathdef.h"

#include "renderer.h"

#include "keyboard.h"
#include "mouse.h"

renderhdl renderer;
keyboardhdl keys;
mousehdl mouse;
float currtime = -100.0;
float starttime = 0.0;

float gettime()
{
	timeval gtime;
	gettimeofday(&gtime, NULL);
	return gtime.tv_sec + gtime.tv_usec*1.0E-6 - starttime;
}

void init()
{
   	glShadeModel(GL_SMOOTH);

	glClearColor(0.0, 0.0, 0.0, 0.0);

	renderer.init();

	starttime = gettime();
}

void displayfunc()
{
	renderer.displayf();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	renderer.display.viewport(w, h);
	mouse.init(200.0, h, w);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 999999999);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mousefunc(int button, int state, int x, int y)
{
	float off, pos;
	pos = (float)x/9.0;
	off = (float)y/15.0;

	renderer.mouse_x = pos;
	renderer.mouse_y = off;
	renderer.mouse_b = button;

	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			float t = gettime();
			if (t - currtime <= 0.25)
			{
				node *n = renderer.findnode(pos, off);
				if (n != NULL)
					n->open = !n->open;
				currtime = -100.0;
			}
			else
			{
				currtime = t;
				renderer.ssection = NULL;
				renderer.snode = NULL;
				renderer.ssection = renderer.findsection(pos, off);
				if (renderer.ssection != NULL)
					renderer.ssection->handledown(pos, off);

				if (renderer.ssection == NULL)
				{
					renderer.snode = renderer.findnode(pos, off);
					if (renderer.snode != NULL)
						renderer.snode->handleclick(pos, off);
				}
			}
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			renderer.snode = renderer.findnode(pos, off);
		}
	}
	else if (state == GLUT_UP)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			if (renderer.sobject != NULL)
			{
				section *s = renderer.findsection(pos, off);

				if (s == NULL)
				{
					if (renderer.ssection->limited_supply)
					{
						renderer.nodelist.add(node(renderer.sobject, vec(pos, off, 0.0)));
						lnode<object*> *curr = renderer.ssection->contents.first;
						while (curr != NULL && curr->data != renderer.sobject)
							curr = curr->next;

						renderer.ssection->contents.rem(curr);
						renderer.sobject = NULL;
					}
					else
					{
						object *new_obj = renderer.sobject->makecopy();

						renderer.objectlist.add(new_obj);
						renderer.nodelist.add(node(new_obj, vec(pos, off, 0.0)));
						renderer.sobject = NULL;
					}
				}
				else
					renderer.sobject = NULL;
			}

			if (renderer.ssection != NULL)
				renderer.ssection->handleup(pos, off);
			if (renderer.snode != NULL)
			{
				renderer.snode->handleclick(pos, off);
				if (renderer.ssection == NULL)
					renderer.ssection = renderer.findsection(pos, off);

				if (renderer.ssection != NULL && renderer.ssection->in_section(pos, off))
				{
					if (renderer.ssection->id != renderer.snode->contents->secid)
						renderer.snode->position.x = 26.8;
					else
					{
						if (renderer.ssection->limited_supply)
						{
							renderer.ssection->contents.add(renderer.snode->contents);
							lnode<node> *curr = renderer.nodelist.first;
							while (curr != NULL && curr->data.contents != renderer.snode->contents)
								curr = curr->next;
							renderer.snode->release();
							renderer.nodelist.rem(curr);
						}
						else
						{
							lnode<node> *curr = renderer.nodelist.first;
							while (curr != NULL && curr->data.contents != renderer.snode->contents)
								curr = curr->next;

							lnode<object*> *curro = renderer.objectlist.first;
							while (curro != NULL && curro->data != renderer.snode->contents)
								curro = curro->next;

							curro->data->release();
							delete curro->data;
							curro->data = NULL;
							renderer.objectlist.rem(curro);
							renderer.snode->release();
							renderer.nodelist.rem(curr);
						}
					}
				}
			}
			renderer.snode = NULL;
			renderer.ssection = NULL;
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			node *n = renderer.findnode(pos, off);
			if (renderer.snode != NULL && n != NULL)
				renderer.snode->connect(n);

			renderer.snode = NULL;
			n = NULL;
		}
	}
}

void motionfunc(int x, int y)
{
	float off, pos;
	pos = (float)x/9.0;
	off = (float)y/15.0;

	renderer.mouse_x = pos;
	renderer.mouse_y = off;

	if (renderer.mouse_b == GLUT_LEFT_BUTTON)
	{
		if (renderer.ssection != NULL && renderer.sobject == NULL)
			renderer.sobject = renderer.ssection->handledrag(pos, off);
		else if (renderer.snode != NULL && renderer.sobject == NULL)
			renderer.snode->handledrag(pos, off);
		else if (renderer.sobject == NULL)
		{
			renderer.snode = renderer.findnode(pos, off);
			if (renderer.snode != NULL)
				renderer.snode->handleclick(pos, off);
		}
	}
}

void keydownfunc(unsigned char key, int x, int y)
{
	keys.keydown(key);
}

void keyupfunc(unsigned char key, int x, int y)
{
	keys.keyup(key);
}

void release()
{
	renderer.release();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	atexit(release);

	glutInitWindowSize(1024, 640);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Luria Delbrueck");

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error %s\n", glewGetErrorString(err));
		exit(1);
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	if (GLEW_ARB_vertex_program)
		fprintf(stdout, "Status: ARB vertex programs available.\n");

	if (glewGetExtension("GL_ARB_fragment_program"))
		fprintf(stdout, "Status: ARB fragment programs available.\n");

	if (glewIsSupported("GL_VERSION_1_4  GL_ARB_point_sprite"))
		fprintf(stdout, "Status: ARB point sprites available.\n");

	glutReshapeFunc(reshape);
	glutDisplayFunc(displayfunc);
	glutIdleFunc(displayfunc);
	glutMotionFunc(motionfunc);
	glutMouseFunc(mousefunc);
	glutKeyboardFunc(keydownfunc);
	glutKeyboardUpFunc(keyupfunc);

	init();
	glutMainLoop();
}
