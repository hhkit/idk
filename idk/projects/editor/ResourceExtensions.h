#pragma once
#include <array>

namespace idk
{
    class Texture;
    class Mesh;

    template<typename T>
    constexpr static inline std::array<const char*, 0> RscExtensions{};

    template<>
    constexpr static inline std::array RscExtensions<Texture>{".png",".jpg",".jpeg",".bmp",".tga",".dds"};
    template<>
    constexpr static inline std::array RscExtensions<Mesh>{".fbx",".obj",".md5mesh"};
}