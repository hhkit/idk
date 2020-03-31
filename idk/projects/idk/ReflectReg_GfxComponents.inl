#pragma once
#include <ReflectReg_Common.inl>
#include <IncludeComponents.h>

// GRAPHICS
#pragma region Graphics
REFLECT_BEGIN(idk::MeshRenderer, "MeshRenderer")
REFLECT_VARS(enabled, mesh, material_instance, cast_shadows, receive_shadows)
REFLECT_END()

REFLECT_BEGIN(idk::Light, "Light")
REFLECT_VARS(enabled, light, shadow_bias, casts_shadows, update_shadow, shadow_layers)
REFLECT_END()

REFLECT_BEGIN(idk::PointLight, "PointLight")
REFLECT_VARS(intensity, light_color, attenuation_radius, use_inv_sq_atten)
REFLECT_END()

REFLECT_BEGIN(idk::DirectionalLight, "DirectionalLight")
REFLECT_VARS(intensity, light_color, left, right, bottom, top, near, far)
REFLECT_END()

REFLECT_BEGIN(idk::SpotLight, "SpotLight")
REFLECT_VARS(intensity, light_color, inner_angle, outer_angle, attenuation_radius, use_inv_sq_atten)
REFLECT_END()

REFLECT_BEGIN(idk::SkinnedMeshRenderer, "SkinnedMeshRenderer")
REFLECT_VARS(mesh, material_instance)
REFLECT_END()

REFLECT_ENUM(idk::TextAlignment, "TextAlignment")
REFLECT_ENUM(idk::TextAnchor, "TextAnchor")

REFLECT_BEGIN(idk::TextMesh, "TextMesh")
REFLECT_VARS(text, letter_spacing, line_height, anchor, alignment, color, font_size, font)
REFLECT_END()
#pragma endregion

// CAMERA
#pragma region Camera
REFLECT_BEGIN(idk::DontClear, "Don'tClear")
REFLECT_VAR(hack)
REFLECT_END()
REFLECT_BEGIN(idk::DepthOnly, "DepthOnly")
REFLECT_VAR(hack)
REFLECT_END()

REFLECT_BEGIN(idk::LayerMask, "LayerMask")
REFLECT_VARS(mask)
REFLECT_END()

REFLECT_BEGIN(idk::Camera, "Camera")
REFLECT_VARS(enabled, layer_mask, near_plane, far_plane, field_of_view, orthographic_size,depth, clear, is_orthographic, viewport, render_target,gamma)
REFLECT_END()
#pragma endregion

// PARTICLE SYSTEM
#pragma region Particle System
REFLECT_BEGIN(idk::MainModule, "MainModule")
REFLECT_VARS(duration, looping, prewarm, max_particles,
	start_delay, start_lifetime, start_speed, start_size, start_rotation, start_color,
	gravity_modifier, in_world_space, play_on_awake, destroy_on_finish)
REFLECT_END();

REFLECT_BEGIN(idk::EmissionModule::Burst, "EmissionBurst")
REFLECT_VARS(time, count, cycles, interval, probability)
REFLECT_END()

REFLECT_BEGIN(idk::vector<idk::EmissionModule::Burst>, "vector<EmissionBurst>")
REFLECT_END()

REFLECT_BEGIN(idk::EmissionModule, "EmissionModule")
REFLECT_VARS(enabled, rate_over_time, bursts)
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
REFLECT_VARS(enabled, linear, orbital, offset, radial)
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

REFLECT_BEGIN(idk::ParticleSystem, "ParticleSystem")
REFLECT_VARS(main, emission, shape, velocity_over_lifetime, color_over_lifetime, size_over_lifetime, rotation_over_lifetime, renderer)
REFLECT_END()
#pragma endregion

// UI
#pragma region UI
REFLECT_BEGIN(idk::RectTransform, "RectTransform")
REFLECT_VARS(anchor_min, anchor_max, offset_min, offset_max, pivot)
REFLECT_END()

REFLECT_BEGIN(idk::Canvas, "Canvas")
REFLECT_VARS(render_target)
REFLECT_END()

REFLECT_BEGIN(idk::Image, "Image")
REFLECT_VARS(texture, tint, material, preserve_aspect)
REFLECT_END()

REFLECT_BEGIN(idk::Text, "Text")
REFLECT_VARS(text, font, font_size, letter_spacing, line_height, alignment, wrap, best_fit, color, material)
REFLECT_END()
#pragma endregion