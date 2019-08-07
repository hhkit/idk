#pragma once

namespace idk
{
	constexpr auto MaxScene = 8;

	/// NEVER CHANGE THIS TUPLE WITHOUT ASKING THE TECH LEAD
	/// YOU WILL BREAK ALL SERIALIZATION
	using Components = std::tuple<
		class Transform
		, class Parent

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
	static constexpr auto ComponentCount = std::tuple_size_v<Components>;

	using EngineSystems = std::tuple<
		class TestSystem
		//,	class InputSystem
		//,	class ScriptSystem
		//,	class PhysicsSystem
		,	class GraphicsSystem
		//,	class AudioSystem
		//,	class IEditor
	>;

	using Systems = decltype(std::tuple_cat(
		std::declval<std::tuple<class Application>>(),
		std::declval<EngineSystems>(),
		std::declval<std::tuple<class IEditor>>()
	));

	constexpr auto SystemCount = std::tuple_size_v<Systems>;
}