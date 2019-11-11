#include "stdafx.h"
#include "Tag.h"
#include <common/TagManager.h>
#include <core/GameObject.h>

namespace idk
{

    Tag::Tag(string_view str)
    {
        index = Core::GetSystem<TagManager>().GetIndexFromTag(str);
    }

    Tag::operator string() const
    {
        return string{ Core::GetSystem<TagManager>().GetTagFromIndex(index) };
    }

}