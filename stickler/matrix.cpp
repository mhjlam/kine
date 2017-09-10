#include "matrix.h"

matrix::matrix() : nrows(0), ncols(0), nil(0.0f)
{
	reset();
}

matrix::matrix(uint nr, uint nc) : nrows(nr), ncols(nc), nil(0.0)
{
	reset();
}

matrix::~matrix(void)
{
	m.clear();
}

void matrix::reset()
{
	for (uint i = 0; i < ncols * nrows; ++i)
	{
		if (i == m.size()) m.push_back(0.0f);
		else m.at(i) = 0.0f;
	}

	// diagonal
	for (uint i = 0; i < m.size(); i += ncols + 1)
		m.at(i) = 1.0f;
}

matrix matrix::product(matrix mat)
{
	matrix result = matrix(num_rows(), mat.num_cols());

	if (num_cols() == mat.num_rows())
	{
		for (uint i = 1; i <= num_rows(); ++i) // for each of our rows
		{
			for (uint k = 1; k <= mat.num_cols(); ++k) // for each of their columns
			{
				float sum = 0;

				for (uint j = 1; j <= num_cols(); ++j) // for each of our columns
				{
					sum += get(i, j) * mat(j, k);
				}

				result(i, k) = sum;
			}
		}
	}
	else
	{
		std::cout << "Warning: Matrix dimensions not compatible" << std::endl;
	}

	return result;
}

float& matrix::get(uint r, uint c)
{
	nil = 0.0f;
	uint i = ((--r * num_cols()) + c) - 1;
	if (i >= m.size()) return nil;
	return m.at(i);
}

vec2 matrix::to_vec2()
{
	// floats are manually casted to uints (and are thus rounded)
	return vec2(int(get(1,1)), int(get(2,1)));
}

vec3 matrix::to_vec3()
{
	return vec3(get(1,1), get(2,1), get(3,1));
}

float& matrix::operator()(uint iRow, uint iColumn)
{
	return get(iRow, iColumn);
}

void matrix::operator*(float scalar)
{
	for (uint i = 0; i < m.size(); ++i)
	{	m.at(i) *= scalar;			}
}

matrix matrix::operator*(matrix matrix)
{
	return product(matrix);
}

matrix matrix::operator*(vec2 vec2)
{
	return product(to_matrix(vec2));
}

matrix matrix::operator*(vec3 vec3)
{
	return product(to_matrix(vec3));
}

void matrix::print()
{
	for (uint i = 1; i <= num_rows(); ++i)
	{
		std::cout << "[";

		for (uint j = 1; j <= num_cols(); ++j)
		{
			std::cout << " " << get(i,j) << " ";
		}

		std::cout << "]" << std::endl;
	}
}

// static functions
matrix matrix::to_matrix(const vec2& vec2)
{
	matrix m2(2,1);
	m2(1,1) = float(vec2.x);
	m2(2,1) = float(vec2.y);
	return m2;
}

matrix matrix::to_matrix(const vec3& vec3)
{
	matrix m3(3,1);
	m3(1,1) = vec3.x;
	m3(2,1) = vec3.y;
	m3(3,1) = vec3.z;
	return m3;
}
