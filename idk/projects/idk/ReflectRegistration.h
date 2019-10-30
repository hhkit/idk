#pragma once

#include <reflect/reflect.h>
#include <core/Core.h>
#include <IncludeComponents.h>
#include <IncludeResources.h>
#include <IncludeSystems.h>
#include <gfx/ShaderGraph.h>
#include <res/MetaBundle.h>
#include <anim/AnimationLayer.h>
#include <math/matrix_decomposition.h>

#include <gfx/RenderTarget.h>
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

REFLECT_BEGIN(float, "float") REFLECT_END()
REFLECT_BEGIN(int, "int") REFLECT_END()

REFLECT_BEGIN(idk::vec2, "vec2")
REFLECT_CTOR(float, float)
REFLECT_VARS(x, y)
REFLECT_END()


REFLECT_BEGIN(idk::ivec2, "ivec2")
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

REFLECT_BEGIN(idk::vec4, "vec4")
REFLECT_CTOR(float, float, float, float)
REFLECT_VARS(x, y, z, w)
REFLECT_END()

REFLECT_BEGIN(idk::ivec4, "ivec4")
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

REFLECT_BEGIN(idk::matrix_decomposition<idk::real>, "matrix_decomposition")
REFLECT_VARS(position, rotation, scale)
REFLECT_END()

/*==========================================================================
 * resource handles
 *========================================================================*/

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
REFLECT_ENUM(idk::FilterMode, "FilterMode")

REFLECT_BEGIN(idk::Texture::Metadata, "TextureMeta")
REFLECT_VARS(uv_mode,internal_format,filter_mode, is_srgb, compressed)
REFLECT_END()

REFLECT_BEGIN(idk::RenderTarget, "RenderTarget")
REFLECT_VARS(size, is_world_renderer, render_debug,color_tex,depth_tex)
REFLECT_END()

REFLECT_ENUM(idk::CMColorFormat, "CMColorFormat")
REFLECT_ENUM(idk::CMUVMode, "CMUVMode")

REFLECT_BEGIN(idk::CubeMap::Metadata, "CubeMapMeta")
REFLECT_VARS(uv_mode, internal_format)
REFLECT_END()

REFLECT_BEGIN(idk::TestResource, "TestResource")
REFLECT_VARS(k, yolo)
REFLECT_END()

REFLECT_BEGIN(idk::Material, "Material")
REFLECT_VARS(_shader_program, uniforms)
REFLECT_END()

REFLECT_BEGIN(idk::UniformInstance, "UniformInstance")
REFLECT_VARS(name, value)
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

REFLECT_BEGIN(idk::Prefab, "Prefab")
REFLECT_VARS(data)
REFLECT_END()

REFLECT_BEGIN(idk::PropertyOverride, "PropertyOverride")
REFLECT_VARS(component_name, property_path, component_nth)
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

/*==========================================================================
 * configs
 *========================================================================*/

REFLECT_BEGIN(idk::TagManager, "TagManager")
REFLECT_END()

REFLECT_BEGIN(idk::TagManagerConfig, "TagManagerConfig")
REFLECT_VARS(tags)
REFLECT_END()

REFLECT_BEGIN(idk::LayerManager, "LayerManager")
REFLECT_END()

REFLECT_BEGIN(idk::LayerManagerConfig, "LayerManagerConfig")
REFLECT_VARS(layers)
REFLECT_END()

REFLECT_BEGIN(idk::SceneManager, "SceneManager")
REFLECT_VARS(_startup_scene, _active_scene)
REFLECT_END()

REFLECT_BEGIN(idk::mono::ScriptSystemConfig, "ScriptConfig")
REFLECT_VARS(path_to_game_dll)
REFLECT_END()

REFLECT_BEGIN(idk::mono::ScriptSystem, "ScriptSystem")
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

REFLECT_BEGIN(idk::Tag, "Tag")
REFLECT_VARS(index)
REFLECT_END()

REFLECT_BEGIN(idk::Layer, "Layer")
REFLECT_VARS(index)
REFLECT_END()

REFLECT_BEGIN(idk::AudioSource, "AudioSource")
REFLECT_VARS(audio_clip_list, volume, pitch, minDistance, maxDistance, is3Dsound, isUnique, isLoop)
REFLECT_END()

REFLECT_BEGIN(idk::AudioListener, "AudioListener")
REFLECT_VARS(is_active)
REFLECT_END()

REFLECT_BEGIN(idk::mono::Behavior, "MonoBehavior")
REFLECT_VARS(enabled)
REFLECT_END()

// PHYSICS
REFLECT_BEGIN(idk::RigidBody, "RigidBody")
REFLECT_VARS(initial_velocity, inv_mass, is_kinematic, use_gravity)
REFLECT_END()

REFLECT_BEGIN(idk::Collider, "Collider")
REFLECT_VARS(enabled, shape, is_trigger, bounciness, dynamic_friction, static_friction)
REFLECT_END()

REFLECT_BEGIN(idk::box, "box")
REFLECT_VARS(center, extents, rotation)
REFLECT_END()

REFLECT_BEGIN(idk::sphere, "sphere")
REFLECT_VARS(center, radius)
REFLECT_END()

REFLECT_BEGIN(idk::capsule, "capsule")
REFLECT_VARS(center, radius, height)
REFLECT_END()

