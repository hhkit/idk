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

		// Mount("C:\\Users\\Joseph\\Desktop\\GIT\\idk_legacy\\Koboru\\Koboru\\resource\\editor", "/test");
	}

	void FileSystem::Run(float dt)
	{
		UNREFERENCED_PARAMETER(dt);

	}

	void FileSystem::Shutdown()
	{
	}

	FileSystem_ErrorCode FileSystem::Mount(const string& fullPath, const string& mountPath)
	{
		size_t curr_mount_index = mounts.size();
		mounts.push_back(file_system_internal::mount_t{});
		initMount(curr_mount_index, fullPath, mountPath);
		
		// if (mount.Init(fullPath, mountPath) > 0)
		// {
		// 	mounts.push_back(mount);
		// 	size_t index = mounts.size() - 1;
		// 	mount_table.emplace(mountPath, static_cast<int8_t>(index));
		// 	return FILESYSTEM_OK;
		// }
		
		return FILESYSTEM_NOT_FOUND;
	}

	void FileSystem::initMount(size_t index, const string& fullPath, const string& mountPath)
	{
		size_t curr_handle_index = file_handles.size();

		file_system_internal::mount_t& mount = mounts[index];

		mount.full_path = fullPath;
		mount.mount_path = mountPath;
		mount.AddDepth();
		
		FS::path currPath{ fullPath };
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

				f.tree_index.depth = 0;
				f.tree_index.index = static_cast<int8_t>(mount.path_tree[0].files.size());

				f.handle_index = curr_handle_index++;
				file_handles.emplace_back(static_cast<int8_t>(index), f.tree_index);

				mount.path_tree[0].files.push_back(f);
				++mount.num_files;
			}
			else
			{
				file_system_internal::dir_t d;

				d.full_path = tmp.string();
				d.filename = tmp.filename().string();

				d.tree_index.depth = 0;
				d.tree_index.index = static_cast<int8_t>(mount.path_tree[0].dirs.size());

				recurseSubDir(index, 0, d);
				mount.path_tree[0].dirs.push_back(d);
			}
		}
		file_handles;
	}

	void FileSystem::recurseSubDir(size_t index, int8_t currDepth, file_system_internal::dir_t& mountSubDir)
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

				mountSubDir.files.push_back(node);

				mount.path_tree[currDepth].files.push_back(f);
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

				recurseSubDir(index, currDepth, d);
				mount.path_tree[currDepth].dirs.push_back(d);
			}
		}
	}
}