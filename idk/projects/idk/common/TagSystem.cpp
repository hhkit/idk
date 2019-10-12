#include "stdafx.h"
#include "TagSystem.h"
#include <common/Tag.h>
#include <iostream>

namespace idk
{
    Handle<GameObject> TagSystem::Find(string_view tag)
    {
        auto iter = _tags_to_indices.find(tag);
        if (iter == _tags_to_indices.end())
            return Handle<GameObject>();

        auto index = iter->second + 1;
        for (const auto& c : GameState::GetGameState().GetObjectsOfType<Tag>())
        {
            if (c.index == index)
                return c.GetGameObject();
        }

        return Handle<GameObject>();
    }

    vector<Handle<GameObject>> TagSystem::FindAll(string_view tag)
    {
        vector<Handle<GameObject>> vec;

        auto iter = _tags_to_indices.find(tag);
        if (iter == _tags_to_indices.end())
            return vec;

        auto index = iter->second + 1;
        for (const auto& c : GameState::GetGameState().GetObjectsOfType<Tag>())
        {
            if (c.index == index)
                vec.push_back(c.GetGameObject());
        }

        return vec;
    }

    string_view TagSystem::GetTagFromIndex(tag_t index)
    {
        if (index > GetConfig().tags.size())
            return "";
        return GetConfig().tags[index - 1];
    }

    TagSystem::tag_t TagSystem::GetIndexFromTag(string_view tag)
    {
        auto iter = _tags_to_indices.find(tag);
        if (iter == _tags_to_indices.end())
            return 0;
        else
            return iter->second + 1;
    }



    void TagSystem::ApplyConfig(const Config& config)
    {
        Config copy = config;

        if (copy.tags.size() > max_tags)
        {
            copy.tags.resize(max_tags);
            std::cout << "[Warning] Cannot have more than " << max_tags << " tags. Tags will be truncated." << std::endl;
            SetConfig(copy);
            return;
        }

        bool changed = false;

        _tags_to_indices.clear();
        for (size_t i = 0; i < copy.tags.size(); ++i)
        {
            if (copy.tags[i].empty())
            {
                copy.tags.erase(copy.tags.begin() + i);
                changed = true;
                continue;
            }

            // has duplicate tag
            if (_tags_to_indices.find(copy.tags[i]) != _tags_to_indices.end())
            {
                string tag = copy.tags[i];
                int counter = 0;
                copy.tags[i] = "";
                while (std::find(copy.tags.begin(), copy.tags.end(), tag + serialize_text(counter)) != copy.tags.end())
                    ++counter;
                copy.tags[i] = tag + serialize_text(counter);
                changed = true;
            }

            _tags_to_indices.emplace(copy.tags[i], static_cast<tag_t>(i));
        }

        if (changed)
            SetConfig(copy);
    }

}