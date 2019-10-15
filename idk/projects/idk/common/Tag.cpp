#include "stdafx.h"
#include "Tag.h"
#include <common/TagManager.h>
#include <core/GameObject.h>

namespace idk
{

    Tag::Tag(string_view str)
    {
        index = Core::GetSystem<TagManager>().GetIndexFromTag(str);
        if (index == 0)
            GetGameObject()->RemoveComponent(GetHandle());
    }

    Tag::operator string() const
    {
        return string{ Core::GetSystem<TagManager>().GetTagFromIndex(index) };
    }

}