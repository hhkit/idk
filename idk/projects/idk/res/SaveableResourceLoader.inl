#pragma once
#include "SaveableResourceLoader.h"
#include <core/Core.h>
#include <res/MetaBundle.inl>
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
	inline ResourceBundle SaveableResourceLoader<Res>::LoadFile(RscHandle<Res> res,PathHandle p, const MetaBundle& bundle)
	{
		auto stream = p.Open(FS_PERMISSIONS::READ);
		parse_text(stringify(stream), *res);

		if constexpr (has_tag_v<Res, MetaTag>)
		{
			auto ser_meta = bundle.FetchMeta<Res>();
			auto real_meta = ser_meta->GetMeta<Res>();
			if (real_meta)
				res->SetMeta(*real_meta);
		}

		return res;
	}

	template<typename Res>
	inline ResourceBundle SaveableResourceLoader<Res>::LoadFile(PathHandle p, const MetaBundle& bundle)
	{
		const auto res = bundle.CreateResource<Res>();

		LoadFile(res, p, bundle);

		return res;
	}
	template<typename BaseRes, typename DerivedRes>
	inline ResourceBundle EasySaveableResourceLoader<BaseRes, DerivedRes>::LoadFile(PathHandle p, const MetaBundle& bundle)
	{
		const auto res = bundle.CreateResource<DerivedRes>();

		SaveableResourceLoader<BaseRes>::LoadFile(RscHandle<BaseRes>{res}, p, bundle);

		return res;
	}
}