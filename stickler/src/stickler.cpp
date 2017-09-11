#include "stickler.h"
#include <GL/freeglut.h>
#include "kine2d.h"
#include "kine3d.h"

vec2 stickler::mpos = vec2(0, 0);
std::shared_ptr<kine2d> stickler::kine_2d;
std::shared_ptr<kine3d> stickler::kine_3d;
kinecontext* stickler::current_context;
uint stickler::window_width = 0;
uint stickler::window_height = 0;
uint stickler::win_handle = 0;
bool stickler::initialized = false;
bool stickler::three_d = false;
bool stickler::mouse_down = false;


void stickler::init(int argc, char* argv[])
{
	glutInit(&argc, argv);
	initialized = true;
}

void stickler::run(uint w, uint h)
{
	if (!initialized)
		return;

	kine_2d.reset(new kine2d());
	kine_3d.reset(new kine3d());

	win_handle = 0;

	make_window(w, h, kine_2d.get());

	// start loop
	glutMainLoop();
}

void stickler::display()
{
	// draw procedures
	if (current_context)
		current_context->draw();

	glutSwapBuffers();
}

void stickler::reshape(int w, int h)
{
	window_width = w;
	window_height = h;

	glViewport(0, 0, w, h);

	if (current_context)
		current_context->init(w, h);
}

void stickler::special(unsigned char c, int x, int y)
{
	if (three_d)
		if (c == 'x' || c == 'y' || c == 'z')
			current_context->switch_rotation_axis(c);

	if (c == 27)
		exit(0);
}

void stickler::keyboard(int key, int x, int y)
{
	if (!current_context)
		return;

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

void stickler::mouse_click(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
		mouse_down = true;

	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
		mouse_down = false;
}

void stickler::mouse_motion(int x, int y)
{
	if (three_d && mouse_down)
		glRotated(1.0f, y - mpos.y, x - mpos.x, 0.0);

	mpos.x = (float)x;
	mpos.y = (float)y;
}

void stickler::mouse_passive(int x, int y)
{
	mpos.x = (float)x;
	mpos.y = (float)y;
}

void stickler::menu_select(int option)
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

		case MENU_HAND3D:
			//setContext(kinHand_.get());
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

void stickler::make_window(uint w, uint h, kinecontext* const context)
{
	if (win_handle != 0)
		glutDestroyWindow(win_handle);

	glutInitWindowSize(w, h);
	glutInitWindowPosition(200, 200);

	window_width = w;
	window_height = h;
	current_context = context;
	current_context->init(w, h);

	win_handle = glutCreateWindow("stickler");

	// call-backs
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(special);
	glutSpecialFunc(keyboard);
	glutMouseFunc(mouse_click);
	glutMotionFunc(mouse_motion);
	glutPassiveMotionFunc(mouse_passive);

	// menu
	glutCreateMenu(menu_select);
	glutAddMenuEntry("Switch to 2D", MENU_KIN2D);
	glutAddMenuEntry("Switch to 3D", MENU_KIN3D);
	//glutAddMenuEntry("Switch to Kinematics Hand",	MENU_HAND3D);
	glutAddMenuEntry("Add vertex (2D only)", MENU_ADD_PT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
