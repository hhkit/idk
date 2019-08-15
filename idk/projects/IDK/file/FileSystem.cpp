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
		char base_dir_c[MAX_PATH] = { 0 };
		int bytes = GetModuleFileNameA(NULL, base_dir_c, MAX_PATH);
		// if (!_getcwd(base_dir_c, sizeof(base_dir_c)))
		if(bytes == 0)
		{
			std::cout << "[File System] Unable to get base directory." << std::endl;
		}
		base_dir = base_dir_c;
		auto pos = base_dir.find_last_of("\\");
		base_dir = base_dir.substr(0, pos + 1);

		// Mount(base_dir + "resource/", "/test_mount");
	}

	void FileSystem::Update()
	{
		// Clear all changed files here
		directory_watcher.ResolveAllChanges();

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

	string FileSystem::GetFullPath(string_view mountPath) const
	{
		string mount_path{ mountPath.data() };

		if (mount_path[0] != '/')
			return string{};

		auto end_pos = mount_path.find_first_of('/', 1);
		if (end_pos == string::npos)
		{
			return string{};
		}

		string mount_key = mount_path.substr(0, end_pos);
		auto mount_index = mount_table.find(mount_key);
		if (mount_index != mount_table.end())
		{
			return mounts[mount_index->second].full_path + mount_path.substr(end_pos + 1);
		}
		else
		{
			return string{};
		}
	}

	bool FileSystem::Exists(string_view mountPath) const
	{
		return ExistsFull(GetFullPath(mountPath));
	}

	bool FileSystem::ExistsFull(string_view fullPath) const
	{
		return GetFileAttributesA(fullPath.data()) != INVALID_FILE_ATTRIBUTES;
	}

	FileSystem_ErrorCode FileSystem::Mount(string_view fullPath, string_view mountPath, bool watch)
	{
		size_t curr_mount_index = mounts.size();
		
		string full_path{ fullPath.data() };
		string mount_path{ mountPath.data() };

		// All mountPaths must begin with '/'
		if (mountPath[0] != '/')
			return FILESYSTEM_BAD_ARGUMENT;

		if (full_path.back() != '/')
			full_path += '/';

		mount_table.emplace(mount_path, mounts.size());
		mounts.push_back(file_system_internal::mount_t{});

		initMount(curr_mount_index, full_path, mount_path, watch);
		
		return FILESYSTEM_NOT_FOUND;
	}

	void FileSystem::initMount(size_t index, string_view fullPath, string_view mountPath, bool watch)
	{
		if (!ExistsFull(fullPath))
		{
			std::cout << "[FILE SYSTEM] Bad path given to Mount function" << std::endl;
			return;
		}

		FS::path currPath{ fullPath };
		FS::directory_iterator dir{ currPath };

		// Initializing base variables of the mount
		file_system_internal::mount_t& mount = mounts[index];
		mount.full_path = fullPath;
		mount.mount_path = mountPath;
		mount.watching = watch;
		mount.mount_index = static_cast<int8_t>(index);
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

				f.tree_index = node;
				f.parent = mountSubDir.tree_index;

				f.handle_index = curr_handle_index++;
				file_handles.emplace_back(node);

				f.time = FS::last_write_time(tmp);

				mountSubDir.files_map.emplace(f.filename, node);

				mount.path_tree[currDepth].files.emplace_back(f);
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

	file_system_internal::dir_t& FileSystem::getDir(file_system_internal::node_t& node)
	{
		return mounts[node.mount_id].path_tree[node.depth].dirs[node.index];
	}

	bool FileSystem::isOpen(const file_system_internal::node_t& n)
	{
		return std::find(open_files.begin(), open_files.end(), n) != open_files.end();
	}

	size_t FileSystem::addFileHandle(const file_system_internal::node_t& node)
	{
		file_handles.emplace_back(node);
		return file_handles.size() - 1;
	}

	FileSystem_ErrorCode FileSystem::Mkdir(string_view mountPath)
	{
		UNREFERENCED_PARAMETER(mountPath);
		// Testing for now.
		// FS::create_directories(FS::path{ "C:/Users/Joseph/Desktop/GIT/idk_legacy/Koboru/Koboru/resource/editor/test" });
		return FILESYSTEM_OK;
	}
}