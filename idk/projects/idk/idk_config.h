#pragma once
#include <meta/tuple.h>
namespace idk
{
	namespace anim { class Skeleton; class Animation; }
	namespace shadergraph { class Graph; }

	constexpr auto MaxScene = 0x82; // 130 scenes allowed

	/// NEVER CHANGE THIS TUPLE WITHOUT ASKING THE TECH LEAD
	/// YOU WILL BREAK ALL SERIALIZATION
	using Components = std::tuple<
		class Transform
		, class Name
		/// EDITOR
		,   class PrefabInstance

		/// PHYSICS
		,	class RigidBody
		,	class Collider
		//, class Constraint

		/// GRAPHICS
		,	class MeshRenderer
		,   class Camera
		,   class SkinnedMeshRenderer
		, class Light

		/// ANIMATION
		,	class AnimationController

		/// SCRIPTING
		//,	class MonoBehavior

		/// AUDIO
		,	class AudioSource
		,	class AudioListener

		/// IVAN BEING STUPID
		, class TestComponent
	>;
	
	using Systems = std::tuple<
		class Application
		,	class FileSystem
		//,	class InputSystem
		//,	class ScriptSystem
		,	class PhysicsSystem
		,	class GraphicsSystem
		,   class DebugRenderer
		,	class AudioSystem
		,	class ResourceManager
		,	class SaveableResourceManager
		,	class SceneManager
		,	class ProjectManager
		,	class IEditor
		,   class TestSystem
		,	class AnimationSystem
	>;

	using Resources = std::tuple<
		class Scene
		,	class TestResource
		,   class AudioClip
		//,	class Font
		,	class Prefab
		,	class ShaderTemplate
		,	class Mesh
		,   class Material
		,	class Texture
		,	class ShaderProgram
		,	class RenderTarget
		,	anim::Animation
		,	anim::Skeleton
        ,   shadergraph::Graph
		//,	class Level
	>;

	template<typename Component> constexpr auto ComponentID = index_in_tuple_v<Component, Components>;
	template<typename System>    constexpr auto SystemID    = index_in_tuple_v<System, Systems>;
	template<typename Resource>  constexpr auto ResourceID  = index_in_tuple_v<Resource, Resources>;

	constexpr auto ComponentCount = std::tuple_size_v<Components>;
	constexpr auto SystemCount    = std::tuple_size_v<Systems>;
	constexpr auto ResourceCount  = std::tuple_size_v<Resources>;

}