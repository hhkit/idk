#include "stdafx.h"

#include "GenericResourceHandle.h"
#include <res/ResourceUtils.inl>
#include <IncludeResources.h>
#include <reflect/reflect.inl>
namespace idk
{
	namespace detail
	{
		template<typename T> struct GenericResourceHelper;

		template<typename ... Ts>
		struct GenericResourceHelper<std::tuple<Ts...>>
		{
			static auto GenLUT()
			{
				auto backup_lut = hash_table < string_view, void(*)(GenericResourceHandle&, Guid)>
				{
					// "name", [](GenericResourceHandle& handle, Guid guid) { handle = RscHandle<>{ guid }; }
					{"Prefab", [](GenericResourceHandle& handle, Guid guid) { handle = RscHandle<Prefab>{ guid }; } },
					{"idk::anim::Animation", [](GenericResourceHandle& handle, Guid guid) { handle = RscHandle<anim::Animation>{ guid }; }},
					{"idk::anim::Skeleton", [](GenericResourceHandle& handle, Guid guid) { handle = RscHandle<anim::Skeleton>{ guid }; } }
				};
				
				backup_lut.merge(hash_table < string_view, void(*)(GenericResourceHandle&, Guid)>
				{
					{reflect::get_type<Ts>().name(), [](GenericResourceHandle& handle, Guid guid)
					{
						handle = RscHandle<Ts>{ guid };
					}}...
				});
				return backup_lut;
			}
		};

	}

	GenericResourceHandle::GenericResourceHandle(string_view type_name, Guid guid)
	{
		static const auto name_lut = detail::GenericResourceHelper<Resources>::GenLUT();

		auto itr = name_lut.find(type_name);
		if (itr != name_lut.end())
			itr->second(*this, guid);
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
