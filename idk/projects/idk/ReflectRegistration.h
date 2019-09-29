#pragma once

#include <reflect/reflect.h>
#include <core/Core.h>
#include <IncludeComponents.h>
#include <IncludeResources.h>
#include <gfx/ShaderGraph.h>
#include <res/MetaBundle.h>
#include <math/matrix_decomposition.h>

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

REFLECT_BEGIN(idk::rad, "rad")
REFLECT_CTOR(float)
REFLECT_VARS(value)
REFLECT_END()

REFLECT_BEGIN(idk::matrix_decomposition<idk::real>, "matrix_decomposition")
REFLECT_VARS(position, rotation, scale)
REFLECT_END()

/*==========================================================================
 * resource handles
 *========================================================================*/

REFLECT_BEGIN(idk::RscHandle<class idk::Scene>, "RscHandle<Scene>")
REFLECT_VARS(guid)
REFLECT_END()

REFLECT_BEGIN(idk::RscHandle<class idk::Mesh>, "RscHandle<Mesh>")
REFLECT_VARS(guid)
REFLECT_END()

REFLECT_BEGIN(idk::RscHandle<class idk::Material>, "RscHandle<Material>")
REFLECT_VARS(guid)
REFLECT_END()

REFLECT_BEGIN(idk::RscHandle<class idk::anim::Animation>, "RscHandle<Animation>")
REFLECT_VARS(guid)
REFLECT_END()

REFLECT_BEGIN(idk::RscHandle<class idk::anim::Skeleton>, "RscHandle<Skeleton>")
REFLECT_VARS(guid)
REFLECT_END()

REFLECT_BEGIN(idk::RscHandle<class idk::MaterialInstance>, "RscHandle<MaterialInstance>")
REFLECT_VARS(guid)
REFLECT_END()

REFLECT_BEGIN(idk::RscHandle<class idk::ShaderProgram>, "RscHandle<ShaderProgram>")
REFLECT_VARS(guid)
REFLECT_END()

/*==========================================================================
 * general resources
 *========================================================================*/

REFLECT_BEGIN(idk::SerializedMeta, "SerializedMetadata")
REFLECT_VARS(guid, name, t_hash, metadata)
REFLECT_END()

REFLECT_BEGIN(idk::MetaBundle, "MetaBundle")
REFLECT_VARS(metadatas)
REFLECT_END()


/*==========================================================================
 * specific resources
 *========================================================================*/

REFLECT_BEGIN(idk::TestResource::Metadata, "TestMeta")
REFLECT_VARS(i, j)
REFLECT_END()

REFLECT_ENUM(idk::ColorFormat, "ColorFormat")
REFLECT_ENUM(idk::UVMode, "UVMode")

REFLECT_BEGIN(idk::Texture::Metadata, "TextureMeta")
REFLECT_VARS(uv_mode,internal_format)
REFLECT_END()

REFLECT_ENUM(idk::CMColorFormat, "CMColorFormat")
REFLECT_ENUM(idk::CMUVMode, "CMUVMode")

REFLECT_BEGIN(idk::CubeMap::Metadata, "CubeMapMeta")
REFLECT_VARS(uv_mode, internal_format)
REFLECT_END()

REFLECT_BEGIN(idk::TestResource, "TestResource")
REFLECT_VARS(k, yolo)
REFLECT_END()

REFLECT_BEGIN(idk::AudioClip::Metadata, "AudioMeta")
REFLECT_VARS(volume, pitch, minDistance, maxDistance, is3Dsound, isUnique, isLoop)
REFLECT_END()

REFLECT_BEGIN(idk::Material, "Material")
REFLECT_VARS(_shader_program, uniforms)
REFLECT_END()

REFLECT_ENUM(idk::BlendMode, "BlendMode")
REFLECT_ENUM(idk::MaterialDomain, "MaterialDomain")
REFLECT_ENUM(idk::ShadingModel, "ShadingModel")

REFLECT_BEGIN(idk::Material::Metadata, "MaterialMeta")
REFLECT_VARS(domain, blend, model)
REFLECT_END()

REFLECT_BEGIN(idk::MaterialInstance, "MaterialInstance")
REFLECT_VARS(material, uniforms)
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
REFLECT_VARS(_shader_program, master_node, nodes, links, parameters)
REFLECT_END()
REFLECT_BEGIN(idk::shadergraph::Link, "ShaderGraphLink")
REFLECT_VARS(node_out, node_in, slot_out, slot_in)
REFLECT_END()
REFLECT_BEGIN(idk::shadergraph::Node, "ShaderGraphNode")
REFLECT_VARS(name, guid, position, input_slots, output_slots, control_values)
REFLECT_END()
REFLECT_BEGIN(idk::shadergraph::Slot, "ShaderGraphSlot")
REFLECT_VARS(type, value)
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

REFLECT_BEGIN(idk::SceneManager, "SceneManager")
REFLECT_VARS(_scenes)
REFLECT_END()

REFLECT_BEGIN(idk::SceneManager::SceneBlock, "SceneBlock")
REFLECT_VARS(build_index, scene)
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

// PHYSICS
REFLECT_BEGIN(idk::RigidBody, "RigidBody")
REFLECT_VARS(initial_velocity, inv_mass, is_kinematic, use_gravity, restitution)
REFLECT_END()

REFLECT_BEGIN(idk::Collider, "Collider")
REFLECT_VARS(enabled, shape, is_trigger)
REFLECT_END()

REFLECT_BEGIN(idk::box, "box")
REFLECT_VARS(center, extents, axes)
REFLECT_END()

REFLECT_BEGIN(idk::sphere, "sphere")
REFLECT_VARS(center, radius)
REFLECT_END()

REFLECT_BEGIN(idk::capsule, "sphere")
REFLECT_VARS(center, radius, height)
REFLECT_END()

// GRAPHICS
REFLECT_BEGIN(idk::MeshRenderer, "MeshRenderer")
REFLECT_VARS(enabled, mesh, cast_shadows, receive_shadows)
REFLECT_END()

REFLECT_BEGIN(idk::Light, "Light")
REFLECT_VARS(light, shadow_bias, casts_shadows)
REFLECT_END()

REFLECT_BEGIN(idk::PointLight, "PointLight")
REFLECT_VARS(intensity, light_color, attenuation_radius)
REFLECT_END()

REFLECT_BEGIN(idk::DirectionalLight, "DirectionalLight")
REFLECT_VARS(intensity, light_color)
REFLECT_END()

REFLECT_BEGIN(idk::SpotLight, "SpotLight")
REFLECT_VARS(intensity, light_color, inner_angle, outer_angle, attenuation_radius)
REFLECT_END()

REFLECT_BEGIN(idk::PrefabInstance, "PrefabInstance")
REFLECT_VARS(prefab, overrides, objects)
REFLECT_END()

REFLECT_BEGIN(idk::SkinnedMeshRenderer, "SkinnedMeshRenderer")
REFLECT_VARS(mesh, material_instance)
REFLECT_END()

REFLECT_BEGIN(idk::Animator, "Animator")
REFLECT_VARS(_skeleton, _animation_table, _animations, _child_objects, _bone_transforms, _bind_pose)
REFLECT_END()

REFLECT_BEGIN(idk::Camera, "Camera")
REFLECT_VARS(enabled, near_plane, far_plane)
REFLECT_END()