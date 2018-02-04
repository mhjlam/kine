#pragma once
#include <vector>
#include <cmath>
#include "structures.h"

class matrix
{
private:
	std::vector<float> m;
	uint nrows;
	uint ncols;
	mutable float nil; // returned in dGet() when requested field doesn't exist

public:
	matrix();
	matrix(uint nr, uint nc);
	virtual ~matrix();

	void reset();

	matrix product(matrix matrix);

	void operator*(float scalar);
	matrix operator*(matrix matrix);
	matrix operator*(vec2 vec2);
	matrix operator*(vec3 vec3);

	float& get(uint iRow, uint iColumn);
	float& operator()(uint uRow, uint uColumn);

	vec2 to_vec2();
	vec3 to_vec3();

	inline uint num_rows() const { return nrows; }
	inline uint num_cols() const { return ncols; }

	static matrix to_matrix(const vec2& vec2);
	static matrix to_matrix(const vec3& vec3);

	void print();
};
