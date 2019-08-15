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
		// Initialize watch handles
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
		UNREFERENCED_PARAMETER(mount);

		dir.status = WaitForMultipleObjects(3, dir.watch_handle, false, 0);

		switch (dir.status)
		{
		case WAIT_OBJECT_0:
		{
			// Directory Created/Deleted/Renamed/Replaced
			RefreshTree(dir);

			if (FindNextChangeNotification(dir.watch_handle[0]) == FALSE) {
				std::printf("\n ERROR: FindNextChangeNotification function failed.\n");
			}
			break;
		}
		case WAIT_OBJECT_0 + 1:
		{
			// File Created/Deleted/Renamed/Replaced
			RefreshDir(dir);

			if (FindNextChangeNotification(dir.watch_handle[1]) == FALSE) {
				std::printf("\n ERROR: FindNextChangeNotification function failed.\n");
			}
			break;
		}
		case WAIT_OBJECT_0 + 2:
		{
			// File was written to recently
			CheckFilesWrittenTo(dir);

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
		FS::path path{ mountDir.full_path };
		
		auto num_files = static_cast<size_t>(std::count_if(FS::directory_iterator{ path }, FS::directory_iterator{},
			static_cast<bool (*)(const FS::path&)>(FS::is_regular_file)));

		// Files were created
		if (num_files > mountDir.files_map.size())
		{
			CheckFilesCreated(mountDir);
		}
		// Files were deleted
		else if (num_files < mountDir.files_map.size())
		{
			CheckFilesDeleted(mountDir);
		}
		//If neither, it means that files were renamed/replaced
		else
		{
			// In the case of replacement of files, we actually want to send a file written to notification
			CheckFilesRenamed(mountDir);
		}

	}

	void file_system_internal::DirectoryWatcher::RefreshTree(file_system_internal::dir_t& mountDir)
	{
		FS::path path{ mountDir.full_path };
		FS::directory_iterator dir{ path };

		auto num_files = static_cast<size_t>(std::count_if(FS::directory_iterator{ path }, FS::directory_iterator{},
			[](const FS::path& p) -> bool
		{
			return !FS::is_regular_file(p);
		}));

		// Files were created
		if (num_files > mountDir.sub_dirs.size())
		{
			std::cout << "[FILE SYSTEM] Files Created" << std::endl;
			
		}
		// Files were deleted
		else if (num_files < mountDir.sub_dirs.size())
		{
			std::cout << "[FILE SYSTEM] Files Deleted" << std::endl;
			
		}
		//If neither, it means that files were renamed/replaced
		else
		{
			// In the case of replacement of files, we actually want to send a file written to notification
			std::cout << "[FILE SYSTEM] Files Renamed" << std::endl;
		}
	}

	void file_system_internal::DirectoryWatcher::CheckFilesCreated(file_system_internal::dir_t& mountDir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		for (auto& file : FS::directory_iterator(mountDir.full_path))
		{
			FS::path tmp{ file.path() };

			// We only check if it is a regular file
			if (!FS::is_regular_file(tmp))
				continue;

			string filename = tmp.filename().string();

			auto result = mountDir.files_map.find(filename);
			if (result == mountDir.files_map.end())
			{
				file_system_internal::file_t f;

				f.full_path = tmp.string();
				f.filename = tmp.filename().string();
				f.extension = tmp.extension().string();

				file_system_internal::node_t node;
				node.mount_id = static_cast<int8_t>(mountDir.tree_index.mount_id);
				node.depth = mountDir.tree_index.depth + 1;
				node.index = static_cast<int8_t>(vfs.mounts[node.mount_id].path_tree[node.depth].files.size());

				f.handle_index = vfs.file_handles.size();
				vfs.file_handles.emplace_back(node);

				f.tree_index = node;

				f.time = FS::last_write_time(tmp);

				mountDir.files_map.emplace(f.filename, node);

				vfs.mounts[node.mount_id].path_tree[node.depth].files.emplace_back(f);

				auto& test = vfs.getFile(f.tree_index);
				std::cout << "[FILE SYSTEM] File Created: " << "\n"
					"\t Path: " << f.full_path << "\n" <<
					"\t Test: " << test.full_path << "\n" << std::endl;
				break;
			}
		}
	}

	void file_system_internal::DirectoryWatcher::CheckFilesDeleted(file_system_internal::dir_t& dir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		for (auto& file : dir.files_map)
		{
			auto& internal_file = vfs.getFile(file.second);
			if (!vfs.ExistsFull(internal_file.full_path))
			{
				internal_file.change_status = DELETED;
				// internal_file.handle_index = -1;
				// internal_file.full_path.clear();

				std::cout << "[FILE SYSTEM] File Deleted: " << "\n"
					"\t Path: " << internal_file.full_path << "\n" << std::endl;
			}
		}

	}

	void file_system_internal::DirectoryWatcher::CheckFilesRenamed(file_system_internal::dir_t& mountDir)
	{
		for (auto& file : FS::directory_iterator(mountDir.full_path))
		{
			auto current_file_last_write_time = FS::last_write_time(file);
			FS::path tmp{ file.path() };

			// We only check if it is a regular file
			if (!FS::is_regular_file(tmp))
				continue;

			string filename = tmp.filename().string();

			auto result = mountDir.files_map.find(filename);
			if (result == mountDir.files_map.end())
			{
				// File was really renamed.
				// Now we need to find the corresponding internal file_t that was renamed
				// One of the file_t inside the dir does not exists anymore due to being renamed.
				auto& vfs = Core::GetSystem<FileSystem>();
				for (auto& internal_file_index : mountDir.files_map)
				{
					auto& internal_file = vfs.getFile(internal_file_index.second);
					if (!vfs.ExistsFull(internal_file.full_path))
					{
						std::cout << "[FILE SYSTEM] File Renamed: " << "\n"
							"\t Prev Path: " << internal_file.full_path << "\n"
							"\t Curr Path: " << tmp.string() << "\n";

						// This is the file that was renamed.
						internal_file.full_path = tmp.string();
						internal_file.filename = filename;
						internal_file.extension = tmp.extension().string();
						internal_file.change_status = RENAMED;

						

						return;
					}
				}
			}
		}

		// std::cout << "[FILE SYSTEM] Cannot find renamed file. Looking for replaced file." << std::endl;
		// If we cannot find the corresponding file or if the directory looks exactly the same, 
		// it means that a file has been replaced. In which case, we want to check for write.
		CheckFilesWrittenTo(mountDir);
	}

	void file_system_internal::DirectoryWatcher::CheckFilesWrittenTo(file_system_internal::dir_t& dir)
	{
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

			auto& internal_file = Core::GetSystem<FileSystem>().getFile(result->second);
			if (current_file_last_write_time != internal_file.time)
			{
				std::cout << "[FILE SYSTEM] File Written To: " << "\n"
					"\t Path: " << internal_file.full_path << "\n";

				internal_file.time = current_file_last_write_time;
			}
		}
	}

	
}