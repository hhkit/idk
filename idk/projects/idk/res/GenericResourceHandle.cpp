#include "stdafx.h"
#include <IncludeResources.h>
namespace idk
{
	namespace detail
	{
		template<typename T>
		struct GenericResourceLut;

		template<typename ... Ts>
		struct GenericResourceLut<std::tuple<Ts...>>
		{
			static constexpr auto GenHandleLUT()
			{
				return hash_table<string, GenericResourceHandle(*)(Guid guid)>
				{
					{string{ reflect::get_type<Ts>().name() }, [](Guid guid)->GenericResourceHandle { return RscHandle<Ts>{guid}; } }...
				};
			}
		};
	}
	GenericResourceHandle::GenericResourceHandle(Guid guid, string typehash)
	{
		static const auto LUT = detail::GenericResourceLut<Resources>::GenHandleLUT();
		auto itr = LUT.find(typehash == "ShaderGraph" ? "Material" : typehash);
		
		if (itr == LUT.end())
			return;
		*this = itr->second(guid);
	}
	Guid GenericResourceHandle::guid() const
	{
		return std::visit([](const auto& handle) { return handle.guid; }, *this);
	}

	size_t GenericResourceHandle::resource_id() const
	{
		return std::visit([](const auto& handle) 
			{
				return BaseResourceID<typename std::decay_t<decltype(handle)>::Resource>; 
			}, *this);
	}
}
