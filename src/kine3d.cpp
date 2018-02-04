#include "kine3d.h"
#include <cmath>


kine3d::kine3d()
{
	active_axis = 'z';
	active_joint = nullptr;
	create_joints(10, 10, 10, 20);
}

kine3d::~kine3d()
{
	std::for_each(joints.begin(), joints.end(), delete_ptr());
	joints.clear();
}

void kine3d::create_joints(float start_x, float start_y, float start_z, float dist)
{
	joint3* p0 = new joint3(start_x, start_y, start_z);
	joint3* p1 = new joint3(dist); // only the z coord needs to be set
	joint3* p2 = new joint3(dist);
	joint3* p3 = new joint3(dist);

	std::pair<joint3*, joint3*> bone1 = std::make_pair(p0, p1);
	std::pair<joint3*, joint3*> bone2 = std::make_pair(p1, p2);
	std::pair<joint3*, joint3*> bone3 = std::make_pair(p2, p3);

	p0->bone = new link3(bone1, dist);
	p1->bone = new link3(bone2, dist);
	p2->bone = new link3(bone3, dist);

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

	active_joint = joints.at(0);
}

matrix kine3d::rotation_matrix(float angle_x, float angle_y, float angle_z)
{
	matrix mX = rotation_matrix_x(angle_x);
	matrix mY = rotation_matrix_y(angle_y);
	matrix mZ = rotation_matrix_z(angle_z);

	matrix totalRotation = mX * mY * mZ;

	return totalRotation;
}

matrix kine3d::rotation_matrix_x(float angle)
{
	angle *= (PI / 180.0f); // to radians

	// rotation matrix over x axis: http://upload.wikimedia.org/math/5/1/4/5148f88bf9e6811e35615c08d2839793.png
	/*
	*		[ 1    0        0    ]
	*		[ 0  cos(θ)  -sin(θ) ]
	 *		[ 0  sin(θ)   cos(θ) ]
	 */

	matrix m(3, 3);
	m(1,1) = 1;
	m(1,2) = 0;
	m(1,3) = 0;

	m(2,1) = 0;
	m(2,2) = cosf(angle);
	m(2,3) = -sinf(angle);

	m(3,1) = 0;
	m(3,2) = sinf(angle);
	m(3,3) = cosf(angle);

	return m;
}

matrix kine3d::rotation_matrix_y(float angle)
{
	angle *= (PI / 180.0f); // to radians

	// rotation matrix over y axis: http://upload.wikimedia.org/math/5/1/4/5148f88bf9e6811e35615c08d2839793.png
	/*
	 *		[  cos(θ)  0  sin(θ) ]
	 *		[    0     1    0    ]
	 *		[ -sin(θ)  0  cos(θ) ]
	 */

	matrix m(3, 3);
	m(1,1) = cosf(angle);
	m(1,2) = 0;
	m(1,3) = sinf(angle);

	m(2,1) = 0;
	m(2,2) = 1;
	m(2,3) = 0;

	m(3,1) = -sinf(angle);
	m(3,2) = 0;
	m(3,3) = cosf(angle);

	return m;
}

matrix kine3d::rotation_matrix_z(float angle)
{
	angle *= (PI / 180.0f); // to radians

	// rotation matrix over z axis: http://upload.wikimedia.org/math/5/1/4/5148f88bf9e6811e35615c08d2839793.png
	/*
	 *		[ cos(θ)  -sin(θ)  0 ]
	 *		[ sin(θ)   cos(θ)  0 ]
	 *		[   0        0     1 ]
	 */

	matrix m(3, 3);
	m(1,1) = cosf(angle);
	m(1,2) = -sinf(angle);
	m(1,3) = 0;

	m(2,1) = sinf(angle);
	m(2,2) = cosf(angle);
	m(2,3) = 0;

	m(3,1) = 0;
	m(3,2) = 0;
	m(3,3) = 1;

	return m;
}


vec3 kine3d::convert_to_world(vec3* Pn_1, matrix& Sn_1, vec3* Tn, matrix& Rn, vec3* local)
{
	// Pn = P[n-1] + S[n-1](T[n] + R[n](0,0))

	// T[n] + R[n](0,0)
	vec3 f = *Tn + (Rn * *local).to_vec3();

	// P[n-1] + S[n-1](f)
	return *Pn_1 + (Sn_1 * f).to_vec3();
}

