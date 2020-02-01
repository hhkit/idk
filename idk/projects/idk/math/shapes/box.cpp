#include "stdafx.h"
#include "box.h"
#include <math/matrix_transforms.inl>
#include <math/matrix_decomposition.inl>
#include <math/quaternion.inl>
#include <ds/zip.inl>
#include <math/vector.inl>

namespace idk
{
	mat3 box::axes() const
	{
		return quat_cast<mat3>(rotation);
	}
	aabb box::bounds() const
	{
		const vec3 hextents = half_extents();
		vec3 tmp;
		const auto axis = axes();
		for (int i = 0; i < 3; ++i)
		{
			auto col_vec = axis[i];
			for (auto& elem : col_vec)
				elem = abs(elem);
			tmp += col_vec * hextents[i];
		}

		return aabb{ center - tmp, center + tmp };
	}
	vec3 box::half_extents() const
	{
		return extents / 2;
	}
	array<vec3, 8> box::points() const
	{
		const auto hextents = half_extents();
		const auto mat = axes();

		const vec3 hm[3] = {
			hextents[0] * mat[0],
			hextents[1] * mat[1],
			hextents[2] * mat[2],
		};

		return array<vec3, 8>{
			center - hm[0] - hm[1] - hm[2],
			center - hm[0] - hm[1] + hm[2],
			center - hm[0] + hm[1] - hm[2],
			center - hm[0] + hm[1] + hm[2],
			center + hm[0] - hm[1] - hm[2],
			center + hm[0] - hm[1] + hm[2],
			center + hm[0] + hm[1] - hm[2],
			center + hm[0] + hm[1] + hm[2],
		};
	}
	box& box::operator*=(const mat4& transform)
	{
		auto this_comp = matrix_decomposition<float>{};
		this_comp.scale = extents;
		this_comp.rotation = rotation;
		this_comp.position = center;
		auto gen = this_comp.recompose();
		//auto gen = mat4{ axes() * scale(extents) };
		//gen[3] = vec4{ center, 1 };
		auto tfm = transform * gen;
		auto decomp = decompose(tfm);
		extents  = decomp.scale;
		rotation = decomp.rotation;
		center   = decomp.position;
		return *this;
	}
	box box::operator*(const mat4& matrix) const
	{
		auto copy = *this;
		return copy *= matrix;
	}
}