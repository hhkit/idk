#pragma once
#include <idk_config.h>

namespace idk::natvis
{
    constexpr auto FileSystemID = SystemID<FileSystem>;
    constexpr auto TagManagerID = SystemID<TagManager>;

    constexpr auto GameObjectID = 0;
    constexpr auto TransformID           = ComponentID<Transform          >;
    constexpr auto NameID                = ComponentID<Name               >;
    constexpr auto TagID                 = ComponentID<Tag                >;
    constexpr auto LayerID               = ComponentID<Layer              >;
    constexpr auto PrefabInstanceID      = ComponentID<PrefabInstance     >;
    constexpr auto RigidBodyID           = ComponentID<RigidBody          >;
    constexpr auto ColliderID            = ComponentID<Collider           >;
    constexpr auto MeshRendererID        = ComponentID<MeshRenderer       >;
    constexpr auto CameraID              = ComponentID<Camera             >;
    constexpr auto SkinnedMeshRendererID = ComponentID<SkinnedMeshRenderer>;
    constexpr auto LightID               = ComponentID<Light              >;
    constexpr auto AnimatorID            = ComponentID<Animator           >;
    constexpr auto monoBehaviorID        = ComponentID<mono::Behavior     >;
    constexpr auto AudioSourceID         = ComponentID<AudioSource        >;
    constexpr auto AudioListenerID       = ComponentID<AudioListener      >;
}