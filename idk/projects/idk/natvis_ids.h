#pragma once
#include <idk_config.h>

namespace idk::natvis
{
    constexpr auto FileSystemID = SystemID<FileSystem>;
    constexpr auto TagManagerID = SystemID<TagManager>;

    constexpr auto GameObjectID = 0;
    constexpr auto TransformID           = ComponentID<Transform          > + 1;
    constexpr auto NameID                = ComponentID<Name               > + 1;
    constexpr auto TagID                 = ComponentID<Tag                > + 1;
    constexpr auto LayerID               = ComponentID<Layer              > + 1;
    constexpr auto PrefabInstanceID      = ComponentID<PrefabInstance     > + 1;
    constexpr auto RigidBodyID           = ComponentID<RigidBody          > + 1;
    constexpr auto ColliderID            = ComponentID<Collider           > + 1;
    constexpr auto MeshRendererID        = ComponentID<MeshRenderer       > + 1;
    constexpr auto CameraID              = ComponentID<Camera             > + 1;
    constexpr auto SkinnedMeshRendererID = ComponentID<SkinnedMeshRenderer> + 1;
    constexpr auto LightID               = ComponentID<Light              > + 1;
    constexpr auto AnimatorID            = ComponentID<Animator           > + 1;
    constexpr auto monoBehaviorID        = ComponentID<mono::Behavior     > + 1;
    constexpr auto AudioSourceID         = ComponentID<AudioSource        > + 1;
    constexpr auto AudioListenerID       = ComponentID<AudioListener      > + 1;
    constexpr auto ParticleSystemID      = ComponentID<ParticleSystem     > + 1;
    constexpr auto CanvasID              = ComponentID<Canvas             > + 1;
    constexpr auto RectTransformID       = ComponentID<RectTransform      > + 1;
}