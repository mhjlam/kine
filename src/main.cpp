#include "kine2d.h"
#include "kine3d.h"
#include "constants.h"
#include "structures.h"

#include <memory>
#include <GL/freeglut.h>

static vec2 mpos = vec2(0, 0);
static std::shared_ptr<kine2d> kine_2d;
static std::shared_ptr<kine3d> kine_3d;
static kinecontext* current_context;
static uint window_width = 0;
static uint window_height = 0;
static uint win_handle = 0;
static bool initialized = false;
static bool three_d = false;
static bool mouse_down = false;

void display()
{
	// draw procedures
	if (current_context)
		current_context->draw();

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	window_width = w;
	window_height = h;

	glViewport(0, 0, w, h);

	if (current_context)
		current_context->init(w, h);
}

void special(unsigned char c, int x, int y)
{
	if (three_d && (c == 'x' || c == 'y' || c == 'z'))
		current_context->switch_rotation_axis(c);

	if (c == 27) exit(0);
}

void keyboard(int key, int x, int y)
{
	if (!current_context) return;

	switch (key)
	{
		case GLUT_KEY_UP:
			current_context->rotate_joint(ROTATION_ANGLE);
			break;

		case GLUT_KEY_DOWN:
			current_context->rotate_joint(-ROTATION_ANGLE);
			break;

		case GLUT_KEY_LEFT:
			current_context->prev_joint();
			break;

		case GLUT_KEY_RIGHT:
			current_context->next_joint();
			break;
	}

	glutPostRedisplay();
}

void mouse_click(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
		mouse_down = true;

	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
		mouse_down = false;
}

void mouse_motion(int x, int y)
{
	if (three_d && mouse_down)
		glRotated(1.0f, y - mpos.y, x - mpos.x, 0.0);

	mpos.x = (float)x;
	mpos.y = (float)y;
}

void mouse_passive(int x, int y)
{
	mpos.x = (float)x;
	mpos.y = (float)y;
}



void make_window(uint w, uint h, kinecontext* const context)
{
	if (win_handle != 0) glutDestroyWindow(win_handle);

	glutInitWindowSize(w, h);
	glutInitWindowPosition(200, 200);

	window_width = w;
	window_height = h;
	current_context = context;
	current_context->init(w, h);

	win_handle = glutCreateWindow("spline");

	// call-backs
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(special);
	glutSpecialFunc(keyboard);
	glutMouseFunc(mouse_click);
	glutMotionFunc(mouse_motion);
	glutPassiveMotionFunc(mouse_passive);

	// menu
	void menu_select(int option);
	glutCreateMenu(menu_select);
	glutAddMenuEntry("Switch to 2D", MENU_KIN2D);
	glutAddMenuEntry("Switch to 3D", MENU_KIN3D);
	glutAddMenuEntry("Add vertex", MENU_ADD_PT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void menu_select(int option)
{
	switch (option)
	{
		case MENU_KIN2D:
			make_window(window_width, window_height, kine_2d.get());
			three_d = false;
			break;

		case MENU_KIN3D:
			make_window(window_width, window_height, kine_3d.get());
			three_d = true;
			break;

		case MENU_ADD_PT:
			if (!three_d)
			{
				// convert mousepos to 'world' pos
				float x = mpos.x - 20.0f;
				float y = -mpos.y + window_height - 20.0f;
				current_context->insert_point(x, y);
			}

			break;
	}
}



int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	initialized = true;

	kine_2d.reset(new kine2d());
	kine_3d.reset(new kine3d());

	win_handle = 0;

	make_window(1280, 720, kine_2d.get());

	// start loop
	glutMainLoop();
	return 0;
}
