#pragma once

#include <ReflectReg_Common.inl>
#include <IncludeComponents.h>
/*==========================================================================
 * components
 *========================================================================*/
#pragma region Components
 // COMMON
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

// AUDIO
REFLECT_BEGIN(idk::vector<idk::RscHandle<idk::AudioClip>>, "vector<<AudioClip>>")
REFLECT_END()

REFLECT_BEGIN(idk::AudioSource, "AudioSource")
REFLECT_VARS(audio_clip_list, audio_clip_volume, volume, pitch, minDistance, maxDistance, is3Dsound, isUnique, isLoop)
REFLECT_END()

REFLECT_BEGIN(idk::AudioListener, "AudioListener")
REFLECT_VARS(is_active)
REFLECT_END()

// MONO
REFLECT_BEGIN(idk::mono::ManagedObject, "ManagedObject")
REFLECT_END()

REFLECT_BEGIN(idk::mono::Behavior, "MonoBehavior")
REFLECT_VARS(enabled, script_data)
REFLECT_END()

// PHYSICS
REFLECT_BEGIN(idk::RigidBody, "RigidBody")
REFLECT_VARS(initial_velocity, inv_mass, is_kinematic, use_gravity)
REFLECT_END()

REFLECT_BEGIN(idk::Collider, "Collider")
REFLECT_VARS(enabled, shape, is_trigger, bounciness, dynamic_friction, static_friction)
REFLECT_END()

// PREFAB
REFLECT_BEGIN(idk::PropertyOverride, "PropertyOverride")
REFLECT_VARS(component_name, property_path, component_nth, value)
REFLECT_END()

REFLECT_BEGIN(idk::ComponentNth, "ComponentNth")
REFLECT_VARS(component_name, component_nth)
REFLECT_END()

REFLECT_BEGIN(idk::vector<idk::PropertyOverride>, "vector<PropertyOverride>")
REFLECT_END()
REFLECT_BEGIN(idk::vector<idk::ComponentNth>, "vector<ComponentNth>")
REFLECT_END()

REFLECT_BEGIN(idk::PrefabInstance, "PrefabInstance")
REFLECT_VARS(prefab, overrides, object_index, removed_components)
REFLECT_END()

#pragma endregion