#pragma once

#include <core/Core.h>
#include <res/MetaBundle.h>
#include <res/ResourceManager.h>
#include <serialize/serialize.h>
#include <util/ioutils.h>

namespace idk
{
	template<typename Res>
	inline ResourceBundle SaveableResourceLoader<Res>::LoadFile(PathHandle p)
	{
		const auto res = Core::GetResourceManager().LoaderEmplaceResource<Res>();
		auto stream = p.Open(FS_PERMISSIONS::READ);
		parse_text(stringify(stream), *res);
		res->Dirty();
		return res;
	}

	template<typename Res>
	inline ResourceBundle SaveableResourceLoader<Res>::LoadFile(PathHandle p, const MetaBundle& bundle)
	{
		if (bundle.metadatas.size())
		{
			const auto res = Core::GetResourceManager().LoaderEmplaceResource<Res>(bundle.metadatas[0].guid);
			auto stream = p.Open(FS_PERMISSIONS::READ);
			parse_text(stringify(stream), *res);
			if constexpr (has_tag_v<Res, MetaTag>)
			{
				auto meta = bundle.metadatas[0].GetMeta<Res>();
				if (meta)
					res->SetMeta(*meta);
			}

			return res;
		}
		else
			return LoadFile(p);
	}
}