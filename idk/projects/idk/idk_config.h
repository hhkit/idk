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
		,	class RigidBody
		,	class Collider
		//, class Constraint

		/// GRAPHICS
		,	class MeshRenderer
		,   class Camera
		//,   class SkinnedMeshRenderer
		//, class Light

		/// ANIMATION
		//,	class Animator

		/// SCRIPTING
		//,	class MonoBehavior

		/// AUDIO
		,	class AudioSource
		,	class AudioListener
		//,	class AudioSource
		//, class AudioListener

		, class TestComponent
	>;
	static constexpr auto ComponentCount = std::tuple_size_v<Components>;

	using Systems = std::tuple<
		class Application
		,	class FileSystem
		//,	class InputSystem
		//,	class ScriptSystem
		//,	class PhysicsSystem
		,	class GraphicsSystem
		,	class AudioSystem
		,	class ResourceManager
		,	class SceneManager
		,	class IEditor
		,   class TestSystem
	>;

	using Resources = std::tuple<
		class Scene
		,	class TestResource
		,   class AudioClip
		//,	class Texture
		//,	class Font
		//, class Skeleton
		,	class Mesh
		,   class Material
		//,	class Animation
		//,	class Level
	>;

	constexpr auto SystemCount = std::tuple_size_v<Systems>;
	constexpr auto ResourceCount = std::tuple_size_v<Resources>;

}