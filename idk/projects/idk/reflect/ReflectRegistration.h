#pragma once

#include <reflect/reflect.h>
#include <core/Core.h>
#include <IncludeComponents.h>

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
 *	  - if REFLECT_VARS has errors, it might be due to
 *		the type not being included in idk_reflect_types.h
*/

/*==========================================================================
 * math
 *========================================================================*/

REFLECT_BEGIN(idk::vec2, "vec2")
REFLECT_CTOR(float, float)
REFLECT_VARS(x, y)
REFLECT_END()

REFLECT_BEGIN(idk::vec3, "vec3")
REFLECT_CTOR(float, float, float)
REFLECT_VARS(x, y, z)
REFLECT_END()

REFLECT_BEGIN(idk::vec4, "vec4")
REFLECT_CTOR(float, float, float, float)
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

/*==========================================================================
 * resources
 *========================================================================*/

REFLECT_BEGIN(idk::RscHandle<class idk::Mesh>, "RscHandle<Mesh>")
REFLECT_VAR(guid)
REFLECT_END()

/*==========================================================================
 * core
 *========================================================================*/

REFLECT_BEGIN(idk::Handle<idk::GameObject>, "Handle<GameObject>")
REFLECT_VAR(id)
REFLECT_END()

/*==========================================================================
 * components
 *========================================================================*/

REFLECT_BEGIN(idk::Parent, "Parent")
REFLECT_VAR(parent)
REFLECT_END()

REFLECT_BEGIN(idk::Transform, "Transform")
REFLECT_VARS(scale, rotation, position)
REFLECT_END()

REFLECT_BEGIN(idk::AudioListener, "AudioListener")
REFLECT_VARS(is_active)
REFLECT_END()

REFLECT_BEGIN(idk::MeshRenderer, "MeshRenderer")
REFLECT_VARS(enabled, mesh, cast_shadows, receive_shadows)
REFLECT_END()