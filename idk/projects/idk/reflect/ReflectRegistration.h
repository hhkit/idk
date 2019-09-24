#pragma once

#include <reflect/reflect.h>
#include <core/Core.h>
#include <IncludeComponents.h>
#include <IncludeResources.h>
#include <res/MetaBundle.h>
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

REFLECT_BEGIN(idk::SerializedMeta, "SerializedMetadata")
REFLECT_VARS(guid, name, metadata)
REFLECT_END()

REFLECT_BEGIN(idk::MetaBundle, "MetaBundle")
REFLECT_VARS(metadatas)
REFLECT_END()

REFLECT_BEGIN(idk::TestResource::Metadata, "TestMeta")
REFLECT_VARS(i, j)
REFLECT_END()

REFLECT_ENUM(idk::ColorFormat, "ColorFormat")
REFLECT_ENUM(idk::UVMode, "UVMode")

REFLECT_BEGIN(idk::Texture::Metadata, "TextureMeta")
REFLECT_VARS(uv_mode,internal_format)
REFLECT_END()

REFLECT_BEGIN(idk::TestResource, "TestResource")
REFLECT_VARS(k, yolo)
REFLECT_END()

REFLECT_BEGIN(idk::AudioClip::Metadata, "AudioMeta")
REFLECT_VARS(volume, pitch, minDistance, maxDistance, is3Dsound, isUnique, isLoop)
REFLECT_END()

REFLECT_BEGIN(idk::RscHandle<class idk::Mesh>, "RscHandle<Mesh>")
REFLECT_VARS(guid)
REFLECT_END()

REFLECT_BEGIN(idk::PrefabData, "PrefabData")
REFLECT_VARS(parent_index, components)
REFLECT_END()

REFLECT_BEGIN(idk::PropertyOverride, "PropertyOverride")
REFLECT_VARS(object_index, component_name, property_path)
REFLECT_END()

// shader graph:
REFLECT_ENUM(idk::shadergraph::ValueType, "ShaderGraphValueType")
REFLECT_BEGIN(idk::shadergraph::Graph, "ShaderGraph")
REFLECT_VARS(master_node, nodes, values, links, parameters)
REFLECT_END()
REFLECT_BEGIN(idk::shadergraph::Link, "ShaderGraphLink")
REFLECT_VARS(node_out, node_in, slot_out, slot_in)
REFLECT_END()
REFLECT_BEGIN(idk::shadergraph::Node, "ShaderGraphNode")
REFLECT_VARS(name, guid, position, input_slots, output_slots)
REFLECT_END()
REFLECT_BEGIN(idk::shadergraph::Value, "ShaderGraphValue")
REFLECT_VARS(type, value, node, slot)
REFLECT_END()
REFLECT_BEGIN(idk::shadergraph::Slot, "ShaderGraphSlot")
REFLECT_VARS(type)
REFLECT_END()
REFLECT_BEGIN(idk::shadergraph::Parameter, "ShaderGraphParameter")
REFLECT_VARS(name, type, default_value)
REFLECT_END()

/*==========================================================================
 * core
 *========================================================================*/

REFLECT_BEGIN(idk::Handle<idk::GameObject>, "Handle<GameObject>")
REFLECT_VARS(id)
REFLECT_END()

/*==========================================================================
 * components
 *========================================================================*/

REFLECT_BEGIN(idk::Transform, "Transform")
REFLECT_VARS(scale, rotation, position, parent)
REFLECT_END()

REFLECT_BEGIN(idk::Name, "Name")
REFLECT_VARS(name)
REFLECT_END()

REFLECT_BEGIN(idk::AudioListener, "AudioListener")
REFLECT_VARS(is_active)
REFLECT_END()

REFLECT_BEGIN(idk::MeshRenderer, "MeshRenderer")
REFLECT_VARS(enabled, mesh, cast_shadows, receive_shadows)
REFLECT_END()

REFLECT_BEGIN(idk::PrefabInstance, "PrefabInstance")
REFLECT_VARS(prefab, overrides, objects)
REFLECT_END()