#include "stdafx.h"
#include "Transform.h"
#include <core/GameObject.h>
#include <common/Parent.h>
#include <math/matrix_transforms.h>
#include <math/matrix_decomposition.h>

namespace idk
{
	mat4 Transform::LocalMatrix() const
	{
		return translate(position) * mat4 { quat_cast<mat3>(rotation)* idk::scale(scale) };
	}

	mat4 Transform::GlobalMatrix() const
	{
		auto parent = GetGameObject()->ParentObject();
		if (parent)
			return LocalMatrix() * parent->Transform()->GlobalMatrix();
		else
			return LocalMatrix();
	}

	void Transform::GlobalMatrix(const mat4& m)
	{
		auto parent = GetGameObject()->ParentObject();
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
		auto parent = GetGameObject()->ParentObject();
		if (!parent)
			return position;
		else
			return GlobalMatrix()[3].xyz;
	}

	quat Transform::GlobalRotation() const
	{
		auto parent = GetGameObject()->ParentObject();
		if (!parent)
			return rotation;
		else
			return decompose(GlobalMatrix()).rotation;
	}

	vec3 Transform::GlobalScale() const
	{
		auto parent = GetGameObject()->ParentObject();
		if (!parent)
			return scale;
		else
		{
			auto mat = GlobalMatrix();
			auto det = mat.determinant();
			return vec3(det < -epsilon ? -mat[0].length() : mat[0].length(), mat[1].length(), mat[2].length());
		}
	}

	void Transform::GlobalPosition(vec3 pos)
	{
		auto parent = GetGameObject()->ParentObject();
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
		auto parent = GetGameObject()->ParentObject();
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
		auto parent = GetGameObject()->ParentObject();
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