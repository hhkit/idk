#pragma once

#include <core/Core.h>
#include <res/MetaBundle.h>
#include <res/ResourceManager.h>
#include <serialize/text.h>
#include <util/ioutils.h>

namespace idk
{
	//template<typename Res>
	//inline ResourceBundle SaveableResourceLoader<Res>::LoadFile(PathHandle p)
	//{
	//	const auto res = Core::GetResourceManager().LoaderEmplaceResource<Res>();
	//	auto stream = p.Open(FS_PERMISSIONS::READ);
	//	parse_text(stringify(stream), *res);
	//	res->Dirty();
	//	return res;
	//}

	template<typename Res>
	inline ResourceBundle SaveableResourceLoader<Res>::LoadFile(PathHandle p, const MetaBundle& bundle)
	{
		auto meta = bundle.FetchMeta<Res>();

		const auto res = meta 
			? Core::GetResourceManager().LoaderEmplaceResource<Res>(meta->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<Res>();
		auto stream = p.Open(FS_PERMISSIONS::READ);
		parse_text(stringify(stream), *res);

		if constexpr (has_tag_v<Res, MetaTag>)
		{
			auto real_meta = meta->GetMeta<Res>();
			if (real_meta)
				res->SetMeta(*real_meta);
		}

		return res;
	}
}