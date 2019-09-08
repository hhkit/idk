#pragma once

#include <file/FileSystem.h>
#include "SaveableResourceManager.h"

namespace idk
{
	template<typename T>
	inline void SaveableResourceManager::RegisterHandle(RscHandle<T> handle)
	{
		static_assert(has_tag_v<T, Saveable>, "T must be a saveable resource");
		assert(control_blocks.find(handle.guid) == control_blocks.end());
		control_blocks[handle.guid] = ControlBlock{ std::nullopt, ResourceID<T> };
	}
	template<typename T>
	inline void SaveableResourceManager::RegisterHandle(RscHandle<T> handle, FileHandle f)
	{
		static_assert(has_tag_v<T, Saveable>, "T must be a saveable resource");
		assert(control_blocks.find(handle.guid) == control_blocks.end());
		control_blocks[handle.guid] = ControlBlock{ f, ResourceID<T> };
		files.emplace(handle.guid, f.GetMountPath());
	}
	template<typename T>
	inline bool SaveableResourceManager::DeregisterHandle(RscHandle<T> handle)
	{
		auto cb_itr = control_blocks.find(handle.guid);
		if (cb_itr != control_blocks.end())
			control_blocks.erase(cb_itr);
		else
			return false;

		auto file_itr = files.find(handle.guid);
		if (file_itr != files.end())
			files.erase(file_itr);

		return true;
	}
	template<typename T>
	SaveableResourceManager::FileAssociateResult SaveableResourceManager::Associate(RscHandle<T> handle, string_view mountPath)
	{
		auto cb_itr = control_blocks.find(handle.guid);

		if (cb_itr == control_blocks.end())
			return FileAssociateResult::Err_HandleNotRegistered;

		if (Core::template GetSystem<FileSystem>().Exists(mountPath))
			return FileAssociateResult::Err_FileAlreadyExists;

		if (Core::template GetSystem<FileSystem>().Open(mountPath, FS_PERMISSIONS::WRITE))
		{
			cb_itr->second.associated_file = FileHandle{ mountPath };
			files.emplace(cb_itr->first, mountPath);
			return FileAssociateResult::Ok;
		}

		return FileAssociateResult::Err_FileCreationFailure;
	}
	template<typename T>
	SaveableResourceManager::SaveResourceResult SaveableResourceManager::Save(RscHandle<T> handle)
	{
		return Save(handle.guid);
	}
}