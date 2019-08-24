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
		mountSubDir._watch_handle[0] = FindFirstChangeNotificationA(
			mountSubDir._full_path.c_str(),
			FALSE,
			FILE_NOTIFY_CHANGE_DIR_NAME);

		if (mountSubDir._watch_handle[0] == INVALID_HANDLE_VALUE)
		{
			std::cout << "\n ERROR: FindFirstChangeNotification FOLDER failed.\n"
				<< mountSubDir._full_path << std::endl;
		}

		mountSubDir._watch_handle[1] = FindFirstChangeNotificationA(
			mountSubDir._full_path.c_str(),
			FALSE,
			FILE_NOTIFY_CHANGE_FILE_NAME);

		if (mountSubDir._watch_handle[1] == INVALID_HANDLE_VALUE)
		{
			std::cout << "\n ERROR: FindFirstChangeNotification FILE failed.\n"
				<< mountSubDir._full_path << std::endl;
		}

		mountSubDir._watch_handle[2] = FindFirstChangeNotificationA(
			mountSubDir._full_path.c_str(),
			FALSE,
			FILE_NOTIFY_CHANGE_LAST_WRITE);

		if (mountSubDir._watch_handle[2] == INVALID_HANDLE_VALUE)
		{
			std::cout << "\n ERROR: FindFirstChangeNotification FILE failed.\n"
				<< mountSubDir._full_path << std::endl;
		}
	}

	void file_system_detail::DirectoryWatcher::UpdateWatchedDir(fs_mount& mount, fs_dir& dir)
	{
		UNREFERENCED_PARAMETER(mount);

		// false = returns when any one of the handles signal
		// 0     = No waiting, function immediately returns
		dir._status = WaitForMultipleObjects(3, dir._watch_handle, false, 0);

		switch (dir._status)
		{
		case WAIT_OBJECT_0:
		{
			// Directory Created/Deleted/Renamed/Replaced
			RefreshTree(dir);

			if (FindNextChangeNotification(dir._watch_handle[0]) == FALSE) {
				std::printf("\n ERROR: FindNextChangeNotification function failed.\n");
			}
			break;
		}
		case WAIT_OBJECT_0 + 1:
		{
			// File Created/Deleted/Renamed/Replaced
			RefreshDir(dir);

			if (FindNextChangeNotification(dir._watch_handle[1]) == FALSE) {
				std::printf("\n ERROR: FindNextChangeNotification function failed.\n");
			}
			break;
		}
		case WAIT_OBJECT_0 + 2:
		{
			// File was written to recently
			checkFilesWrittenTo(dir);

			if (FindNextChangeNotification(dir._watch_handle[2]) == FALSE) {
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
		FS::path path{ mountDir._full_path.sv() };
		
		auto num_files = static_cast<size_t>(std::count_if(FS::directory_iterator{ path }, FS::directory_iterator{},
			static_cast<bool (*)(const FS::path&)>(FS::is_regular_file)));

		// Files were created
		if (num_files > mountDir._files_map.size())
		{
			checkFilesCreated(mountDir);
		}
		// Files were deleted
		else if (num_files < mountDir._files_map.size())
		{
			checkFilesDeleted(mountDir);
		}
		//If number of files are the same, it means that files were renamed/replaced
		else
		{
			// In the case of replacement of files, we actually want to send a file written to notification
			checkFilesRenamed(mountDir);
		}

	}

	void file_system_detail::DirectoryWatcher::RefreshTree(file_system_detail::fs_dir& mountDir)
	{
		FS::path path{ mountDir._full_path.sv() };
		FS::directory_iterator dir{ path };

		auto num_files = static_cast<size_t>(std::count_if(FS::directory_iterator{ path }, FS::directory_iterator{},
			[](const FS::path& p) -> bool
		{
			return !FS::is_regular_file(p);
		}));

		// Files were created
		if (num_files > mountDir._sub_dirs.size())
		{
			std::cout << "[FILE SYSTEM] Files Created" << std::endl;
			
		}
		// Files were deleted
		else if (num_files < mountDir._sub_dirs.size())
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
			if (vfs._mounts.empty())
				throw("Something is terribly wrong. No mounts found.");

			auto& internal_collated = vfs._mounts[internal_file._tree_index._mount_id]._path_tree[internal_file._tree_index._depth];
			auto& parent_dir = vfs.getDir(internal_file._parent);
			
			switch (internal_file._change_status)
			{
			case CHANGE_STATUS::CREATED:
				// If a file was created, we need to create a suitable handle for it.
				// Since this was already when it was actually checked, we dont need to do it here.
				break;
			case CHANGE_STATUS::DELETED:
			{
				// Most troublesome one. 
				// Need to invalidate the fs_file_handle and all FileHandles pointing to it.

				// First we remove this file from the parent dir.
				auto result = parent_dir._files_map.find(internal_file._filename);
				parent_dir._files_map.erase(result);

				// Next, invalidate it from path_tree. Should be a better way to do this. Not sure yet.
				// When we create files in run-time, we will loop through all the fs_file and find the first valid one if available. Linear time POG.
				internal_collated._files[internal_file._tree_index._index]._valid = false;

				// Lastly, we invalidate the file handle that this file was using.
				// Remember that we DO NOT erase the fs_file_handle in FileSyste::file_handles. We simply invalidate it.
				vfs._file_handles[internal_file._handle_index].Invalidate();
				break;
			}
			case CHANGE_STATUS::RENAMED:
				// Very straightforward. Since only the name was changed, don't have to do anything here I think.
				break;
			default:
				break;
			}

			internal_file._change_status = CHANGE_STATUS::NO_CHANGE;
		}

		changed_files.clear();
		changed_dirs.clear();
	}

	void file_system_detail::DirectoryWatcher::checkFilesCreated(file_system_detail::fs_dir& mountDir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		for (auto& file : FS::directory_iterator(mountDir._full_path.sv()))
		{
			FS::path tmp{ file.path() };

			// We only check if it is a regular file
			if (!FS::is_regular_file(tmp))
				continue;

			string filename = tmp.filename().string();

			auto result = mountDir._files_map.find(filename);
			if (result == mountDir._files_map.end())
			{
				// Check if there are even mounts. If this hits, something is terribly wrong...
				if (vfs._mounts.empty())
					throw("Something is terribly wrong. No mounts found.");

				// Request a slot from mounts
				fs_key slot = vfs.requestFileSlot(vfs._mounts[mountDir._tree_index._mount_id], mountDir._tree_index._depth + 1);
				fs_file& f = vfs.getFile(slot);
				vfs.initFile(f, mountDir, tmp);
				f._change_status = CHANGE_STATUS::CREATED;

				changed_files.push_back(f._tree_index);

				auto& test = vfs.getFile(f._tree_index);
				std::cout << "[FILE SYSTEM] File Created: " << "\n"
					"\t Path: " << f._full_path << "\n" <<
					"\t Test: " << test._full_path << "\n" <<  std::endl;
				break;
			}
		}
	}

	void file_system_detail::DirectoryWatcher::checkFilesDeleted(file_system_detail::fs_dir& dir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		for (auto& file : dir._files_map)
		{
			auto& internal_file = vfs.getFile(file.second);
			if (!vfs.ExistsFull(internal_file._full_path))
			{
				// Might need to do more here depending on what we need.
				internal_file._change_status = CHANGE_STATUS::DELETED;
				
				// internal_file.handle_index = -1;
				// internal_file.full_path.clear();

				changed_files.push_back(internal_file._tree_index);

				std::cout << "[FILE SYSTEM] File Deleted: " << "\n"
					"\t Path: " << internal_file._full_path << "\n" << std::endl;
			}
		}

	}

	void file_system_detail::DirectoryWatcher::checkFilesRenamed(file_system_detail::fs_dir& mountDir)
	{
		for (auto& file : FS::directory_iterator(mountDir._full_path.sv()))
		{
			FS::path tmp{ file.path() };

			// We only check if it is a regular file
			if (!FS::is_regular_file(tmp))
				continue;

			string filename = tmp.filename().string();

			auto result = mountDir._files_map.find(filename);
			if (result == mountDir._files_map.end())
			{
				// File was really renamed.
				// Now we need to find the corresponding internal fs_file that was renamed
				// One of the fs_file inside the dir does not exists anymore due to being renamed.
				auto& vfs = Core::GetSystem<FileSystem>();
				for (auto& internal_file_index : mountDir._files_map)
				{
					auto& internal_file = vfs.getFile(internal_file_index.second);
					if (!vfs.ExistsFull(internal_file._full_path))
					{
						std::cout << "[FILE SYSTEM] File Renamed: " << "\n"
							"\t Prev Path: " << internal_file._full_path << "\n"
							"\t Curr Path: " << tmp.string() << "\n" << std::endl;

						// This is the file that was renamed.
						vfs.initFile(internal_file, mountDir, tmp);
						internal_file._change_status = CHANGE_STATUS::RENAMED;

						changed_files.push_back(internal_file._tree_index);
						return;
					}
				}
			}
		}

		// std::cout << "[FILE SYSTEM] Cannot find renamed file. Looking for replaced file." << std::endl;
		// If we cannot find the corresponding file or if the directory looks exactly the same, 
		// it means that a file has been replaced. In which case, we want to check for write.
		checkFilesWrittenTo(mountDir);
	}

	void file_system_detail::DirectoryWatcher::checkFilesWrittenTo(file_system_detail::fs_dir& dir)
	{
		for (auto& file : FS::directory_iterator(dir._full_path.sv()))
		{
			auto current_file_last_write_time = FS::last_write_time(file);
			FS::path tmp{ file.path() };

			// We only check if it is a regular file
			if (!FS::is_regular_file(tmp))
				continue;

			string filename = tmp.filename().string();

			auto result = dir._files_map.find(filename);
			if (result == dir._files_map.end())
			{
				// This means that we either entered into here erronously or some other bug.
				std::cout << "[FILE SYSTEM] FILE_NOTIFY_CHANGE_LAST_WRITTEN WAS IGNORED." << std::endl;
				break;
			}

			auto& internal_file = Core::GetSystem<FileSystem>().getFile(result->second);
			if (current_file_last_write_time != internal_file._time)
			{
				std::cout << "[FILE SYSTEM] File Written To: " << "\n"
					"\t Path: " << internal_file._full_path << "\n" << std::endl;

				internal_file._time = current_file_last_write_time;
				internal_file._change_status = CHANGE_STATUS::WRITTEN;

				changed_files.push_back(internal_file._tree_index);
			}
		}
	}

	
}