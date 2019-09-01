#include "stdafx.h"
#include "Matrix.h"

namespace idk
{
	template struct tmat<float, 3, 3>;
	template struct tmat<float, 4, 4>;
	template tvec<float, 3> operator*(const tmat<float, 3, 3>&, const tvec<float, 3>&);
	template tvec<float, 4> operator*(const tmat<float, 4, 4>&, const tvec<float, 4>&);
	template tmat<float, 3, 3> operator*(const tmat<float, 3, 3> & lhs, const tmat<float, 3, 3> & rhs);
	template tmat<float, 4, 4> operator*(const tmat<float, 4, 4> & lhs, const tmat<float, 4, 4> & rhs);
}