// GRAPHICS
REFLECT_BEGIN(idk::MeshRenderer, "MeshRenderer")
REFLECT_VARS(enabled, mesh, material_instance, cast_shadows, receive_shadows)
REFLECT_END()

REFLECT_BEGIN(idk::Light, "Light")
REFLECT_VARS(light, shadow_bias, casts_shadows)
REFLECT_END()

REFLECT_BEGIN(idk::PointLight, "PointLight")
REFLECT_VARS(intensity, light_color, attenuation_radius)
REFLECT_END()

REFLECT_BEGIN(idk::DirectionalLight, "DirectionalLight")
REFLECT_VARS(intensity, light_color, width, height)
REFLECT_END()

REFLECT_BEGIN(idk::SpotLight, "SpotLight")
REFLECT_VARS(intensity, light_color, inner_angle, outer_angle, attenuation_radius, use_inv_sq_atten)
REFLECT_END()

REFLECT_BEGIN(idk::PrefabInstance, "PrefabInstance")
REFLECT_VARS(prefab, overrides, object_index)
REFLECT_END()

REFLECT_BEGIN(idk::SkinnedMeshRenderer, "SkinnedMeshRenderer")
REFLECT_VARS(mesh, material_instance)
REFLECT_END()

// Animation Enums
REFLECT_ENUM(idk::AnimLayerBlend, "AnimLayerBlend")
REFLECT_ENUM(idk::AnimDataType, "AnimDataType")
REFLECT_ENUM(idk::BlendTreeType, "BlendTreeType")

REFLECT_BEGIN(idk::BasicAnimationState, "BasicAnimationState")
REFLECT_VARS(motion)
REFLECT_END()

REFLECT_BEGIN(idk::BlendTree, "BlendTree")
REFLECT_VARS(params, blend_tree_type)
REFLECT_END()

REFLECT_BEGIN(idk::AnimationState, "AnimationState")
REFLECT_VARS(name, enabled, loop, speed, state_data)
REFLECT_END()

REFLECT_BEGIN(idk::AnimationLayer, "AnimationLayer")
REFLECT_VARS(name, default_state, default_weight, bone_mask, blend_type)
REFLECT_END()


REFLECT_BEGIN(idk::Bone, "Bone")
REFLECT_VARS(_bone_name, _bone_index)
REFLECT_END()

REFLECT_BEGIN(idk::Animator, "Animator")
REFLECT_VARS(skeleton, animation_table, layer_table, layers)
REFLECT_END()

// Particle System
REFLECT_BEGIN(idk::ParticleSystem, "ParticleSystem")
REFLECT_VARS(main, emission, shape, velocity_over_lifetime, color_over_lifetime, size_over_lifetime, rotation_over_lifetime, renderer)
REFLECT_END()

REFLECT_BEGIN(idk::MainModule, "MainModule")
REFLECT_VARS(duration, looping, prewarm, max_particles,
             start_delay, start_lifetime, start_speed, start_size, start_rotation, start_color,
             gravity_modifier, in_world_space, play_on_awake)
REFLECT_END();

REFLECT_BEGIN(idk::EmissionModule, "EmissionModule")
REFLECT_VARS(enabled, rate_over_time)
REFLECT_END()

REFLECT_BEGIN(idk::ShapeModule, "ShapeModule")
REFLECT_VARS(enabled, radius, radius_thickness, randomize_direction)
REFLECT_END()

REFLECT_ENUM(idk::MinMaxMode, "MinMaxMode")
REFLECT_BEGIN(idk::MinMax<idk::vec3>, "MinMaxVec3") REFLECT_VARS(min, max, mode) REFLECT_END()
REFLECT_BEGIN(idk::MinMax<idk::color>, "MinMaxColor") REFLECT_VARS(min, max, mode) REFLECT_END()
REFLECT_BEGIN(idk::MinMax<idk::rad>, "MinMaxRad") REFLECT_VARS(min, max, mode) REFLECT_END()
REFLECT_BEGIN(idk::MinMax<float>, "MinMaxFloat") REFLECT_VARS(min, max, mode) REFLECT_END()

REFLECT_BEGIN(idk::VelocityOverLifetimeModule, "VelocityOverLifetimeModule")
REFLECT_VARS(enabled, linear)
REFLECT_END()

REFLECT_BEGIN(idk::ColorOverLifetimeModule, "ColorOverLifetimeModule")
REFLECT_VARS(enabled, color)
REFLECT_END()

REFLECT_BEGIN(idk::RotationOverLifetimeModule, "RotationOverLifetimeModule")
REFLECT_VARS(enabled, angular_velocity)
REFLECT_END()

REFLECT_BEGIN(idk::SizeOverLifetimeModule, "SizeOverLifetimeModule")
REFLECT_VARS(enabled, size)
REFLECT_END()

REFLECT_BEGIN(idk::RendererModule, "RendererModule")
REFLECT_VARS(enabled, material)
REFLECT_END()


REFLECT_BEGIN(idk::Viewport, "Viewport")
REFLECT_VARS(position,size)
REFLECT_END()


REFLECT_BEGIN(idk::DontClear, "<Don't Clear>")
REFLECT_END()

REFLECT_BEGIN(idk::Camera, "Camera")
REFLECT_VARS(enabled, near_plane, far_plane, depth,clear,is_orthographic,overlay_debug_draw, viewport,render_target)
REFLECT_END()