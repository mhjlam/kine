#include "kine2d.h"
#include <cmath>

kine2d::kine2d()
{
	active_joint = NULL;

	create_joints(150, 150, 100);
}

kine2d::~kine2d()
{
	std::for_each(joints.begin(), joints.end(), delete_ptr());
	joints.clear();
}

void kine2d::create_joints(float start_x, float start_y, float dist)
{
	// create some joints and setup their data accordingly

	joint2* p0 = new joint2(start_x, start_y);
	joint2* p1 = new joint2(dist); // only the x coord needs to be set, as the next joint lies on the x-axis of the parent
	joint2* p2 = new joint2(dist);
	joint2* p3 = new joint2(dist);

	std::pair<joint2*, joint2*> bone1 = std::make_pair(p0, p1);
	std::pair<joint2*, joint2*> bone2 = std::make_pair(p1, p2);
	std::pair<joint2*, joint2*> bone3 = std::make_pair(p2, p3);

	p0->bone = new link2(bone1, dist);
	p1->bone = new link2(bone2, dist);
	p2->bone = new link2(bone3, dist);

	p1->parent = p0;
	p2->parent = p1;
	p3->parent = p2;

	p0->child = p1;
	p1->child = p2;
	p2->child = p3;

	joints.push_back(p0);
	joints.push_back(p1);
	joints.push_back(p2);
	joints.push_back(p3);

	active_joint = joints[0];
}

matrix kine2d::rotation_matrix(float angle)
{
	angle *= (PI / 180.0f); // to radians

	/*	rotation matrix (non-homogeneous):
	 *
	 *		[ cos(angle) -sin(angle) ]
	 *		[ sin(angle)  cos(angle) ]
	 */

	matrix m(2,2);
	m(1,1) = cosf(angle);
	m(1,2) = -sinf(angle);

	m(2,1) = sinf(angle);
	m(2,2) = cosf(angle);

	return m;
}

vec2 kine2d::convert_to_world(vec2* Pn_1, matrix& Sn_1, vec2* Tn, matrix& Rn, vec2* localCoordinates)
{
	/*
	 * 	general equation:
	 * 	P[n] = P[n-1] + S[n-1](T[n] + R[n](x,y))
	 *
	 * 	where:
	 * 	P[n]	position in global coordinates
	 * 	P[n-1]	parent's position in global coordinates
	 * 	S[n-1]	parent's total rotation matrix
	 * 	T[n]	joint's position relative to parent
	 * 	R[n]	rotation matrix
	 * 	(x,y)	position in local coordinates ( (0,0) for the joint itself)
	 *
	 */

	// T[n] + R[n](x,y)
	vec2 f = (*Tn) + (Rn * (*localCoordinates)).to_vec2();

	// P[n-1] + S[n-1](f)
	vec2 g = (*Pn_1) + (Sn_1 * f).to_vec2();

	return g;
}

void kine2d::draw_world_axis()
{
	glLineWidth(3.0f);

	// x-axis (red)
	glColor3f(1,0,0);
	glBegin(GL_LINES);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(500.0f, 0.0f);
	glEnd();
	glBegin(GL_QUADS); // arrow
		glVertex2f(495.0f, 5.0f);
		glVertex2f(505.0f, 0.0f);
		glVertex2f(505.0f, 0.0f);
		glVertex2f(495.0f, -5.0f);
	glEnd();

	// y-axis (blue)
	glColor3f(0,0,1);
	glBegin(GL_LINES);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(0.0f, 500.0f);
	glEnd();
	glBegin(GL_QUADS); // arrow
		glVertex2f(0.0f, 505.0f);
		glVertex2f(0.0f, 505.0f);
		glVertex2f(5.0f, 495.0f);
		glVertex2f(-5.0f, 495.0f);
	glEnd();
}

// visualize joints with small circles
void kine2d::draw_vertex(vec2* v, uint radius, bool highlight)
{
	if (highlight)
		glColor3f(1,0,1);
	else
		glColor3f(0,0,0);

	glBegin(GL_POLYGON);
	{
		for (uint i = 0; i < CIRCLE_PRECISION; i++)
		{
			float angle = i*2*PI/CIRCLE_PRECISION;
			glVertex2f(cos(angle)*radius + v->x, sin(angle)*radius + v->y);
		}
	}
	glEnd();
}

void kine2d::draw_line(vec2* start, vec2* end, float thickness, ColorType color)
{
	switch(color)
	{
	case COLOR_RED:
		glColor3f(1,0,0);
	break;

	case COLOR_BLUE:
		glColor3f(0,0,1);
	break;

	case COLOR_BLACK:
	default:
		glColor3f(0,0,0);
	break;
	}

	glLineWidth(thickness);
	glBegin(GL_LINES);
	{
		glVertex2f(start->x, start->y);
		glVertex2f(end->x, end->y);
	}
	glEnd();
}

