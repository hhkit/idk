#include "stdafx.h"
#include "Matrix.h"

namespace idk::math
{
	template struct matrix<float, 3, 3>;
	template struct matrix<float, 4, 4>;
	template vector<float, 3> operator*(const matrix<float, 3, 3>&, const vector<float, 3>&);
	template vector<float, 4> operator*(const matrix<float, 4, 4>&, const vector<float, 4>&);
	template matrix<float, 3, 3> operator*(const matrix<float, 3, 3> & lhs, const matrix<float, 3, 3> & rhs);
	template matrix<float, 4, 4> operator*(const matrix<float, 4, 4> & lhs, const matrix<float, 4, 4> & rhs);
}