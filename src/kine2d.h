#pragma once

#include "structures.h"
#include "matrix.h"

class kine2d : public kinecontext
{
private:
	std::stack<vec2*> dangling_points; // points that have yet to be attached to a bone
	std::vector<joint2*> joints;
	joint2* active_joint;

private:
	void create_joints(float start_x, float start_y, float dist);

	matrix rotation_matrix(float rotation);

	vec2 convert_to_world(vec2* Pn_1, matrix& Sn_1, vec2* Tn, matrix& Rn, vec2* localCoordinates);

	void draw_world_axis();
	void draw_vertex(vec2* v, uint radius, bool highlight = false);
	void draw_line(vec2* start, vec2* end, float thickness = 1.0f, ColorType color = COLOR_BLACK);

public:
	kine2d();
	~kine2d();

	void init(int w, int h);
	void draw();

	void prev_joint();
	void next_joint();

	void rotate_joint(float degrees);
	void insert_point(float x, float y);
	void switch_rotation_axis(char axis) {};
};
