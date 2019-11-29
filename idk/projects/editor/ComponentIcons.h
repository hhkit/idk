#pragma once

#include <idk.h>
#include <IconsFontAwesome5_c.h>

namespace idk
{

    template<typename T>
    static constexpr inline auto ComponentIcon = "";

    template<> static constexpr inline auto ComponentIcon<Transform> = ICON_FA_CUBE;

    template<> static constexpr inline auto ComponentIcon<RigidBody> = ICON_FA_CERTIFICATE;
    template<> static constexpr inline auto ComponentIcon<Collider> = ICON_FA_VECTOR_SQUARE;

    template<> static constexpr inline auto ComponentIcon<MeshRenderer> = ICON_FA_GLOBE;
    template<> static constexpr inline auto ComponentIcon<SkinnedMeshRenderer> = ICON_FA_GLOBE;
    template<> static constexpr inline auto ComponentIcon<Camera> = ICON_FA_VIDEO;
    template<> static constexpr inline auto ComponentIcon<Light> = ICON_FA_LIGHTBULB;
    template<> static constexpr inline auto ComponentIcon<TextMesh> = ICON_FA_FONT;

    template<> static constexpr inline auto ComponentIcon<ParticleSystem> = ICON_FA_ATOM;

    template<> static constexpr inline auto ComponentIcon<Animator> = ICON_FA_STREAM;
    template<> static constexpr inline auto ComponentIcon<Bone> = ICON_FA_BONE;

    template<> static constexpr inline auto ComponentIcon<Canvas> = ICON_FA_OBJECT_GROUP;
    template<> static constexpr inline auto ComponentIcon<RectTransform> = ICON_FA_BORDER_STYLE;
    template<> static constexpr inline auto ComponentIcon<Image> = ICON_FA_IMAGE;
    template<> static constexpr inline auto ComponentIcon<Text> = ICON_FA_FONT;

    template<> static constexpr inline auto ComponentIcon<mono::Behavior> = ICON_FA_FILE_CODE;

    template<> static constexpr inline auto ComponentIcon<AudioSource> = ICON_FA_VOLUME_UP;
    template<> static constexpr inline auto ComponentIcon<AudioListener> = ICON_FA_MICROPHONE;

}