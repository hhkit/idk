#pragma once
#include <meta/tuple.h>
namespace idk
{
	namespace anim { class Skeleton; class Animation; }
	namespace mono { class ScriptSystem; class Behavior; }
	namespace shadergraph { class Graph; }

	constexpr auto MaxScene = 0x82; // 130 scenes allowed

	/// NEVER CHANGE THIS TUPLE WITHOUT ASKING THE TECH LEAD
	/// YOU WILL BREAK ALL SERIALIZATION
	using Components = std::tuple<
		class Transform
		, class Name
        , class Tag
        , class Layer

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
		,	class Light

		/// ANIMATION
		,	class Animator

		/// SCRIPTING
		,	class mono::Behavior

		/// AUDIO
		,	class AudioSource
		,	class AudioListener

		/// IVAN BEING STUPID
		,	class TestComponent
	>;
	
	using Systems = std::tuple<
			class LogSystem
		,	class Application
		,	class FileSystem
		//,	class InputSystem
		,	class GamepadSystem
		,	class mono::ScriptSystem
		,	class PhysicsSystem
		,	class GraphicsSystem
		,   class DebugRenderer
		,	class AudioSystem
		
		,	class ProjectManager
		,	class SceneManager
		,	class IEditor
		,   class TestSystem
		,	class AnimationSystem
        ,   class TagManager
		, class LayerManager
		, class ResourceManager
        
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
		,	class MaterialInstance
		,	class Texture
		,   class CubeMap
		,	class ShaderProgram
		,	class RenderTarget
		,	anim::Animation
		,	anim::Skeleton
		//,	class Level
	>;

	template<typename Component> constexpr auto ComponentID = index_in_tuple_v<Component, Components>;
	template<typename System>    constexpr auto SystemID    = index_in_tuple_v<System, Systems>;
	template<typename Resource>  constexpr auto ResourceID  = index_in_tuple_v<Resource, Resources>;

	constexpr auto ComponentCount = std::tuple_size_v<Components>;
	constexpr auto SystemCount    = std::tuple_size_v<Systems>;
	constexpr auto ResourceCount  = std::tuple_size_v<Resources>;
}