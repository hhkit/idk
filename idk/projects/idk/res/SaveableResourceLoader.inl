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
	inline ResourceBundle SaveableResourceLoader<Res>::LoadFile(RscHandle<Res> res,PathHandle p, const MetaBundle&)
	{
		auto stream = p.Open(FS_PERMISSIONS::READ);
		parse_text(stringify(stream), *res);

		static_assert(!has_tag_v<Res, MetaResource>, "SaveableResourceLoader cannot use MetaTag");

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