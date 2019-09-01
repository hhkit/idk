#include "stdafx.h"
#include <sstream>
#include <IncludeResources.h>
#include <serialize/serialize.h>

namespace idk
{
	template<typename T>
	struct FileResourceHelper;

	template<typename ... Rs>
	struct FileResourceHelper<std::tuple<Rs...>>
	{
		static auto GetResourceIDJumpTable()
		{
			return hash_table<string_view, size_t>
			{
				{ reflect::detail::pretty_function_name<Rs>(), ResourceID<Rs>} ...
			};
		}

		static auto RecreateMetaJumpTable()
		{
			using FnPtr = std::shared_ptr<void>(*)(string_view);
			return array<FnPtr, ResourceCount>{
				[]([[maybe_unused]] string_view data) -> std::shared_ptr<void>
				{
					if constexpr (has_tag_v<Rs, MetaTag>)
					{
						auto metadata = std::make_shared<typename Rs::Metadata>();
						parse_text(data.data(), *metadata);
						return metadata;
					}
					else
						return nullptr;
				}...
			};
		}

		static auto GenResourceIDJumpTable()
		{
			using FnPtr = string_view(*)();
			return array<FnPtr, ResourceCount>{
				[]() -> string_view
				{
					return reflect::detail::pretty_function_name<Rs>();
				} ...
			};
		}

		static auto GenSerializeJumpTable()
		{
			using FnPtr = opt<string>(*)(const std::shared_ptr<void> & ptr);
			return array<FnPtr, ResourceCount>{
				[]([[maybe_unused]] const std::shared_ptr<void>& ptr) -> opt<string>
			{
				if constexpr (has_tag_v<Rs, MetaTag>)
					return serialize_text((*r_cast<typename Rs::Metadata*>(ptr.get())));
				else
					return std::nullopt;
			} ...
			};
		}
	};

	GenericRscHandle::operator bool() const
	{
		return std::visit([](auto handle) { return static_cast<bool>(handle); }, _handle);
	}

	MetaFile save_meta(const FileResources& resources)
	{
		auto retval = MetaFile{};

		for (auto& handle : resources.resources)
		{
			handle.visit([&](auto&& h)
			{
				retval.guids.emplace_back(h.guid);
				if constexpr (has_tag_v<typename std::decay_t<decltype(h)>::Resource, MetaTag>)
					retval.resource_metas.emplace_back(h->GetMeta());
				else
					retval.resource_metas.emplace_back(reflect::dynamic{});

			});
		}

		return retval;
	}
}
