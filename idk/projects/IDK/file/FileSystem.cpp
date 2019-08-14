#include "stdafx.h"

#include "FileSystem.h"

#include <filesystem>
#include <utility>
#include <direct.h>
#include <tchar.h>
#include <stdio.h>  
#include <Shlwapi.h>
#include <iostream>

namespace FS = std::filesystem;


namespace idk {
	
	void FileSystem::Init()
	{
		// Get the base directory. This is where the prog is run from.
		char base_dir_c[MAX_PATH];
		if (!_getcwd(base_dir_c, sizeof(base_dir_c)))
		{
			std::cout << "[File System] Unable to get base directory." << std::endl;
		}
		base_dir = base_dir_c;

		//Mount("C:/Users/HP/Desktop/idk_legacy-master/Koboru/Koboru/resource/editor", "/test");
	}

	void FileSystem::Update()
	{
		mounts;
		// For every mount that is watched
		for (auto& mount : mounts)
		{
			if (mount.watching)
			{
				// For every depth
				for (auto& collated : mount.path_tree)
				{
					// For every dir in the current depth
					for (auto& dir : collated.dirs)
					{
						updateWatchedDir(mount, dir);
					}
					
				}
				
			}
		}
	}

	void FileSystem::Shutdown()
	{
	}

	bool FileSystem::Exists(string_view mountPath) const
	{
		return false;
	}

	bool FileSystem::ExistsFull(string_view fullPath) const
	{
		return GetFileAttributesA(fullPath.data()) != INVALID_FILE_ATTRIBUTES;;
	}

	FileSystem_ErrorCode FileSystem::Mount(const string& fullPath, const string& mountPath, bool watch)
	{
		size_t curr_mount_index = mounts.size();
		mounts.push_back(file_system_internal::mount_t{});
		initMount(curr_mount_index, fullPath, mountPath, watch);
		
		return FILESYSTEM_NOT_FOUND;
	}

	void FileSystem::initMount(size_t index, const string& fullPath, const string& mountPath, bool watch)
	{
		FS::path currPath{ fullPath };
		FS::directory_iterator dir{ currPath };

		// Initializing base variables of the mount
		file_system_internal::mount_t& mount = mounts[index];
		mount.full_path = fullPath;
		mount.mount_path = mountPath;
		mount.watching = watch;
		mount.AddDepth();

		// The root of this mount is depth 0
		file_system_internal::dir_t d;

		d.full_path = fullPath;
		d.filename = currPath.filename().string();

		d.tree_index.depth = 0;
		d.tree_index.index = static_cast<int8_t>(mount.path_tree[0].dirs.size());
		if (watch)
			watchDir(d);

		recurseSubDir(index, 0, d, watch);
		mount.path_tree[0].dirs.push_back(d);
	}

	void FileSystem::recurseSubDir(size_t index, int8_t currDepth, file_system_internal::dir_t& mountSubDir, bool watch)
	{
		file_system_internal::mount_t& mount = mounts[index];
		size_t curr_handle_index = file_handles.size();

		// Increase the depth if this expands the tree
		++currDepth;
		if (currDepth >= mount.path_tree.size() - 1)
			mount.AddDepth();

		FS::path currPath{ mountSubDir.full_path };
		FS::directory_iterator dir{ currPath };

		// initializing all the paths from this path
		for (auto& elem : dir)
		{
			FS::path tmp{ elem.path() };
			if (FS::is_regular_file(tmp))
			{
				file_system_internal::file_t f;

				f.full_path = tmp.string();
				f.filename = tmp.filename().string();
				f.extension = tmp.extension().string();

				file_system_internal::node_t node;
				node.depth = currDepth;
				node.index = static_cast<int8_t>(mount.path_tree[currDepth].files.size());

				f.handle_index = curr_handle_index++;
				file_handles.emplace_back(static_cast<int8_t>(index), node);

				f.tree_index = node;

				f.time = FS::last_write_time(tmp);

				mountSubDir.files_map.emplace(f.filename, node);

				// mount.path_tree[currDepth].files_map.emplace(f.filename, mount.path_tree[currDepth].files.size());
				mount.path_tree[currDepth].files.emplace_back(f);
				++mount.num_files;
			}
			else
			{
				file_system_internal::dir_t d;

				d.full_path = tmp.string();
				d.filename = tmp.filename().string();

				file_system_internal::node_t node;
				node.depth = currDepth;
				node.index = static_cast<int8_t>(mount.path_tree[currDepth].dirs.size());

				d.tree_index = node;

				mountSubDir.sub_dirs.push_back(node);
				if (watch)
					watchDir(d);

				recurseSubDir(index, currDepth, d, watch);
				mount.path_tree[currDepth].dirs.push_back(d);
			}
		}
	}
	void FileSystem::watchDir(file_system_internal::dir_t& mountSubDir)
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

	void FileSystem::updateWatchedDir(file_system_internal::mount_t& mount, file_system_internal::dir_t& dir)
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

	file_system_internal::file_t& FileSystem::getFile(int8_t mount_index, file_system_internal::node_t& node)
	{
		return mounts[mount_index].GetFile(node);
	}

	void FileSystem::RefreshDir(file_system_internal::dir_t& dir)
	{
	}

	void FileSystem::RefreshTree(file_system_internal::dir_t& dir)
	{
	}
}