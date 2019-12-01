#pragma once
#include <idk.h>
#include <meta/comparator.h>

namespace idk
{
	// tags a resource with metadata of the type Meta
	// when SetMeta is called, OnMetaUpdate is called first before the meta is set.
	//   override for custom behavior
	// the Meta is the data that will be serialized when the resource is saved
	template<typename Meta>
	struct MetaTag
	{
		using Metadata = Meta;
		void SetMeta(const Meta& inmeta);
		const Meta& GetMeta() const;
	protected:
		Meta meta{};
		bool _dirtymeta { false };
		virtual void OnMetaUpdate(const Meta& newmeta) { (newmeta); };

		friend class ResourceManager;
	};

	struct SerializedMeta
		: comparable<SerializedMeta>
	{
		Guid   guid;
		string name;
		string t_hash{};
		string metadata;

		explicit SerializedMeta(Guid guid = Guid{}, string_view name = "", string_view t_hash = "", string_view metadata = "");

		template<typename Res, typename = sfinae<has_tag_v<Res, MetaTag>>>
		opt<typename Res::Metadata> GetMeta() const;

		bool operator<(const SerializedMeta&) const noexcept;
	};

}

#include "ResourceMeta.inl"