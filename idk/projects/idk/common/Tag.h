#pragma once

#include <core/Component.h>

namespace idk
{
    class Tag : public Component<Tag>
    {
    public:
        uint8_t index;

        Tag() = default;
        explicit Tag(string_view str);
        explicit operator string() const;
    };
};