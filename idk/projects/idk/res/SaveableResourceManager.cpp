#include "stdafx.h"
#include "SaveableResourceManager.h"
#include <reflect/reflect.h>
#include <serialize/serialize.h>
#include <IncludeResources.h>

namespace idk
{
	namespace detail
	{
		template<typename T>
		struct tSavedHelper;

		template<typename ... Rs>
		struct tSavedHelper<std::tuple<Rs...>>
		{
			static auto GenerateDirtyCheckJT()
			{
				using FnPtr = bool(*)(Guid);
				return array<FnPtr, ResourceCount>
				{
					[](Guid guid) -> bool
					{
						if constexpr (has_tag_v<Rs, Saveable>)
						{
							RscHandle<Rs> handle{ guid };
							assert(handle);
							return handle->_dirty;
						}
						else
						{
							UNREFERENCED_PARAMETER(guid);
							assert(false);
							return false;
						}
					} ...
				};
			}

			static auto GenerateAutosaveJT()
			{
				using FnPtr = bool (*)();
				return array<FnPtr, ResourceCount>
				{
					[]() -> bool
					{
						if constexpr (has_tag_v<Rs, Saveable>)
							return Rs::autosave;
						else
						{
							assert(false);
							return false;
						}
					} ...
				};
			}

			static auto GenerateExtensionJT()
			{
				using FnPtr = string_view (*)();
				return array<FnPtr, ResourceCount>
				{
					[]() -> string_view
					{
						if constexpr (has_tag_v<Rs, Saveable>)
						{
							static_assert(Rs::ext.size(), "Must specialize template ResourceExtension<> for T before definition of T.");
							return Rs::ext;
						}
						else
						{
							assert(false);
							return "";
						}
					} ...
				};
			}

			static auto GenerateNameGetJT()
			{
				using FnPtr = string* (*)(Guid);
				return array<FnPtr, ResourceCount>
				{
					[](Guid guid) -> string*
					{
						if constexpr (has_tag_v<Rs, Saveable>)
						{
							RscHandle<Rs> handle{ guid };
							assert(handle);
							return &handle->name;
						}
						else
						{
							UNREFERENCED_PARAMETER(guid);
							assert(false);
							return nullptr;
						}
					} ...
				};
			}

			static auto GenerateSaveFileJT()
			{
				using FnPtr = void(*)(Guid, FileHandle);
				return array<FnPtr, ResourceCount>
				{
					[](Guid guid, FileHandle f)
					{
						if constexpr (has_tag_v<Rs, Saveable>)
						{
							RscHandle<Rs> handle{ guid };
							if (handle)
							{
								f.Open(FS_PERMISSIONS::WRITE) << serialize_text(reflect::dynamic{ *handle });
								handle->Clean();
							}
						}
						else
						{
							UNREFERENCED_PARAMETER(guid);
							UNREFERENCED_PARAMETER(f);
						}
					} ...
				};
			}

			static auto GenerateAssociateJT()
			{
				using FnPtr = void(*)(SaveableResourceManager *, Guid, string_view);
				return array<FnPtr, ResourceCount>
				{
					[](SaveableResourceManager* man, Guid guid, string_view path)
					{
						if constexpr (has_tag_v<Rs, Saveable>)
							man->Associate(RscHandle<Rs>{ guid }, path);
						else
						{
							UNREFERENCED_PARAMETER(man);
							UNREFERENCED_PARAMETER(guid);
							UNREFERENCED_PARAMETER(path);
						}
					} ...
				};
			}
		};

		using SavedHelper = tSavedHelper<Resources>;
	}


	void SaveableResourceManager::SaveDirtyFiles()
	{
		static const auto is_dirtyjt  = detail::SavedHelper::GenerateDirtyCheckJT();
		static const auto autosavejt  = detail::SavedHelper::GenerateAutosaveJT();
		static const auto extensionjt = detail::SavedHelper::GenerateExtensionJT();
		static const auto associatejt = detail::SavedHelper::GenerateAssociateJT();
		static const auto namegetjt   = detail::SavedHelper::GenerateNameGetJT();

		static_assert(!has_tag_v<Texture, Saveable>, "lol");

		for (auto& [guid, control_block]: control_blocks)
		{
			auto index = control_block.resource_id;

			if (!is_dirtyjt[index](guid))
				continue;

			if (!autosavejt[index]()) // do not autosave
				continue;

			// generate unique filename
			if (!control_block.associated_file)
			{
				auto& name = *namegetjt[index](guid);
				auto ext = extensionjt[index]();
				while (Core::GetSystem<FileSystem>().Exists("/assets/" + string{ name } +ext.data()))
				{
					auto find = name.find("(copy");
					if (find == string::npos)
						name += " (copy)";
					else
					{
						string_view substr = string_view(name).substr(find);
						int copy_num = 2;
						if (sscanf_s(substr.data(), "(copy %d)", &copy_num) == 0)
							name.replace(find, string::npos, "(copy 2)");
						else
						{
							char buf[24];
							sprintf_s(buf, "(copy %d)", copy_num + 1);
							name.replace(find, string::npos, buf);
						}
					}
				}

				auto filepath = "/assets/" + string{ name } +ext.data();
				associatejt[index](this, guid, filepath);
			}

			Save(guid);
		}
	}

	void SaveableResourceManager::Init()
	{
		// register extension loaders
	}

	void SaveableResourceManager::Shutdown()
	{
	}

	SaveableResourceManager::SaveResourceResult SaveableResourceManager::Save(Guid guid)
	{
		auto control_block = control_blocks.find(guid);
		if (control_block == control_blocks.end())
			return SaveResourceResult::Err_HandleNotHandled;

		auto index = control_block->second.resource_id;
		static const auto save_filejt = detail::SavedHelper::GenerateSaveFileJT();
		auto& filepath = control_block->second.associated_file;
		if (!filepath)
			return SaveResourceResult::Err_ResourceNoAssociatedFile;

		save_filejt[index](guid, *filepath);
		return SaveResourceResult::Ok;
	}
}
