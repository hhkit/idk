#include "stdafx.h"
#include "Transform.h"
#include <core/GameObject.h>
#include <math/matrix_transforms.h>
#include <math/matrix_decomposition.h>

namespace idk
{
	void Transform::SetParent(Handle<class GameObject> new_parent, bool preserve_global)
	{
		if (preserve_global)
		{
			auto curr_global = GlobalMatrix();
			auto new_local = new_parent->Transform()->GlobalMatrix().inverse() * curr_global;
			auto decomp = decompose(new_local);
			position = decomp.position;
			rotation = decomp.rotation;
			scale = decomp.scale;
		}
		parent = new_parent;
	}
	vec3 Transform::Forward() const
	{
		return GlobalMatrix()[2].get_normalized();
	}

	vec3 Transform::Right() const
	{
		return GlobalMatrix()[0].get_normalized();
	}
	
	vec3 Transform::Up() const
	{
		return GlobalMatrix()[1].get_normalized();
	}

	mat4 Transform::LocalMatrix() const
	{
		return translate(position) * mat4 { quat_cast<mat3>(rotation)* idk::scale(scale) };
	}

	mat4 Transform::GlobalMatrix() const
	{
		if (parent)
			return LocalMatrix() * parent->Transform()->GlobalMatrix();
		else
			return LocalMatrix();
	}

	void Transform::LocalMatrix(const mat4& m)
	{
		auto decomp = decompose(m);
		position = decomp.position;
		rotation = decomp.rotation;
		scale    = decomp.scale;
	}

	void Transform::GlobalMatrix(const mat4& m)
	{
		if (!parent)
		{
			auto decomp = decompose(m);
			position = decomp.position;
			rotation = decomp.rotation;
			scale    = decomp.scale;
		}
		else
		{
			auto decomp = decompose(parent->Transform()->GlobalMatrix().inverse() * m);
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
			auto curr_global = GlobalMatrix();
			auto decomp = decompose(GlobalMatrix());
			decomp.position = pos;
			auto new_global = decomp.recompose();
			
			auto parent_inv = parent->Transform()->GlobalMatrix().inverse();

			position = decompose(parent_inv * new_global).position;
		}
	}

	void Transform::GlobalRotation(quat rot)
	{
		if (!parent)
			rotation = rot;
		else
		{
			auto curr_global = GlobalMatrix();
			auto decomp = decompose(GlobalMatrix());
			decomp.rotation = rot;
			auto new_global = decomp.recompose();

			auto parent_inv = parent->Transform()->GlobalMatrix().inverse();

			position = decompose(parent_inv * new_global).rotation;
		}
	}

	void Transform::GlobalScale(vec3 scl)
	{
		if (!parent)
			scale = scl;
		else
		{
			auto curr_global = GlobalMatrix();
			auto decomp = decompose(GlobalMatrix());
			decomp.scale = scl;
			auto new_global = decomp.recompose();

			auto parent_inv = parent->Transform()->GlobalMatrix().inverse();
			position = decompose(parent_inv * new_global).scale;
		}
	}

	void Transform::CleanRotation()
	{
		rotation.normalize();
	}
}