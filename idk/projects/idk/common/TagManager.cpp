#include "stdafx.h"
#include "TagManager.h"
#include <common/Tag.h>
#include <core/GameObject.h>
#include <iostream>
#include <serialize/text.h>
#include <ds/span.inl>

namespace idk
{

    void TagManager::Init()
    {
        GameState::GetGameState().OnObjectCreate<Tag>() += [](Handle<Tag> tag)
        {
            if (tag->index == 0)
                tag->GetGameObject()->RemoveComponent(tag);
        };
    }



    Handle<GameObject> TagManager::Find(string_view tag) const
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

    vector<Handle<GameObject>> TagManager::FindAll(string_view tag) const
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

    string_view TagManager::GetTagFromIndex(tag_t index) const
    {
        if (index > GetConfig().tags.size())
            return "";
        return GetConfig().tags[index - 1];
    }

    TagManager::tag_t TagManager::GetIndexFromTag(string_view tag) const
    {
        auto iter = _tags_to_indices.find(tag);
        if (iter == _tags_to_indices.end())
            return 0;
        else
            return iter->second + 1;
    }

    size_t TagManager::GetNumOfTags() const
    {
        return GetConfig().tags.size();
    }



    void TagManager::ApplyConfig(Config& config)
    {
        if (config.tags.size() > max_tags)
        {
            config.tags.resize(max_tags);
            std::cout << "[Warning] Cannot have more than " << max_tags << " tags." << std::endl;
        }

        _tags_to_indices.clear();
        for (size_t i = 0; i < config.tags.size(); ++i)
        {
            if (config.tags[i].empty())
            {
                config.tags.erase(config.tags.begin() + i);
                continue;
            }

            // has duplicate tag
            if (_tags_to_indices.find(config.tags[i]) != _tags_to_indices.end())
            {
                string tag = config.tags[i];
                int counter = 0;
                config.tags[i] = "";
                while (std::find(config.tags.begin(), config.tags.end(), tag + serialize_text(counter)) != config.tags.end())
                    ++counter;
                config.tags[i] = tag + serialize_text(counter);
            }

            _tags_to_indices.emplace(config.tags[i], static_cast<tag_t>(i));
        }
    }

}