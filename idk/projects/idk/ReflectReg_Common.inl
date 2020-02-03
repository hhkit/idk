#pragma once

#include <reflect/reflect.inl>
#include <core/Core.inl>
#include <math/matrix_decomposition.inl>
#include <ds/span.inl>
#include <ds/result.inl>
#include <serialize/text.inl>
/*
 * !!! NOTE !!!
 * TO BE INCLUDED IN THE ENTRY POINT CPP, LIKE GAME.CPP
 */

 /*
  *	macros:
  *	  REFLECT_BEGIN(CLASS_TYPE, ALIAS)
  *	  REFLECT_END()
  *	  REFLECT_VARS(VAR_0 , VAR_1 , ... , VAR_N)
  *	  REFLECT_PARENT(PARENT_TYPE)
  *	  REFLECT_CTOR(ARG_TYPE_0, ARG_TYPE_1 , ... , ARG_TYPE_N) // max n = 6
  *	  REFLECT_FRIEND
  *    REFLECT_ENUM(ENUM_TYPE, ALIAS) // only for macro-based enum using ENUM
  *
  *	notes:
  *    - !!! WARNING !!!
  *      Please DO NOT USE SPACES for the alias, as yaml tags cannot have spaces.
  *	  - You don't have to include every type in idk_reflect_type,
  *      just the ones you need type context during visit. However, you need
  *      to register the dependent types BEFORE the dependees.
  *      eg. AnimationLayer before Animator
  *    - You can also choose not to include container types in idk_reflect_type.
  *      however, you will need to register it the same way
  *      ie. REFLECT_BEGIN(container, name) REFLECT_END()
  *    - if the container type has commas, use using alias = type before registering,
  *      or use decltype(type::member) instead of the container type
 */

REFLECT_BEGIN(idk::reflect::dynamic, "dynamic") REFLECT_END()
REFLECT_BEGIN(idk::reflect::type, "type") REFLECT_END()

REFLECT_BEGIN(idk::string, "string") REFLECT_END()
REFLECT_BEGIN(float, "float") REFLECT_END()
REFLECT_BEGIN(int, "int") REFLECT_END()
REFLECT_BEGIN(bool, "bool") REFLECT_END()
REFLECT_BEGIN(char, "char") REFLECT_END()
REFLECT_BEGIN(short, "short") REFLECT_END()
REFLECT_BEGIN(unsigned, "uint") REFLECT_END()
REFLECT_BEGIN(double, "double") REFLECT_END()

// COMMON CONTAINERS
REFLECT_BEGIN(idk::vector<idk::reflect::dynamic>, "vector<dynamic>")
REFLECT_END()
REFLECT_BEGIN(idk::vector<idk::string>, "vector<string>")
REFLECT_END()
REFLECT_BEGIN(idk::vector<unsigned char>, "vector<uchar>")
REFLECT_END()
REFLECT_BEGIN(idk::vector<float>, "vector<float>")
REFLECT_END()

/*==========================================================================
 * math
 *========================================================================*/