void kine2d::init(int w, int h)
{
	// init a 2d glut configuration
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glClearColor(0.85f, 0.85f, 0.80f, 1.0f);

	gluOrtho2D(-20.0, w, -20.0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void kine2d::draw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	draw_world_axis();

	// total rotation matrix
	matrix Sn_1(2,2);

	joint2* joint = joints[0];
	vec2 Pn_1(0,0); // P[n-1]
	vec2 Pn(0,0);	// P[n]

	vec2 zero(0,0);
	vec2 axisX(50,0);
	vec2 axisY(0,50);

	// list of bone positions in global coordinates
	std::vector<std::pair<vec2, link2*> > globalPosLinks;

	while (joint)
	{
		// make a rotation matrix
		matrix Rn = rotation_matrix(joint->theta);

		// convert the position of the child to global coordinates
		Pn = convert_to_world(&Pn, Sn_1, joint->t, Rn, &zero);

		// get end-points of local axis lines (note that it is based on Pn_1)
		vec2 x = convert_to_world(&Pn_1, Sn_1, joint->t, Rn, &axisX);
		vec2 y = convert_to_world(&Pn_1, Sn_1, joint->t, Rn, &axisY);

		// draw joint (pink if selected, black otherwise)
		draw_vertex(&Pn, 5, joint == active_joint);

		// draw link
		if (!(Pn_1.x == 0 && Pn_1.y == 0)) // prevent drawing from origin to first joint
			draw_line(&Pn_1, &Pn, 2, COLOR_BLACK);

		// draw vertices
		link2* bone = joint->bone;

		if (bone)
		{
			for (uint i = 0; i < bone->attachments.size(); ++i)
			{
				vec2 boneGlobalPos = convert_to_world(&Pn_1, Sn_1, joint->t, Rn, bone->attachments.at(i));
				draw_vertex(&boneGlobalPos, 2, false);
			}

			if (!dangling_points.empty())
			{
				// convert the bone's center position into a global position
				vec2 boneCenterGlobalPos = convert_to_world(&Pn_1, Sn_1, joint->t, Rn, bone->center);
				globalPosLinks.push_back(std::make_pair(boneCenterGlobalPos, bone));
			}
		}

		// draw local axes
		draw_line(&Pn, &x, 3, COLOR_RED);
		draw_line(&Pn, &y, 3, COLOR_BLUE);

		// setup next iteration
		Sn_1 = Sn_1 * Rn; 	// set to S[n], which is S[n-1] for the next iteration
		Pn_1 = Pn;			// set to P[n], which is P[n-1] for the next iteration

		joint = joint->child;
	}

	// attach the added point(s) to the closest bone
	for (uint i = 0; i < dangling_points.size(); ++i)
	{
		vec2* pointToAttach = dangling_points.top();
		vec2 closestCenter(999, 999);
		link2* associatedBone = NULL;

		// iterate over the all bones (in global coordinates)
		for (uint j = 0; j < globalPosLinks.size(); ++j)
		{
			std::pair<vec2, link2*>& pair = globalPosLinks.at(j);

			// compare distances
			if (pointToAttach->dist(pair.first) < pointToAttach->dist(closestCenter))
			{
				closestCenter = pair.first;
				associatedBone = pair.second;
			}
		}

		// convert from global to local
		pointToAttach->x -= closestCenter.x;
		pointToAttach->y -= closestCenter.y;

		// pointToAttach is now based on closestCenter, meaning it is effectively already in local space
		// we just need to correct the offset to compensate for taking the center of the bone
		pointToAttach->x += associatedBone->center->x;
		pointToAttach->y += associatedBone->center->y;

		associatedBone->attach(pointToAttach);
		dangling_points.pop();
	}

	glutPostRedisplay();
}

void kine2d::prev_joint()
{
	if (active_joint == joints[0])
		return;

	if (active_joint)
		if (active_joint->parent)
			active_joint = active_joint->parent;
}

void kine2d::next_joint()
{
	if (active_joint)
		if (active_joint->child)
			active_joint = active_joint->child;
}

void kine2d::rotate_joint(float degrees)
{
	if (active_joint)
		active_joint->rotate(degrees);
}

void kine2d::insert_point(float x, float y)
{
	dangling_points.push(new vec2(x,y)); // vec2 ptr will be deleted by the bone it gets attached to
}
