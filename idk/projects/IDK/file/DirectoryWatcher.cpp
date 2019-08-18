#include "stdafx.h"
#include "DirectoryWatcher.h"
#include "FileSystem.h"
#include "core/Core.h"

#include <iostream>
#include <filesystem>

namespace FS = std::filesystem;

namespace idk
{
	void file_system_detail::DirectoryWatcher::WatchDirectory(fs_dir& mountSubDir)
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

	void file_system_detail::DirectoryWatcher::UpdateWatchedDir(fs_mount& mount, fs_dir& dir)
	{
		UNREFERENCED_PARAMETER(mount);

		// false = returns when any one of the handles signal
		// 0     = No waiting, function immediately returns
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

	void file_system_detail::DirectoryWatcher::RefreshDir(file_system_detail::fs_dir& mountDir)
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
		//If number of files are the same, it means that files were renamed/replaced
		else
		{
			// In the case of replacement of files, we actually want to send a file written to notification
			CheckFilesRenamed(mountDir);
		}

	}

	void file_system_detail::DirectoryWatcher::RefreshTree(file_system_detail::fs_dir& mountDir)
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

	void file_system_detail::DirectoryWatcher::ResolveAllChanges()
	{
		auto& vfs = Core::GetSystem<FileSystem>();

		// Resolve changed files
		for (auto& file_index : changed_files)
		{
			auto& internal_file = vfs.getFile(file_index);

			// Check if there are even mounts. If this hits, something is terribly wrong...
			if (vfs.mounts.empty())
				throw("Something is terribly wrong. No mounts found.");

			auto& internal_collated = vfs.mounts[internal_file.tree_index.mount_id].path_tree[internal_file.tree_index.depth];
			auto& parent_dir = vfs.getDir(internal_file.parent);
			
			switch (internal_file.change_status)
			{
			case CREATED:
				// If a file was created, we need to create a suitable handle for it.
				// Since this was already when it was actually checked, we dont need to do it here.
				break;
			case DELETED:
			{
				// Most troublesome one. 
				// Need to invalidate the fs_file_handle and all FileHandles pointing to it.

				// First we remove this file from the parent dir.
				auto result = parent_dir.files_map.find(internal_file.filename);
				parent_dir.files_map.erase(result);

				// Next, invalidate it from path_tree. Should be a better way to do this. Not sure yet.
				// When we create files in run-time, we will loop through all the fs_file and find the first valid one if available. Linear time POG.
				internal_collated.files[internal_file.tree_index.index].valid = false;

				// Lastly, we invalidate the file handle that this file was using.
				// Remember that we DO NOT erase the fs_file_handle in FileSyste::file_handles. We simply invalidate it.
				vfs.file_handles[internal_file.handle_index].Invalidate();
				break;
			}
			case RENAMED:
				// Very straightforward. Since only the name was changed, don't have to do anything here I think.
				break;
			default:
				break;
			}

			internal_file.change_status = NO_CHANGE;
		}

		changed_files.clear();
		changed_dirs.clear();
	}

	void file_system_detail::DirectoryWatcher::CheckFilesCreated(file_system_detail::fs_dir& mountDir)
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
				// Request a slot from mounts

				// Check if there are even mounts. If this hits, something is terribly wrong...
				if (vfs.mounts.empty())
					throw("Something is terribly wrong. No mounts found.");

				fs_key slot = vfs.mounts[mountDir.tree_index.mount_id].RequestFileSlot(mountDir.tree_index.depth + 1);
				fs_file& f = vfs.getFile(slot);

				f.full_path = tmp.string();
				f.filename = tmp.filename().string();
				f.extension = tmp.extension().string();

				f.parent = mountDir.tree_index;

				f.time = FS::last_write_time(tmp);

				// Adding the new file to all relevant data structures
				mountDir.files_map.emplace(f.filename, f.tree_index);
				changed_files.push_back(f.tree_index);

				auto& test = vfs.getFile(f.tree_index);
				std::cout << "[FILE SYSTEM] File Created: " << "\n"
					"\t Path: " << f.full_path << "\n" <<
					"\t Test: " << test.full_path << "\n" <<  std::endl;
				break;
			}
		}
	}

	void file_system_detail::DirectoryWatcher::CheckFilesDeleted(file_system_detail::fs_dir& dir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		for (auto& file : dir.files_map)
		{
			auto& internal_file = vfs.getFile(file.second);
			if (!vfs.ExistsFull(internal_file.full_path))
			{
				// Might need to do more here depending on what we need.
				internal_file.change_status = DELETED;
				
				// internal_file.handle_index = -1;
				// internal_file.full_path.clear();

				changed_files.push_back(internal_file.tree_index);

				std::cout << "[FILE SYSTEM] File Deleted: " << "\n"
					"\t Path: " << internal_file.full_path << "\n" << std::endl;
			}
		}

	}

	void file_system_detail::DirectoryWatcher::CheckFilesRenamed(file_system_detail::fs_dir& mountDir)
	{
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
				// File was really renamed.
				// Now we need to find the corresponding internal fs_file that was renamed
				// One of the fs_file inside the dir does not exists anymore due to being renamed.
				auto& vfs = Core::GetSystem<FileSystem>();
				for (auto& internal_file_index : mountDir.files_map)
				{
					auto& internal_file = vfs.getFile(internal_file_index.second);
					if (!vfs.ExistsFull(internal_file.full_path))
					{
						std::cout << "[FILE SYSTEM] File Renamed: " << "\n"
							"\t Prev Path: " << internal_file.full_path << "\n"
							"\t Curr Path: " << tmp.string() << "\n" << std::endl;

						// This is the file that was renamed.
						internal_file.full_path = tmp.string();
						internal_file.filename = filename;
						internal_file.extension = tmp.extension().string();
						internal_file.change_status = RENAMED;

						changed_files.push_back(internal_file.tree_index);
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

	void file_system_detail::DirectoryWatcher::CheckFilesWrittenTo(file_system_detail::fs_dir& dir)
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
					"\t Path: " << internal_file.full_path << "\n" << std::endl;

				internal_file.time = current_file_last_write_time;

				changed_files.push_back(internal_file.tree_index);
			}
		}
	}

	
}