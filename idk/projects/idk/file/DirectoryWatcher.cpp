#include "stdafx.h"
#include "DirectoryWatcher.h"
#include "FileSystem.h"
#include "core/Core.h"

#include <iostream>
#include <filesystem>
#include <chrono>
#include <utility>

namespace FS = std::filesystem;
using namespace std::chrono_literals;

namespace idk
{
	void file_system_detail::DirectoryWatcher::WatchDirectory(fs_dir& mountDir)
	{
		// Initialize watch handles
		mountDir._watch_handle = FindFirstChangeNotificationA(
										mountDir._full_path.c_str(),		// full path of mount
										TRUE,								// Watch sub trees
										FILE_NOTIFY_CHANGE_DIR_NAME |		// Directory create/delete/rename
										FILE_NOTIFY_CHANGE_FILE_NAME |		// File create/delete/rename
										FILE_NOTIFY_CHANGE_LAST_WRITE);		// File write

		if (mountDir._watch_handle == INVALID_HANDLE_VALUE)
		{
			std::cout << "[Directory Watcher] ERROR: Unable to watch directory: \n" <<
						 "Path: " << mountDir._full_path << std::endl;
		}
	}

	void file_system_detail::DirectoryWatcher::UpdateWatchedDir(fs_dir& dir)
	{
		if (!dir.IsValid())
			return;

		// Wait for file/dir changes
		dir._status = WaitForSingleObject(dir._watch_handle, 0);

		switch (dir._status)
		{
		case WAIT_OBJECT_0:
		{
			print_log.clear();
			// Check all paths to see if there are changes. Possible to have no changes.
			CheckAllPathChanges(dir);

			// Print logs if there are path changes.
			if (changed_dirs.size() > 0 || changed_files.size() > 0)
			{
				std::cout << "=========================================================================================================\n";
				std::cout << "[Directory Watcher] Path changes detected in: " << dir._mount_path << ".\n";
				std::cout << "=========================================================================================================\n" << std::endl;

				std::cout << print_log << std::endl;

				std::cout << "=========================================================================================================" << std::endl;
				std::cout << "Total Directories Changed: " << changed_dirs.size() << "." << std::endl;
				std::cout << "Total Files Changed: " << changed_files.size() << "." << std::endl;
				std::cout << "Total Paths Changed: " << changed_dirs.size() + changed_files.size() << "." << std::endl;
				std::cout << "=========================================================================================================\n" << std::endl;
			}

			if (FindNextChangeNotification(dir._watch_handle) == FALSE) 
			{
				std::cout << "[Directory Watcher] ERROR: FindNextChangeNotification failed." << std::endl;
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

	void file_system_detail::DirectoryWatcher::CheckAllPathChanges(file_system_detail::fs_dir& dir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		FS::path path{ dir._full_path };
		// Do nothing if the directory doesnt exists
		if (!FS::exists(path))
			return;

		CheckFileChanges(dir);
		CheckDirChanges(dir);

		for (auto& sub_dir_index : dir._sub_dirs)
		{
			auto& sub_dir = vfs.getDir(sub_dir_index.second);

			// We don't want to check directories that were just created or deleted
			if (sub_dir._change_status == FS_CHANGE_STATUS::DELETED || sub_dir._change_status == FS_CHANGE_STATUS::CREATED)
				continue;

			CheckAllPathChanges(sub_dir);
		}
	}

	void file_system_detail::DirectoryWatcher::CheckFileChanges(file_system_detail::fs_dir& dir)
	{
		auto num_files = s_cast<size_t>(std::count_if(FS::directory_iterator{ dir._full_path }, FS::directory_iterator{},
			static_cast<bool (*)(const FS::path&)>(FS::is_regular_file)));

		// Checking for file changes.
		if (num_files > dir._files_map.size())
		{
			checkFilesCreated(dir);
		}
		// Files were deleted
		else if (num_files < dir._files_map.size())
		{
			checkFilesDeleted(dir);
		}
		//If number of files are the same, it means that files were renamed/replaced
		else
		{
			// We need to check if files were renamed as well as written.
			checkFilesRenamed(dir);
			checkFilesWritten(dir);
		}
	}

	void file_system_detail::DirectoryWatcher::CheckDirChanges(file_system_detail::fs_dir& dir)
	{
		const auto num_dirs = static_cast<size_t>(std::count_if(FS::directory_iterator{ dir._full_path }, FS::directory_iterator{},
			[](const FS::path& p) -> bool
			{
				return !FS::is_regular_file(p);
			}));

		// Dirs were created
		if (num_dirs > dir._sub_dirs.size())
		{
			checkDirCreated(dir);
		}
		// Dirs were deleted
		else if (num_dirs < dir._sub_dirs.size())
		{
			checkDirDeleted(dir);

		}
		// If neither, it means that dirs were renamed
		else
		{
			checkDirRenamed(dir);
		}
	}

	void file_system_detail::DirectoryWatcher::ResolveAllChanges()
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		
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
				print_log += "[File Created]\n";
				print_log += "Path: " + f._mount_path + "\n\n";
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

				print_log += "[File Deleted]\n";
				print_log += "Path: " + internal_file._mount_path + "\n\n";
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
				// vector<std::pair<fs_key, FS::path>> renamed_files;
				for (auto& internal_file_index : mountDir._files_map)
				{
					auto& internal_file = vfs.getFile(internal_file_index.second);
					if (!vfs.ExistsFull(internal_file._full_path))
					{
						string prev_path = internal_file._mount_path;
						// renamed_files.emplace_back(std::make_pair( internal_file_index, tmp ));
						fileRename(mountDir, internal_file, tmp);
						
						print_log += "[File Renamed]\n";
						print_log += "Before:" + prev_path + "\n";
						print_log += "After: " + internal_file._mount_path + "\n\n";
						
						break;
					}
				}
			}
		}

		// std::cout << "[FILE SYSTEM] Cannot find renamed file. Looking for replaced file." << std::endl;
		// If we cannot find the corresponding file or if the directory looks exactly the same, 
		// it means that a file has been replaced. In which case, we want to check for write.
		// checkFilesWritten(mountDir);
	}

	void file_system_detail::DirectoryWatcher::checkFilesWritten(file_system_detail::fs_dir& dir)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
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
				print_log += "[Directory Watcher] Error encountered in checkFileWrite.\n";
				print_log += "File: " + filename + " does not exists.\n\n";
				break;
			}

