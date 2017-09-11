#pragma once

#include "structures.h"
#include "matrix.h"

class kine3d : public kinecontext
{
private:
	char active_axis;				// axis to rotate about
	std::vector<joint3*> joints;	// joints of the model
	joint3* active_joint;			// selected joint

private:
	void create_joints(float start_x, float start_y, float start_z, float dist);

	matrix rotation_matrix(float angle_x, float angle_y, float angle_z);
	matrix rotation_matrix_x(float angle_x);
	matrix rotation_matrix_y(float angle_y);
	matrix rotation_matrix_z(float angle_z);

	vec3 convert_to_world(vec3* Pn_1, matrix& Sn_1, vec3* Tn, matrix& Rn, vec3* local);

	void draw_world_axis();
	void draw_vertex(vec3* v, uint radius, bool highlight = false);
	void draw_line(vec3* start, vec3* end, float thickness = 1.0f, ColorType color = COLOR_BLACK);

public:
	kine3d();
	virtual ~kine3d();

	void init(int w, int h);
	void draw();

	void prev_joint();
	void next_joint();

	void rotate_joint(float degrees);
	void insert_point(float x, float y) {};
	void switch_rotation_axis(char axis);
};
