#pragma once
#include <idk.h>

namespace idk
{
	/// NEVER CHANGE THIS TUPLE WITHOUT ASKING THE TECH LEAD
	/// YOU WILL BREAK ALL SERIALIZATION
	using Handleables = tuple<
		class GameObject,
		class Transform,
		class Parent,

	/// EDITOR
	// class Prefab,

	/// PHYSICS
	//	class RigidBody,
	//	class Collider,

	/// GRAPHICS
	//	class MeshRenderer,

	/// ANIMATION
	//	class Animator,

	/// SCRIPTING
	//	class MonoBehavior,

	/// AUDIO
	//	class AudioSource
	>;
}