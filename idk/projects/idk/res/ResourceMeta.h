#pragma once
#include <idk.h>
#include <meta/comparator.h>
#include <res/Guid.h>
#include <res/Resource.h>

namespace idk
{
	// tags a resource with metadata of the type Meta
	// when SetMeta is called, OnMetaUpdate is called first before the meta is set.
	//   override for custom behavior
	// the Meta is the data that will be serialized when the resource is saved
	template<typename Res, typename Meta>
	struct MetaResource
		: Resource<Res>
	{
		using Metadata = Meta;
		Meta& GetMeta();
		void DirtyMeta();
	};

	struct SerializedMeta
		: comparable<SerializedMeta>
	{
		Guid   guid;
		string name;
		string t_hash{};
		string metadata;

		explicit SerializedMeta(Guid guid = Guid{}, string_view name = "", string_view t_hash = "", string_view metadata = "");

		template<typename Res, typename = sfinae<has_tag_v<Res, MetaResource>>>
		opt<typename Res::Metadata> GetMeta() const;

		bool operator<(const SerializedMeta&) const noexcept;
	};

}
