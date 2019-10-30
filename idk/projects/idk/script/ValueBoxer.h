#pragma once
#include <idk.h>

#include <utility>

#include <mono/jit/jit.h>

#include <core/Handle.h>

namespace idk
{
	struct ManagedCollision;
}

namespace idk::mono
{
	class Behavior;
	class ManagedObject;

	template<typename T, typename = sfinae<!std::is_class_v<T>>>
	std::decay_t<T> box(T & obj) { return obj; }

	template<typename T>
	uint64_t box(Handle<T> obj) { return obj.id; }



	MonoObject* box(Handle<Behavior> behavior);
	MonoObject* box(const ManagedObject& obj);
	MonoObject* box(MonoObject* obj);
	MonoObject* box(vec3& obj);
	MonoObject* box(ManagedCollision& obj);
}