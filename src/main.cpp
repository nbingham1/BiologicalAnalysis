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
	glEnable(GL_DEPTH_TEST);
   	glShadeModel(GL_SMOOTH);

   	GLfloat mat_shininess[] = { 100.0 };
   	GLfloat light_position[] = { 1.0, 0.0, 1.0, 0.0 };
   	glShadeModel (GL_SMOOTH);

   	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  	glEnable(GL_LIGHT0);
  	glEnable(GL_COLOR_MATERIAL);
  	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

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
	glOrtho(-1.1*(float)w/(float)h, 1.1*(float)w/(float)h, -1.1, 1.1, 2.0, 9999);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mousefunc(int button, int state, int x, int y)
{
	float off, pos;
	pos = (float)x/9.0;
	off = (float)y/15.0;

	float upx = 2.2*float(x)/(float)renderer.display.scrn_height - 1.1*(float)renderer.display.scrn_width/(float)renderer.display.scrn_height;
	float upy = 2.2*float(y)/renderer.display.scrn_height - 1.1;

	renderer.mouse_x = pos;
	renderer.mouse_y = off;
	renderer.mouse_b = button;

	if (state == GLUT_DOWN)
	{
		lnode<checkbox> *curr = renderer.boxlist.first;
		while (curr != NULL)
		{
			if (curr->data.in_box(vec(pos, off, 0.0)))
				break;
			curr = curr->next;
		}

		renderer.fitgraph.handleclick(upx, upy);
		renderer.errorgraph.handleclick(upx, upy);
	}
	else if (state == GLUT_UP)
	{
		renderer.fitgraph.selected = false;
		renderer.errorgraph.selected = false;
	}
}

void motionfunc(int x, int y)
{
	float off, pos;
	pos = (float)x/9.0;
	off = (float)y/15.0;

	float upx = 2.2*float(x)/(float)renderer.display.scrn_height - 1.1*(float)renderer.display.scrn_width/(float)renderer.display.scrn_height;
	float upy = 2.2*float(y)/renderer.display.scrn_height - 1.1;

	renderer.mouse_x = pos;
	renderer.mouse_y = off;

	if (renderer.mouse_b == GLUT_LEFT_BUTTON)
	{
		renderer.fitgraph.handledrag(upx, upy);
		renderer.errorgraph.handledrag(upx, upy);
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

	glutInitWindowSize(1400, 550);
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
