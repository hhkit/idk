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

	GenericMetadata::GenericMetadata(string_view serialized)
	{
		char guidbuf[64];
		char typebuf[64];

		sscanf_s(serialized.data(), "%s%s", guidbuf, 64, typebuf, 64);

		string_view rest = serialized.substr(strlen(guidbuf) + 1 + strlen(typebuf) + 1);
		static const auto get_resource_id_ht = FileResourceHelper<Resources>::GetResourceIDJumpTable();
		static const auto recreate_meta_jt = FileResourceHelper<Resources>::RecreateMetaJumpTable();

		auto find = get_resource_id_ht.find(typebuf);
		if (find != get_resource_id_ht.end())
		{
			resource_id = find->second;
			pimpl = recreate_meta_jt[find->second](rest);
		}
	}

	GenericMetadata::operator string() const
	{
		static const auto name_jump_table = FileResourceHelper<Resources>::GenResourceIDJumpTable();
		static const auto ser_jump_table = FileResourceHelper<Resources>::GenSerializeJumpTable();
		std::string construct;
		construct += string{ guid };                   construct += "\n";
		construct += name_jump_table[resource_id]();   construct += "\n";
		auto str2 = ser_jump_table[resource_id](pimpl);
		if (str2)
			construct += *str2;

		return construct;
	}
}
