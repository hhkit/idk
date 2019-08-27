#pragma once

namespace idk
{
	template<typename Meta>
	struct MetaTag
	{
		using Metadata = Meta;
		Meta meta;
		void SetMeta(const Meta& inmeta)
		{
			meta = inmeta;
			OnMetaUpdate();
		}
	protected:
		virtual void OnMetaUpdate() {};
	};
}