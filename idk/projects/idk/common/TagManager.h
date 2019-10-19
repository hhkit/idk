#pragma once

#include <core/ConfigurableSystem.h>
#include <common/Tag.h>

namespace idk
{
    struct TagManagerConfig
    {
        vector<string> tags;
    };

    class TagManager : public ConfigurableSystem<TagManagerConfig>
    {
    public:
        using tag_t = decltype(Tag::index);
        constexpr static size_t max_tags = std::numeric_limits<tag_t>::max();

        virtual void Init();
        virtual void Shutdown() {};

        Handle<GameObject> Find(string_view tag) const;
        vector<Handle<GameObject>> FindAll(string_view tag) const;
        string_view GetTagFromIndex(tag_t index) const;
        tag_t GetIndexFromTag(string_view tag) const;
        size_t GetNumOfTags() const;

    protected:
        virtual void ApplyConfig(Config& config);

    private:
        hash_table<string_view, tag_t> _tags_to_indices;
    };
}