void kine3d::draw_world_axis()
{
	glLineWidth(5.0f);

	// x-axis (red)
	glColor3f(1,0,0);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(20.0f, 0.0f, 0.0f);
	glEnd();

	glPushMatrix();
		glTranslatef(20,0,0);
		glRotatef(90,0,1,0);
		glutSolidCone(1,2,15,10);
	glPopMatrix();

	// y-axis (blue)
	glColor3f(0,0,1);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 20.0f, 0.0f);
	glEnd();

	glPushMatrix();
		glTranslatef(0,20,0);
		glRotatef(90,-1,0,0);
		glutSolidCone(1,2,15,10);
	glPopMatrix();

	// z-axis (green)
	glColor3f(0,1,0);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 20.0f);
	glEnd();

	glPushMatrix();
		glTranslatef(0,0,20);
		glRotatef(90,0,0,1);
		glutSolidCone(1,2,15,10);
	glPopMatrix();
}

void kine3d::draw_vertex(vec3* v, uint radius, bool highlight)
{
	if (highlight)
			glColor3f(1,0,1);
		else
			glColor3f(1,1,1);

	glPushMatrix();
	glTranslatef(v->x, v->y, v->z);
	glutSolidSphere(1.0f, 15, 10);
	glPopMatrix();
}

void kine3d::draw_line(vec3* start, vec3* end, float thickness, ColorType color)
{
	switch(color)
	{
		case COLOR_RED:
			glColor3f(1,0,0);
		break;

		case COLOR_BLUE:
			glColor3f(0,0,1);
		break;

		case COLOR_GREEN:
			glColor3f(0,1,0);
		break;

		case COLOR_WHITE:
			glColor3f(1,1,1);
		break;

		case COLOR_BLACK:
		default:
			glColor3f(0,0,0);
		break;
	}

	glLineWidth(thickness);
	glBegin(GL_LINES);
	{
		glVertex3f(start->x, start->y, start->z);
		glVertex3f(end->x, end->y, end->z);
	}
	glEnd();
}

void kine3d::init(int w, int h)
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);

	// init a 3D glut configuration
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(40.0, (GLdouble)w/h, 0.0, 200.0);
	gluLookAt(0.0, 0.0, 100.0,    0.0, 0.0, 0.0,    0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-35.0f, -25.0f, 0.0f);
}

void kine3d::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_world_axis();

	matrix Sn_1(3,3);	// S[n-1]

	joint3* joint = joints[0];
	vec3 Pn_1(0,0,0); 	// P[n-1]
	vec3 Pn(0,0,0);		// P[n]

	vec3 zero(0,0,0);
	vec3 axisX(10.0f,0,0);
	vec3 axisY(0,10.0f,0);
	vec3 axisZ(0,0,10.0f);

	while (joint)
	{
		matrix Rn = rotation_matrix(joint->theta_x, joint->theta_y, joint->theta_z);

		// convert the position of the child to global coordinates
		Pn = convert_to_world(&Pn, Sn_1, joint->t, Rn, &zero);

		// get end-points of local axis lines (note that it is based on Pn_1)
		vec3 x = convert_to_world(&Pn_1, Sn_1, joint->t, Rn, &axisX);
		vec3 y = convert_to_world(&Pn_1, Sn_1, joint->t, Rn, &axisY);
		vec3 z = convert_to_world(&Pn_1, Sn_1, joint->t, Rn, &axisZ);

		// draw joint (pink if selected, white otherwise)
		draw_vertex(&Pn, 5, joint == active_joint);

		// draw link
		if (!(Pn_1.x == 0 && Pn_1.y == 0 && Pn_1.z == 0)) // prevent drawing from origin to first joint
			draw_line(&Pn_1, &Pn, 2, COLOR_WHITE);

		// draw local axes
		draw_line(&Pn, &x, 3, COLOR_RED);
		draw_line(&Pn, &y, 3, COLOR_BLUE);
		draw_line(&Pn, &z, 3, COLOR_GREEN);

		// setup next iteration
		Sn_1 = Sn_1 * Rn; 	// set to S[n], which is S[n-1] for the next iteration
		Pn_1 = Pn;			// set to P[n], which is P[n-1] for the next iteration

		joint = joint->child;
	}

	glutPostRedisplay();
}

void kine3d::prev_joint()
{
	if (active_joint)
		if (active_joint->parent)
			active_joint = active_joint->parent;
}

void kine3d::next_joint()
{
	if (active_joint)
		if (active_joint->child)
			active_joint = active_joint->child;
}

void kine3d::rotate_joint(float degrees)
{
	if (active_joint)
	{
		switch (active_axis)
		{
		case 'x':
			active_joint->rotate_x(degrees);
		break;

		case 'y':
			active_joint->rotate_y(degrees);
		break;

		case 'z':
		default:
			active_joint->rotate_z(degrees);
		break;
		}
	}
}

void kine3d::switch_rotation_axis(char axis)
{
	if (axis == 'x' || axis == 'y' || axis == 'z')
		active_axis = axis;
	else
		active_axis = 'z';
}
