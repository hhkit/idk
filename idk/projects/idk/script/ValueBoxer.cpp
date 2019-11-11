#include "stdafx.h"
#include "ValueBoxer.h"
#include <script/ManagedObj.h>
#include <script/MonoBehavior.h>
#include <script/ScriptSystem.h>
#include <script/MonoEnvironment.h>

namespace idk::mono
{
	MonoObject* box(const ManagedObject& obj)
	{
		return obj.Raw();
	}

	MonoObject* box(MonoObject* obj)
	{
		return obj;
	}

	MonoObject* box(vec3& obj)
	{
		return mono_value_box(mono_domain_get(),
			Core::GetSystem<ScriptSystem>().Environment().Type("Vector3")->Raw(), // todo: get boxed thing
			&obj);
	}
	MonoObject* box(ManagedCollision& obj)
	{
		return mono_value_box(mono_domain_get(),
			Core::GetSystem<ScriptSystem>().Environment().Type("Collision")->Raw(), // todo: get boxed thing
			&obj);
	}
}