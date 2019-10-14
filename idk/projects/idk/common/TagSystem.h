#pragma once

#include <core/ConfigurableSystem.h>
#include <common/Tag.h>

namespace idk
{
    struct TagSystemConfig
    {
        vector<string> tags;
    };

    class TagSystem : public ConfigurableSystem<TagSystemConfig>
    {
    public:
        using tag_t = decltype(Tag::index);
        constexpr static size_t max_tags = std::numeric_limits<tag_t>::max();

        virtual void Init() {};
        virtual void Shutdown() {};

        Handle<GameObject> Find(string_view tag);
        vector<Handle<GameObject>> FindAll(string_view tag);
        string_view GetTagFromIndex(tag_t index);
        tag_t GetIndexFromTag(string_view tag);

    protected:
        virtual void ApplyConfig(Config& config);

    private:
        hash_table<string_view, tag_t> _tags_to_indices;
    };
}