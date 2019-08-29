#pragma once

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
		Meta meta;
		virtual void OnMetaUpdate(const Meta& newmeta) { (newmeta); };
	};
}

#include "ResourceMeta.inl"