			auto& internal_file = vfs.getFile(result->second);
			if (current_file_last_write_time != internal_file._time)
			{
				internal_file._time = current_file_last_write_time;
				internal_file._change_status = FS_CHANGE_STATUS::WRITTEN;

				print_log += "[File Written]\n";
				print_log += "Path:" + internal_file._mount_path + "\n\n";

				changed_files.push_back(internal_file._tree_index);	
			}
		}

		// for (auto& dir_index : dir._sub_dirs)
		// {
		// 	auto& internal_dir = vfs.getDir(dir_index.second);
		// 	checkFilesWritten(internal_dir);
		// }
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
				// Create the new dir
				const auto key = dirCreate(mountDir, tmp);

				auto& d = vfs.getDir(key);
				print_log += "[Directory Created]\n";
				print_log += "Path: " + d._mount_path + "\n\n";
							
				recurseAndAdd(d);
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
				print_log += "[Directory Deleted]\n";
				print_log += "Path: " + internal_dir._mount_path + "\n\n";

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
						string prev_path = internal_dir._mount_path;
						dirRename(mountDir, internal_dir, tmp);

						print_log += "[Directory Renamed]\n";
						print_log += "Before:" + prev_path + "\n";
						print_log += "After: " + internal_dir._mount_path + "\n\n";

						recurseAndRename(internal_dir);
						break;
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
				// Create the new dir
				const auto key = dirCreate(mountDir, tmp);

				auto& d = vfs.getDir(key);
				print_log += "[Directory Created] (recursed)\n";
				print_log += "Path: " + d._mount_path + "\n\n";
				recurseAndAdd(d);
			}
			else
			{
				// Add file
				const auto key = fileCreate(mountDir, tmp);

				const auto& f = vfs.getFile(key);
				print_log += "[File Created] (recursed)\n";
				print_log += "Path: " + f._mount_path + "\n\n";
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

			print_log += "[File Deleted] (recursed)\n";
			print_log += "Path: " + internal_file._mount_path + "\n\n";
		}

		for (auto& dir : mountDir._sub_dirs)
		{
			auto& internal_dir = vfs.getDir(dir.second);
			dirDelete(internal_dir);

			print_log += "[Directory Deleted] (recursed)\n";
			print_log += "Path: " + internal_dir._mount_path + "\n\n";

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

			string prev_path = internal_file._mount_path;
			internal_file._full_path = dir._full_path + append;
			internal_file._rel_path = dir._rel_path + append;
			internal_file._mount_path = dir._mount_path + append;

			print_log += "[File Renamed] (recursed)\n";
			print_log += "Before:" + prev_path + "\n";
			print_log += "After: " + internal_file._mount_path + "\n\n";
		}

		for (auto& dir_index : dir._sub_dirs)
		{
			auto& internal_dir = vfs.getDir(dir_index.second);
			string append = '/' + internal_dir._filename;

			string prev_path = internal_dir._mount_path;
			internal_dir._full_path = dir._full_path + append;
			internal_dir._rel_path = dir._rel_path + append;
			internal_dir._mount_path = dir._mount_path + append;

			print_log += "[Directory Renamed] (recursed)\n";
			print_log += "Before:" + prev_path + "\n";
			print_log += "After: " + internal_dir._mount_path + "\n\n";

			recurseAndRename(internal_dir);
		}
	}

	file_system_detail::fs_key file_system_detail::DirectoryWatcher::fileCreate(file_system_detail::fs_dir& mountDir, const FS::path& p)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		
		// Request a slot from mounts
		const fs_key slot = vfs.requestFileSlot(vfs._mounts[mountDir._tree_index._mount_id], mountDir._tree_index._depth + 1);
		fs_file& f = vfs.getFile(slot);
		vfs.initFile(f, mountDir, p);
		mountDir._files_map.emplace(f._filename, f._tree_index);

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

	void file_system_detail::DirectoryWatcher::fileRename(file_system_detail::fs_dir& mountDir, file_system_detail::fs_file& file, const std::filesystem::path& p)
	{
		// This is the file that was renamed.
		// Remove myself from the table first.
		auto res = mountDir._files_map.find(file._filename);
		assert(res != mountDir._files_map.end());
		mountDir._files_map.erase(res);

		Core::GetSystem<FileSystem>().initFile(file, mountDir, p);
		mountDir._files_map.emplace(file._filename, file._tree_index);

		file._change_status = FS_CHANGE_STATUS::RENAMED;

		changed_files.push_back(file._tree_index);
	}

	file_system_detail::fs_key file_system_detail::DirectoryWatcher::dirCreate(file_system_detail::fs_dir& mountDir, const std::filesystem::path& p)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		// Request a slot from mounts
		const fs_key slot = vfs.requestDirSlot(vfs._mounts[mountDir._tree_index._mount_id], mountDir._tree_index._depth + 1);
		fs_dir& d = vfs.getDir(slot);
		vfs.initDir(d, mountDir, p);
		mountDir._sub_dirs.emplace(d._filename, d._tree_index);

		d._change_status = FS_CHANGE_STATUS::CREATED;

		changed_dirs.push_back(d._tree_index);

		return d._tree_index;
	}

	void file_system_detail::DirectoryWatcher::dirDelete(file_system_detail::fs_dir& dir)
	{
		// Might need to do more here depending on what we need.
		dir._change_status = FS_CHANGE_STATUS::DELETED;

		changed_dirs.push_back(dir._tree_index);
	}

	void file_system_detail::DirectoryWatcher::dirRename(file_system_detail::fs_dir& mountDir, file_system_detail::fs_dir& dir, const std::filesystem::path& p)
	{
		// This is the dir that was renamed.
		// Remove myself from the table first.
		auto res = mountDir._sub_dirs.find(dir._filename);
		assert(res != mountDir._sub_dirs.end());
		mountDir._sub_dirs.erase(res);

		Core::GetSystem<FileSystem>().initDir(dir, mountDir, p);
		mountDir._sub_dirs.emplace(dir._filename, dir._tree_index);

		dir._change_status = FS_CHANGE_STATUS::RENAMED;

		changed_dirs.push_back(dir._tree_index);
	}

	
}