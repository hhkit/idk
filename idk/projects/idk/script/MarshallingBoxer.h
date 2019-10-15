#pragma once
#include <idk.h>

#include <utility>

#include <mono/jit/jit.h>

#include <core/Handle.h>
#include <script/ManagedObj.h>
#include <script/MonoBehavior.h>
#include <script/ScriptSystem.h>
#include <script/MonoEnvironment.h>

namespace idk::mono
{

	template<typename T, typename = sfinae<!std::is_class_v<T>>>
	std::decay_t<T> box(T & obj) { return box; }

	template<typename T>
	uint64_t box(Handle<T> obj) { return obj.id; }

	MonoObject* box(Handle<mono::Behavior> behavior);
	MonoObject* box(const ManagedObject& obj);
	MonoObject* box(MonoObject* obj);
	MonoObject* box(vec3& obj);
}