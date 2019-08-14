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
						directory_watcher.UpdateWatchedDir(mount, dir);
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
		UNREFERENCED_PARAMETER(mountPath);
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

		d.tree_index.mount_id = static_cast<int8_t>(index);
		d.tree_index.depth = 0;
		d.tree_index.index = static_cast<int8_t>(mount.path_tree[0].dirs.size());
		if (watch)
			directory_watcher.WatchDirectory(d);

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
				node.mount_id = static_cast<int8_t>(index);
				node.depth = currDepth;
				node.index = static_cast<int8_t>(mount.path_tree[currDepth].files.size());

				f.handle_index = curr_handle_index++;
				file_handles.emplace_back(node);

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
				node.mount_id = static_cast<int8_t>(index);
				node.depth = currDepth;
				node.index = static_cast<int8_t>(mount.path_tree[currDepth].dirs.size());

				d.tree_index = node;

				mountSubDir.sub_dirs.push_back(node);
				if (watch)
					directory_watcher.WatchDirectory(d);

				recurseSubDir(index, currDepth, d, watch);
				mount.path_tree[currDepth].dirs.push_back(d);
			}
		}
	}

	file_system_internal::file_t& FileSystem::getFile(file_system_internal::node_t& node)
	{
		return mounts[node.mount_id].GetFile(node);
	}

	void FileSystem::RefreshDir(file_system_internal::dir_t& dir)
	{
		UNREFERENCED_PARAMETER(dir);
	}

	void FileSystem::RefreshTree(file_system_internal::dir_t& dir)
	{
		UNREFERENCED_PARAMETER(dir);
	}
}