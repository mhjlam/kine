#pragma once

#include <memory>
#include "constants.h"
#include "structures.h"

class kinecontext;
class kine2d;
class kine3d;

class stickler
{
private:
	static vec2 mpos;

	static std::shared_ptr<kine2d> kine_2d;
	static std::shared_ptr<kine3d> kine_3d;

	static kinecontext* current_context;

	static uint win_handle;
	static uint window_width;
	static uint window_height;

	static bool initialized;
	static bool three_d;
	static bool mouse_down;

public:
	static void init(int argc, char* argv[]);
	static void run(uint w, uint h);

	static void display();
	static void reshape(int w, int h);
	static void keyboard(int key, int x, int y);
	static void special(unsigned char c, int x, int y);
	static void mouse_click(int button, int state, int x, int y);
	static void mouse_motion(int x, int y);
	static void mouse_passive(int x, int y);
	static void menu_select(int option);

	static void make_window(uint w, uint h, kinecontext* const context);
};
