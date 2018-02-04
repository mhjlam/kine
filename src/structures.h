#pragma once

#include <cmath>
#include <stack>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <gl/freeglut.h>

#include "constants.h"

struct vec2;
struct vec3;
struct joint2;
struct joint3;
struct link2;
struct link3;

class kinecontext
{
public:
	virtual void init(int w, int h) = 0;
	virtual void draw() = 0;

	virtual void prev_joint() = 0;
	virtual void next_joint() = 0;

	virtual void rotate_joint(float degrees) = 0;
	virtual void insert_point(float x, float y) = 0;
	virtual void switch_rotation_axis(char axis) = 0;
};


struct vec2
{
	float x, y;

	vec2(float x, float y) : x(x), y(y) {}

	vec2 operator+(const vec2& other)
	{
		return vec2((x + other.x), (y + other.y));
	}

	vec2 operator-(const vec2& other)
	{
		return vec2((x - other.x), (y - other.y));
	}

	// Euclidean distance
	uint dist(const vec2& other)
	{
		float xsqr = (other.x - x) * (other.x - x);
		float ysqr = (other.y - y) * (other.y - x);

		return (uint) sqrtf(xsqr + ysqr);
	}
};

struct vec3
{
	float x, y, z;

	vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	vec3 operator+(const vec3& other)
	{
		return vec3((x + other.x), (y + other.y), (z + other.z));
	}

	vec3 operator-(const vec3& other)
	{
		return vec3((x - other.x), (y - other.y), (z - other.z));
	}

	uint dist(const vec3& other)
	{
		float xsqr = (other.x - x) * (other.x - x);
		float ysqr = (other.y - y) * (other.y - x);
		float zsqr = (other.z - z) * (other.z - z);

		return (uint)sqrtf(xsqr + ysqr + zsqr);
	}
};


struct joint2
{
	vec2* t; 							// translation vector (position in parent's coordinates)
	float theta;						// angle of rotation in relation to parent
	joint2* parent;
	joint2* child; 						// only need one for now
	//std::vector<Joint2*> children;	// needed when joints are able to split
	link2* bone;						// associated link

	// lx = length of this joint to its parent (on the x axis)
	joint2(float lx) : t(new vec2(lx, 0)), theta(0), parent(nullptr), child(nullptr), bone(nullptr) {}
	joint2(float x, float y) : t(new vec2(x,y)), theta(0), parent(nullptr), child(nullptr), bone(nullptr) {}

	~joint2()
	{
		delete t;
		t = nullptr;
	}

	//void add_child(joint2* const child)
	//{
	//	children_.push_back(child);
	//}

	void rotate(float degrees)
	{
		theta += degrees;

		if (theta > 360 || theta < 0)
			theta = fmodf(theta, 360.0f);
	}
};

struct joint3
{
	vec3* t; 							// translation vector (position in parent's coordinates)
	float theta_x;						// angle of rotation in relation to parent (more degrees of freedom means rotation for x, y and z axis (pitch, yaw and roll))
	float theta_y;
	float theta_z;
	joint3* parent;						// parent joint
	joint3* child; 						// only need one for now
	//std::vector<Joint3*> children;	// needed when joints are able to split
	link3* bone;						// associated link

	// lx = length of this joint to its parent (on the x axis)
	joint3(float lx) : t(new vec3(lx, 0.0f, 0.0f)), theta_x(0), theta_y(0), theta_z(0), parent(nullptr), child(nullptr), bone(nullptr) {}
	joint3(float x, float y, float z) : t(new vec3(x, y, z)), theta_x(0), theta_y(0), theta_z(0), parent(nullptr), child(nullptr), bone(nullptr) {}

	~joint3()
	{
		delete t;
		t = nullptr;
	}

	//void addChild(joint3* const child)
	//{
	//	children.push_back(child);
	//}

	void rotate_x(float degrees)
	{
		theta_x += degrees;

		if (theta_x > 360 || theta_x < 0)
			theta_x = fmodf(theta_x, 360.0f);
	}

	void rotate_y(float degrees)
	{
		theta_y += degrees;

		if (theta_y > 360 || theta_y < 0)
			theta_y = fmodf(theta_y, 360.0f);
	}

	void rotate_z(float degrees)
	{
		theta_z += degrees;

		if (theta_z > 360 || theta_z < 0)
			theta_z = fmodf(theta_z, 360.0f);
	}
};

struct link2
{
	float length;
	vec2* center;
	std::pair<joint2*, joint2*> connection;
	std::vector<vec2*> attachments;

	link2(std::pair<joint2*, joint2*> connect, float length) : length(length), connection(connect)
	{
		// possible loss of precision here
		center = new vec2(connect.second->t->x/2, connect.second->t->y/2);
	}

	~link2()
	{
		std::for_each(attachments.begin(), attachments.end(), delete_ptr());
		attachments.clear();
	}

	void attach(vec2* const v)
	{
		attachments.push_back(v);
	}
};

struct link3
{
	float length;
	vec3* center;
	std::pair<joint3*, joint3*> connection;
	std::vector<vec3*> attachments;

	link3(std::pair<joint3*, joint3*> connect, float length) : length(length), connection(connect)
	{
		center = new vec3(connect.second->t->x / 2, connect.second->t->y / 2, connect.second->t->z / 2);
	}

	~link3()
	{
		std::for_each(attachments.begin(), attachments.end(), delete_ptr());
		attachments.clear();
	}

	void attach(vec3* const v)
	{
		attachments.push_back(v);
	}
};
