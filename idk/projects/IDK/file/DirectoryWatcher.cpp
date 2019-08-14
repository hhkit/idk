#include "stdafx.h"
#include "DirectoryWatcher.h"
#include "FileSystem.h"
#include "core/Core.h"

#include <iostream>
#include <filesystem>

namespace FS = std::filesystem;

namespace idk
{
	void file_system_internal::DirectoryWatcher::WatchDirectory(dir_t& mountSubDir)
	{
		// start watching this folder
		mountSubDir.watch_handle[0] = FindFirstChangeNotificationA(
			mountSubDir.full_path.c_str(),
			FALSE,
			FILE_NOTIFY_CHANGE_DIR_NAME);

		if (mountSubDir.watch_handle[0] == INVALID_HANDLE_VALUE)
		{
			std::cout << "\n ERROR: FindFirstChangeNotification FOLDER failed.\n"
				<< mountSubDir.full_path << std::endl;
		}

		mountSubDir.watch_handle[1] = FindFirstChangeNotificationA(
			mountSubDir.full_path.c_str(),
			FALSE,
			FILE_NOTIFY_CHANGE_FILE_NAME);

		if (mountSubDir.watch_handle[1] == INVALID_HANDLE_VALUE)
		{
			std::cout << "\n ERROR: FindFirstChangeNotification FILE failed.\n"
				<< mountSubDir.full_path << std::endl;
		}

		mountSubDir.watch_handle[2] = FindFirstChangeNotificationA(
			mountSubDir.full_path.c_str(),
			FALSE,
			FILE_NOTIFY_CHANGE_LAST_WRITE);

		if (mountSubDir.watch_handle[2] == INVALID_HANDLE_VALUE)
		{
			std::cout << "\n ERROR: FindFirstChangeNotification FILE failed.\n"
				<< mountSubDir.full_path << std::endl;
		}
	}

	void file_system_internal::DirectoryWatcher::UpdateWatchedDir(mount_t& mount, dir_t& dir)
	{
		dir.status = WaitForMultipleObjects(3, dir.watch_handle, false, 0);

		switch (dir.status)
		{
		case WAIT_OBJECT_0:
		{
			// DIRECTORY WAS CREATED OR RENAMED
			std::cout << "[FILE SYSTEM] Directory change detected in: " << dir.full_path << std::endl;

			if (FindNextChangeNotification(dir.watch_handle[0]) == FALSE) {
				std::printf("\n ERROR: FindNextChangeNotification function failed.\n");
			}
			break;
		}
		case WAIT_OBJECT_0 + 1:
		{
			// FILE WAS CREATED OR RENAMED
			std::cout << "[FILE SYSTEM] File change detected in: " << dir.full_path << std::endl;

			if (FindNextChangeNotification(dir.watch_handle[1]) == FALSE) {
				std::printf("\n ERROR: FindNextChangeNotification function failed.\n");
			}
			break;
		}
		case WAIT_OBJECT_0 + 2:
		{
			// File was written to recently
			for (auto& file : FS::directory_iterator(dir.full_path))
			{
				auto current_file_last_write_time = FS::last_write_time(file);
				FS::path tmp{ file.path() };

				// We only check if it is a regular file
				if (!FS::is_regular_file(tmp))
					continue;

				string filename = tmp.filename().string();

				auto result = dir.files_map.find(filename);
				if (result == dir.files_map.end())
				{
					// This means that we either entered into here erronously or some other bug.
					std::cout << "[FILE SYSTEM] FILE_NOTIFY_CHANGE_LAST_WRITTEN WAS IGNORED." << std::endl;
					break;
				}

				auto& internal_file = mount.GetFile(result->second);
				if (current_file_last_write_time != internal_file.time)
				{
					std::cout << "[FILE SYSTEM] File written detected in: " << internal_file.full_path << std::endl;
					internal_file.time = current_file_last_write_time;
				}
			}

			if (FindNextChangeNotification(dir.watch_handle[2]) == FALSE) {
				std::printf("\n ERROR: FindNextChangeNotification function failed.\n");
			}
			break;
		}
		case WAIT_TIMEOUT:
			break;
		default:
		{
			assert(false);
			break;
		}
		}
	}

	void file_system_internal::DirectoryWatcher::RefreshDir(file_system_internal::dir_t& mountDir)
	{
		vector<FS::path> files_changed;
		
		// auto& fs = Core::GetSystem<FileSystem>();

		FS::path path{ mountDir.full_path };
		FS::directory_iterator dir{ path };

		size_t num_files = 0;
		for (auto& elem : dir)
		{
			FS::path p{ elem.path() };
			if (FS::is_regular_file(p))
			{
				auto result = mountDir.files_map.find(p.filename().string());
				if (result == mountDir.files_map.end())
				{
					files_changed.push_back(p);
				}
				++num_files;
			}
		}

	
	}

	void file_system_internal::DirectoryWatcher::RefreshTree(file_system_internal::dir_t& dir)
	{
		UNREFERENCED_PARAMETER(dir);
	}

	void file_system_internal::DirectoryWatcher::CheckFilesDeleted(file_system_internal::dir_t& dir)
	{
		auto& fs = Core::GetSystem<FileSystem>();
		for (auto& file : dir.files_map)
		{
			if (!fs.ExistsFull(dir.full_path + file.first))
			{
				auto& internal_file = fs.getFile(file.second);
				internal_file.change_status = DELETED;
				internal_file.handle_index = -1;
				internal_file.full_path.clear();
			}
		}

	}

	// file_system_internal::file_t& file_system_internal::DirectoryWatcher::addInternalFile(string_view full_path)
	// {
	// 	UNREFERENCED_PARAMETER(full_path);
	// 	return file_system_internal::file_t{};
	// }
}