#pragma region Math
	REFLECT_BEGIN(idk::vec2, "vec2")
	REFLECT_CTOR(float, float)
	REFLECT_VARS(x, y)
	REFLECT_END()

	REFLECT_BEGIN(idk::ivec2, "ivec2")
	REFLECT_CTOR(int, int)
	REFLECT_VARS(x, y)
	REFLECT_END()

	REFLECT_BEGIN(idk::uvec2, "uvec2")
	REFLECT_CTOR(int, int)
	REFLECT_VARS(x, y)
	REFLECT_END()

	REFLECT_BEGIN(idk::vec3, "vec3")
	REFLECT_CTOR(float, float, float)
	REFLECT_VARS(x, y, z)
	REFLECT_END()

	REFLECT_BEGIN(idk::ivec3, "ivec3")
	REFLECT_CTOR(int, int, int)
	REFLECT_VARS(x, y, z)
	REFLECT_END()

	REFLECT_BEGIN(idk::uvec3, "uvec3")
	REFLECT_CTOR(int, int, int)
	REFLECT_VARS(x, y, z)
	REFLECT_END()

	REFLECT_BEGIN(idk::vec4, "vec4")
	REFLECT_CTOR(float, float, float, float)
	REFLECT_VARS(x, y, z, w)
	REFLECT_END()

	REFLECT_BEGIN(idk::ivec4, "ivec4")
	REFLECT_CTOR(int, int, int, int)
	REFLECT_VARS(x, y, z, w)
	REFLECT_END()

	REFLECT_BEGIN(idk::uvec4, "uvec4")
	REFLECT_CTOR(int, int, int, int)
	REFLECT_VARS(x, y, z, w)
	REFLECT_END()

	REFLECT_BEGIN(idk::quat, "quat")
	REFLECT_CTOR(float, float, float, float)
	REFLECT_VARS(x, y, z, w)
	REFLECT_END()

	REFLECT_BEGIN(idk::mat3, "mat3")
	REFLECT_CTOR(float, float, float, float, float, float, float, float, float)
	REFLECT_VARS(intern[0], intern[1], intern[2])
	REFLECT_END()

	REFLECT_BEGIN(idk::mat4, "mat4")
	REFLECT_CTOR(float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float)
	REFLECT_VARS(intern[0], intern[1], intern[2], intern[3])
	REFLECT_END()

	REFLECT_BEGIN(idk::color, "color")
	REFLECT_CTOR(float, float, float, float)
	REFLECT_CTOR(float, float, float)
	REFLECT_VARS(r, g, b, a)
	REFLECT_END()

	REFLECT_BEGIN(idk::rad, "rad")
	REFLECT_CTOR(float)
	REFLECT_VARS(value)
	REFLECT_END()

	REFLECT_BEGIN(idk::box, "box")
	REFLECT_VARS(center, extents, rotation)
	REFLECT_END()

	REFLECT_BEGIN(idk::sphere, "sphere")
	REFLECT_VARS(center, radius)
	REFLECT_END()

	REFLECT_BEGIN(idk::capsule, "capsule")
	REFLECT_VARS(center, radius, height, dir)
	REFLECT_END()


	REFLECT_BEGIN(idk::rect, "rect")
	REFLECT_VARS(position, size)
	REFLECT_END()

	REFLECT_BEGIN(idk::matrix_decomposition<idk::real>, "matrix_decomposition")
	REFLECT_VARS(position, rotation, scale)
	REFLECT_END()
#pragma endregion Math

	
/*==========================================================================
 * core
 *========================================================================*/
#pragma region Core
REFLECT_BEGIN(idk::Handle<idk::GameObject>, "Handle<GameObject>")
REFLECT_VARS(id)
REFLECT_END()
#pragma endregion
	/*==========================================================================
	 * resource handles
	 *========================================================================*/
#pragma region Resource Handles
	REFLECT_BEGIN(idk::RscHandle<class idk::Scene>, "<Scene>")
	REFLECT_VARS(guid)
	REFLECT_END()

	REFLECT_BEGIN(idk::RscHandle<class idk::Prefab>, "<Prefab>")
	REFLECT_VARS(guid)
	REFLECT_END()

	REFLECT_BEGIN(idk::RscHandle<class idk::Mesh>, "<Mesh>")
	REFLECT_VARS(guid)
	REFLECT_END()

	REFLECT_BEGIN(idk::RscHandle<class idk::AudioClip>, "<AudioClip>")
	REFLECT_VARS(guid)
	REFLECT_END()

	REFLECT_BEGIN(idk::RscHandle<class idk::Texture>, "<Texture>")
	REFLECT_VARS(guid)
	REFLECT_END()

	REFLECT_BEGIN(idk::RscHandle<class idk::FontAtlas>, "<FontAtlas>")
	REFLECT_VARS(guid)
	REFLECT_END()

	REFLECT_BEGIN(idk::RscHandle<class idk::Material>, "<Material>")
	REFLECT_VARS(guid)
	REFLECT_END()

	REFLECT_BEGIN(idk::RscHandle<class idk::anim::Animation>, "<Animation>")
	REFLECT_VARS(guid)
	REFLECT_END()

	REFLECT_BEGIN(idk::RscHandle<class idk::anim::Skeleton>, "<Skeleton>")
	REFLECT_VARS(guid)
	REFLECT_END()

	REFLECT_BEGIN(idk::RscHandle<class idk::CubeMap>, "<CubeMap>")
	REFLECT_VARS(guid)
	REFLECT_END()

	REFLECT_BEGIN(idk::RscHandle<class idk::MaterialInstance>, "<MaterialInstance>")
	REFLECT_VARS(guid)
	REFLECT_END()

	REFLECT_BEGIN(idk::RscHandle<class idk::ShaderProgram>, "<ShaderProgram>")
	REFLECT_VARS(guid)
	REFLECT_END()

	REFLECT_BEGIN(idk::RscHandle<class idk::RenderTarget>, "<RenderTarget>")
	REFLECT_VARS(guid)
	REFLECT_END()
#pragma endregion