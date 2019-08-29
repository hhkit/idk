#include "stdafx.h"
#include <IncludeResources.h>
#include <serialize/serialize.h>

namespace idk
{
	template<typename T>
	struct FileResourceHelper;

	template<typename ... Rs>
	struct FileResourceHelper<std::tuple<Rs...>>
	{
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

	vector<GenericMetadata> save_meta(const FileResources& resources)
	{
		auto retval = vector<GenericMetadata>();

		for (auto& handle : resources.resources)
		{
			handle.visit([&](auto&& h)
			{
				retval.emplace_back(*h);
			});
		}

		return retval;
	}

	GenericMetadata::operator string() const
	{
		static auto jump_table = FileResourceHelper<Resources>::GenSerializeJumpTable();
		std::string construct{guid};
		auto str1 = serialize_text(resource_id);
		auto str2 = jump_table[resource_id](pimpl);
		
		return construct;
	}
}
