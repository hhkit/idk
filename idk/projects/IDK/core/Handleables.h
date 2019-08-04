#pragma once
#include <idk.h>
#include <tuple>

namespace idk
{
	/// NEVER CHANGE THIS TUPLE WITHOUT ASKING THE TECH LEAD
	/// YOU WILL BREAK ALL SERIALIZATION
	using Components = tuple<
		class Transform
	,	class Parent

	/// EDITOR
	//, class Prefab

	/// PHYSICS
	//,	class RigidBody
	//,	class Collider
	//, class Constraint

	/// GRAPHICS
	//,	class MeshRenderer
	//, class Light

	/// ANIMATION
	//,	class Animator

	/// SCRIPTING
	//,	class MonoBehavior

	/// AUDIO
	//,	class AudioSource
	//, class AudioListener
	>;

	using Handleables = std::decay_t<decltype(std::tuple_cat(
		std::declval<std::tuple<class GameObject>>(),
		std::declval<Components>()
	))>;
}