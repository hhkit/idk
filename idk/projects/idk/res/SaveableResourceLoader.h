#pragma once
#include <idk.h>
#include <res/FileLoader.h>

namespace idk
{
	template<typename Res>
	class SaveableResourceLoader
		: public IFileLoader
	{
	public:
		using Resource = Res;

		static ResourceBundle LoadFile(RscHandle<Res> res,PathHandle p, const MetaBundle& bundle);
		ResourceBundle LoadFile(PathHandle p, const MetaBundle& bundle);
	};
	template<typename BaseRes,typename DerivedRes>
	class EasySaveableResourceLoader
		: public IFileLoader
	{
	public:
		using Resource = BaseRes;

		ResourceBundle LoadFile(PathHandle p, const MetaBundle& bundle);
	};
}

#include "SaveableResourceLoader.inl"