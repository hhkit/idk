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

	void file_system_detail::DirectoryWatcher::UpdateWatchedDir(const fs_mount& mount, fs_dir& dir)
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
			checkFilesWritten(dir);

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
		FS::path path{ mountDir._full_path };
		
		auto num_files = s_cast<size_t>(std::count_if(FS::directory_iterator{ path }, FS::directory_iterator{},
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
		FS::path path{ mountDir._full_path };
		FS::directory_iterator dir{ path };

		const auto num_files = static_cast<size_t>(std::count_if(FS::directory_iterator{ path }, FS::directory_iterator{},
							[](const FS::path& p) -> bool
							{
								return !FS::is_regular_file(p);
							}));

		// Files were created
		if (num_files > mountDir._sub_dirs.size())
		{
			checkDirCreated(mountDir);
		}
		// Files were deleted
		else if (num_files < mountDir._sub_dirs.size())
		{
			checkDirDeleted(mountDir);
			
		}
		//If neither, it means that files were renamed/replaced
		else
		{
			std::cout << "DIRECTORY RENAMED NOT DONE YET!!!" << std::endl;
			// checkDirRenamed(mountDir);
		}
	}

	void file_system_detail::DirectoryWatcher::ResolveAllChanges()
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (vfs._mounts.empty())
			throw("Something is terribly wrong. No mounts found.");

		// Resolve changed files
		for (auto& file_index : changed_files)
		{
			auto& internal_file = vfs.getFile(file_index);
			
			switch (internal_file._change_status)
			{
			case FS_CHANGE_STATUS::CREATED:
				// If a file was created, we need to create a suitable handle for it.
				// Since this was already when it was actually checked, we dont need to do it here.
				break;
			case FS_CHANGE_STATUS::DELETED:
			{
				// Most troublesome one. 
			
				// We remove this file from the parent dir.
				auto& parent_dir = vfs.getDir(internal_file._parent);
				auto result = parent_dir._files_map.find(internal_file._filename);
				parent_dir._files_map.erase(result);

				internal_file.IncRefCount();
				internal_file.SetValid(false);

				break;
			}
			case FS_CHANGE_STATUS::RENAMED:
				// Very straightforward. Since only the name was changed, don't have to do anything here I think.
				break;
			case FS_CHANGE_STATUS::WRITTEN:
				// Very straightforward. Since only the name was changed, don't have to do anything here I think.
				break;
			default:
				break;
			}

			internal_file._change_status = FS_CHANGE_STATUS::NO_CHANGE;
		}

		// Resolve changed directories
		for (auto& dir_index : changed_dirs)
		{
			auto& internal_dir = vfs.getDir(dir_index);

			switch (internal_dir._change_status)
			{
			case FS_CHANGE_STATUS::CREATED:
				break;
			case FS_CHANGE_STATUS::DELETED:
			{
				// First we remove this file from the parent dir.
				auto& parent_dir = vfs.getDir(internal_dir._parent);
				auto result = parent_dir._sub_dirs.find(internal_dir._filename);
				parent_dir._sub_dirs.erase(result);

				// Next, invalidate it from path_tree. Should be a better way to do this. Not sure yet.
				// When we create files in run-time, we will loop through all the fs_file and find the first valid one if available. Linear time POG.
				internal_dir.IncRefCount();
				internal_dir.SetValid(false);

				break;
			}
			case FS_CHANGE_STATUS::RENAMED:
				break;
			default:
				break;
			}
			internal_dir._change_status = FS_CHANGE_STATUS::NO_CHANGE;
		}

		changed_files.clear();
		changed_dirs.clear();
	}

	void file_system_detail::DirectoryWatcher::checkFilesCreated(file_system_detail::fs_dir& mountDir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		for (auto& file : FS::directory_iterator(mountDir._full_path))
		{
			FS::path tmp{ file.path() };

			// We only check if it is a regular file
			if (!FS::is_regular_file(tmp))
				continue;

			string filename = tmp.filename().generic_string();

			auto result = mountDir._files_map.find(filename);
			if (result == mountDir._files_map.end())
			{
				const auto key = fileCreate(mountDir, tmp);

				const auto& f = vfs.getFile(key);
				std::cout << "[FILE SYSTEM] File Created: " << "\n"
							 "\t Path: " << f._full_path << "\n" << std::endl;
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
				fileDelete(internal_file);
				
				std::cout << "[FILE SYSTEM] File Deleted: " << "\n"
							 "\t Path: " << internal_file._full_path << "\n" << std::endl;
			}
		}

	}

	void file_system_detail::DirectoryWatcher::checkFilesRenamed(file_system_detail::fs_dir& mountDir)
	{
		for (auto& file : FS::directory_iterator(mountDir._full_path))
		{
			FS::path tmp{ file.path() };

			// We only check if it is a regular file
			if (!FS::is_regular_file(tmp))
				continue;

			string filename = tmp.filename().generic_string();

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
						string prev_path = internal_file._full_path;
						fileRename(mountDir, internal_file, tmp);
						
						std::cout << "[FILE SYSTEM] File Renamed: " << "\n"
									 "\t Prev Path: " << prev_path << "\n"
									 "\t Curr Path: " << tmp.generic_string() << "\n" << std::endl;
						return;
					}
				}
			}
		}

		// std::cout << "[FILE SYSTEM] Cannot find renamed file. Looking for replaced file." << std::endl;
		// If we cannot find the corresponding file or if the directory looks exactly the same, 
		// it means that a file has been replaced. In which case, we want to check for write.
		checkFilesWritten(mountDir);
	}

	void file_system_detail::DirectoryWatcher::checkFilesWritten(file_system_detail::fs_dir& dir)
	{
		for (auto& file : FS::directory_iterator(dir._full_path))
		{
			const auto current_file_last_write_time = FS::last_write_time(file);
			FS::path tmp{ file.path() };

			// We only check if it is a regular file
			if (!FS::is_regular_file(tmp))
				continue;

			string filename = tmp.filename().generic_string();

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
				internal_file._change_status = FS_CHANGE_STATUS::WRITTEN;

				changed_files.push_back(internal_file._tree_index);
				return;
			}
		}
		// std::cout << "[FILE SYSTEM] Cannot find file write change." << std::endl;
	}

	void file_system_detail::DirectoryWatcher::checkDirCreated(file_system_detail::fs_dir& mountDir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		for (auto& file : FS::directory_iterator(mountDir._full_path))
		{
			FS::path tmp{ file.path() };

			// We only check directories
			if (FS::is_regular_file(tmp))
				continue;

			string dir_name = tmp.filename().generic_string();

			auto result = mountDir._sub_dirs.find(dir_name);
			if (result == mountDir._sub_dirs.end())
			{
				// Check if there are even mounts. If this hits, something is terribly wrong...
				if (vfs._mounts.empty())
					throw("Something is terribly wrong. No mounts found.");

				// Create the new dir
				const auto key = dirCreate(mountDir, tmp);

				auto& d = vfs.getDir(key);
				std::cout << "[FILE SYSTEM] Dir Created: " << "\n"
							 "\t Path: " << d._full_path << "\n" << std::endl;
							
				recurseAndAdd(d);
				break;
			}
		}
	}

	void file_system_detail::DirectoryWatcher::checkDirDeleted(file_system_detail::fs_dir& mountDir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		for (auto& dir : mountDir._sub_dirs)
		{
			auto& internal_dir = vfs.getDir(dir.second);
			if (!vfs.ExistsFull(internal_dir._full_path))
			{
				
				dirDelete(internal_dir);
				std::cout <<	"[FILE SYSTEM] Dir Deleted: " << "\n"
								"\t Path: " << internal_dir._full_path << "\n" << std::endl;

				recurseAndDelete(internal_dir);
			}
		}
	}

	void file_system_detail::DirectoryWatcher::checkDirRenamed(file_system_detail::fs_dir& mountDir)
	{
		for (auto& file : FS::directory_iterator(mountDir._full_path))
		{
			FS::path tmp{ file.path() };

			// We only check if it is a dir
			if (FS::is_regular_file(tmp))
				continue;

			string filename = tmp.filename().generic_string();

			auto result = mountDir._sub_dirs.find(filename);
			if (result == mountDir._sub_dirs.end())
			{
				// File was really renamed.
				// Now we need to find the corresponding internal fs_file that was renamed
				// One of the fs_file inside the dir does not exists anymore due to being renamed.
				auto& vfs = Core::GetSystem<FileSystem>();
				for (auto& internal_dir_index : mountDir._sub_dirs)
				{
					auto& internal_dir = vfs.getDir(internal_dir_index.second);
					if (!vfs.ExistsFull(internal_dir._full_path))
					{
						string prev_path = internal_dir._full_path;
						dirRename(mountDir, internal_dir, tmp);

						std::cout << "[FILE SYSTEM] File Renamed: " << "\n"
							"\t Prev Path: " << prev_path << "\n"
							"\t Curr Path: " << tmp.generic_string() << "\n" << std::endl;
						return;
					}
				}
			}
		}
	}

	void file_system_detail::DirectoryWatcher::recurseAndAdd(file_system_detail::fs_dir& mountDir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		for (auto& file : FS::directory_iterator(mountDir._full_path))
		{
			FS::path tmp{ file.path() };
			if (!FS::is_regular_file(tmp))
			{
				// Check if there are even mounts. If this hits, something is terribly wrong...
				if (vfs._mounts.empty())
					throw("Something is terribly wrong. No mounts found.");

				// Create the new dir
				const auto key = dirCreate(mountDir, tmp);

				auto& d = vfs.getDir(key);
				std::cout << "[FILE SYSTEM] Dir Created (Recursed): " << "\n"
							 "\t Path: " << d._full_path << "\n" << std::endl;
							 
				recurseAndAdd(d);
			}
			else
			{
				// Add file
				const auto key = fileCreate(mountDir, tmp);

				const auto& f = vfs.getFile(key);
				std::cout << "[FILE SYSTEM] File Created (From dir created notification): " << "\n"
							 "\t Path: " << f._full_path << "\n" << std::endl;					 
			}
		}
	}

	void file_system_detail::DirectoryWatcher::recurseAndDelete(file_system_detail::fs_dir& mountDir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		for (auto& file : mountDir._files_map)
		{
			auto& internal_file = vfs.getFile(file.second);
			fileDelete(internal_file);

			std::cout << "[FILE SYSTEM] File Deleted (from dir deleted notication): " << "\n"
						 "\t Path: " << internal_file._full_path << "\n" << std::endl;
		}

		for (auto& dir : mountDir._sub_dirs)
		{
			auto& internal_dir = vfs.getDir(dir.second);
			dirDelete(internal_dir);

			std::cout << "[FILE SYSTEM] Dir Deleted (recursed): " << "\n"
						 "\t Path: " << internal_dir._full_path << "\n" << std::endl;

			recurseAndDelete(internal_dir);
		}
	}

	void file_system_detail::DirectoryWatcher::recurseAndRename(file_system_detail::fs_dir& dir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		for (auto& file_index : dir._files_map)
		{
			auto& internal_file = vfs.getFile(file_index.second);
			string append = '/' + internal_file._filename;

			internal_file._full_path = dir._full_path + append;
			internal_file._rel_path = dir._rel_path + append;
			internal_file._mount_path = dir._mount_path + append;
		}

		for (auto& dir_index : dir._sub_dirs)
		{
			auto& internal_dir = vfs.getDir(dir_index.second);
			string append = '/' + internal_dir._filename;

			internal_dir._full_path = dir._full_path + append;
			internal_dir._rel_path = dir._rel_path + append;
			internal_dir._mount_path = dir._mount_path + append;

			recurseAndRename(internal_dir);
		}
	}

	file_system_detail::fs_key file_system_detail::DirectoryWatcher::fileCreate(file_system_detail::fs_dir& mountDir, const FS::path& p)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (vfs._mounts.empty())
			throw("Something is terribly wrong. No mounts found.");

		// Request a slot from mounts
		const fs_key slot = vfs.requestFileSlot(vfs._mounts[mountDir._tree_index._mount_id], mountDir._tree_index._depth + 1);
		fs_file& f = vfs.getFile(slot);
		vfs.initFile(f, mountDir, p);
		f._change_status = FS_CHANGE_STATUS::CREATED;

		changed_files.push_back(f._tree_index);
		return f._tree_index;
	}

	void file_system_detail::DirectoryWatcher::fileDelete(file_system_detail::fs_file& file)
	{
		// auto& vfs = Core::GetSystem<FileSystem>();
		file._change_status = FS_CHANGE_STATUS::DELETED;

		// Invalidate it from path_tree. Should be a better way to do this. Not sure yet. 
		// When we create files in run-time, we will loop through all the fs_file and find the first valid one if available. Linear time POG.

		// Invalidate the file for read/write as well as 
		file.SetValid(false);
		file.SetOpenMode(FS_PERMISSIONS::NONE);

		changed_files.push_back(file._tree_index);
	}

	void file_system_detail::DirectoryWatcher::fileRename(file_system_detail::fs_dir& dir, file_system_detail::fs_file& file, const std::filesystem::path& p, FS_CHANGE_STATUS status)
	{
		// This is the file that was renamed.
		Core::GetSystem<FileSystem>().initFile(file, dir, p);
		file._change_status = status;

		changed_files.push_back(file._tree_index);
	}

	file_system_detail::fs_key file_system_detail::DirectoryWatcher::dirCreate(file_system_detail::fs_dir& mountDir, const std::filesystem::path& p)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		// Request a slot from mounts
		const fs_key slot = vfs.requestDirSlot(vfs._mounts[mountDir._tree_index._mount_id], mountDir._tree_index._depth + 1);
		fs_dir& d = vfs.getDir(slot);
		vfs.initDir(d, mountDir, p);
		d._change_status = FS_CHANGE_STATUS::CREATED;

		changed_dirs.push_back(d._tree_index);

		WatchDirectory(d);

		return d._tree_index;
	}

	void file_system_detail::DirectoryWatcher::dirDelete(file_system_detail::fs_dir& dir)
	{
		// Might need to do more here depending on what we need.
		dir._change_status = FS_CHANGE_STATUS::DELETED;

		changed_dirs.push_back(dir._tree_index);
	}

	void file_system_detail::DirectoryWatcher::dirRename(file_system_detail::fs_dir& mountDir, file_system_detail::fs_dir& dir, const std::filesystem::path& p, FS_CHANGE_STATUS status)
	{
		// This is the file that was renamed.
		Core::GetSystem<FileSystem>().initDir(dir, mountDir, p);
		dir._change_status = status;

		changed_dirs.push_back(dir._tree_index);
	}

	
}