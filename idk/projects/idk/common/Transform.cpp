#include "stdafx.h"
#include "Transform.h"
#include <core/GameObject.inl>
#include <math/matrix_transforms.inl>
#include <math/matrix_decomposition.inl>
#include <math/quaternion.inl>

namespace idk
{
	void Transform::SetParent(Handle<class GameObject> new_parent, bool preserve_global)
	{
		if (preserve_global)
		{
			auto curr_global = GlobalMatrix();
			const auto new_local = [&]() {
				if (new_parent)
					return curr_global * new_parent->Transform()->GlobalMatrix().inverse();
				else
					return curr_global;
			}();

			const auto decomp = decompose(new_local);
			position = decomp.position;
			rotation = decomp.rotation;
			scale = decomp.scale;
		}
		parent = new_parent;
	}
	vec3 Transform::Forward() const
	{
		return -vec3{ GlobalMatrix()[2].get_normalized() };
	}

	vec3 Transform::Right() const
	{
		return vec3{ GlobalMatrix()[0].get_normalized() };
	}
	
	vec3 Transform::Up() const
	{
		return vec3{ GlobalMatrix()[1].get_normalized() };
	}

	mat4 Transform::LocalMatrix() const
	{
		auto rotation_mtx = quat_cast<mat4>(rotation);
		return translate(prescale(rotation_mtx, vec4{ scale, 1 }), position);
	}

	mat4 Transform::GlobalMatrix() const
	{
		if (parent)
			return parent->Transform()->GlobalMatrix() * LocalMatrix();
		else
			return LocalMatrix();
	}

	void Transform::LocalMatrix(const mat4& m)
	{
		const auto decomp = decompose(m);
		position = decomp.position;
		rotation = decomp.rotation;
		scale    = decomp.scale;
	}

	void Transform::GlobalMatrix(const mat4& m)
	{
		if (!parent)
		{
			const auto decomp = decompose(m);
			position = decomp.position;
			rotation = decomp.rotation;
			scale    = decomp.scale;
		}
		else
		{
			const auto decomp = decompose(parent->Transform()->GlobalMatrix().inverse() * m);
			position = decomp.position;
			rotation = decomp.rotation;
			scale    = decomp.scale;
		}
	}

	vec3 Transform::GlobalPosition() const
	{
		if (!parent)
			return position;
		else
			return GlobalMatrix()[3].xyz;
	}

	quat Transform::GlobalRotation() const
	{
		if (!parent)
			return rotation;
		else
			return decompose(GlobalMatrix()).rotation;
	}

	vec3 Transform::GlobalScale() const
	{
		if (!parent)
			return scale;
		else
		{
			auto mat = GlobalMatrix();
			auto det = mat.determinant();
			return vec3(det < -epsilon ? -mat[0].length() : mat[0].length(), mat[1].length(), mat[2].length());
		}
	}

	unsigned Transform::Depth() const
	{
		return parent ? parent->Transform()->Depth() + 1 : 0;
	}

	void Transform::GlobalPosition(vec3 pos)
	{
		if (!parent)
			position = pos;
		else
		{
			const auto curr_global = GlobalMatrix();
			auto decomp = decompose(GlobalMatrix());
			decomp.position = pos;
			const auto new_global = decomp.recompose();
			const auto parent_inv = parent->Transform()->GlobalMatrix().inverse();
			position = decompose(parent_inv * new_global).position;
		}
	}

	void Transform::GlobalRotation(quat rot)
	{
		if (!parent)
			rotation = rot;
		else
		{
			const auto curr_global = GlobalMatrix();
			auto decomp = decompose(GlobalMatrix());
			decomp.rotation = rot;
			const auto new_global = decomp.recompose();
			const auto parent_inv = parent->Transform()->GlobalMatrix().inverse();
			rotation = decompose(parent_inv * new_global).rotation;
		}
	}

	void Transform::GlobalScale(vec3 scl)
	{
		if (!parent)
			scale = scl;
		else
		{
			const auto curr_global = GlobalMatrix();
			auto decomp = decompose(GlobalMatrix());
			decomp.scale = scl;
			const auto new_global = decomp.recompose();

			const auto parent_inv = parent->Transform()->GlobalMatrix().inverse();
			position = decompose(parent_inv * new_global).scale;
		}
	}

	void Transform::CleanRotation()
	{
		rotation.normalize();
	}
}