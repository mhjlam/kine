#pragma once

// typedefs
typedef unsigned int uint;

// constants
static const float PI = 3.1415926f;
static const float ROTATION_ANGLE = 10.0f;
static const uint CIRCLE_PRECISION = 100;

// enums
enum MenuOption
{
	MENU_KIN2D,
	MENU_KIN3D,
	MENU_ADD_PT
};

enum ColorType
{
	COLOR_BLACK,
	COLOR_WHITE,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_BLUE
};

// functor
struct delete_ptr
{
	template <typename T>
	void operator()(T& p)
	{
		delete p;
		p = nullptr;
	}
};
