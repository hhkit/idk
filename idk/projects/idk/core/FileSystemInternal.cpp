#include "stdafx.h"
#include "FileSystemInternal.h"

#include <filesystem>
#include <utility>
#include <direct.h>
#include <tchar.h>
#include <stdio.h>  
#include <Shlwapi.h>

namespace FS = std::filesystem;

namespace idk{

	size_t filesystem_internal::mount_t::Init(const string& fullPath, const string& mountPath)
	{
		full_path = fullPath;
		mount_path = mountPath;

		FS::path currPath{ fullPath };
		FS::directory_iterator dir{ currPath };
		
		AddDepth();
		// collated_t& collated = path_tree[0];
		
		// initializing all the paths from this path
		for (auto& elem : dir)
		{
			FS::path tmp{ elem.path() };
			if (FS::is_regular_file(tmp))
			{
				filesystem_internal::file_t f;
				
				f.full_path = tmp.string();
				f.filename	= tmp.filename().string();
				f.extension = tmp.extension().string();

				f.tree_index.depth = 0;
				f.tree_index.index = static_cast<int8_t>(path_tree[0].files.size());

				path_tree[0].files.push_back(f);
			}
			else
			{
				filesystem_internal::dir_t d;

				d.full_path = tmp.string();
				d.filename = tmp.filename().string();

				d.tree_index.depth = 0;
				d.tree_index.index = static_cast<int8_t>(path_tree[0].dirs.size());
				
				recurseDir(0, d);
				path_tree[0].dirs.push_back(d);
			}
		}
		return path_tree.size() - 1;
	}

	size_t filesystem_internal::mount_t::AddDepth()
	{
		path_tree.emplace_back(collated_t{});
		path_tree.back().depth = static_cast<int8_t>(path_tree.size() - 1);

		return path_tree.size() - 1;
	}

	void filesystem_internal::mount_t::recurseDir(int8_t currDepth, dir_t& mountSubDir)
	{
		// Increase the depth if this expands the tree
		++currDepth;
		if (currDepth >= path_tree.size() - 1)
			AddDepth();

		FS::path currPath{ mountSubDir.full_path };
		FS::directory_iterator dir{ currPath };

		// initializing all the paths from this path
		for (auto& elem : dir)
		{
			FS::path tmp{ elem.path() };
			if (FS::is_regular_file(tmp))
			{
				filesystem_internal::file_t f;

				f.full_path = tmp.string();
				f.filename = tmp.filename().string();
				f.extension = tmp.extension().string();

				node_t node;
				node.depth = currDepth;
				node.index = static_cast<int8_t>(path_tree[currDepth].files.size());

				f.tree_index = node;

				mountSubDir.files.push_back(node);

				path_tree[currDepth].files.push_back(f);
			}
			else
			{
				filesystem_internal::dir_t d;

				d.full_path = tmp.string();
				d.filename = tmp.filename().string();

				node_t node;
				node.depth = currDepth;
				node.index = static_cast<int8_t>(path_tree[currDepth].dirs.size());

				d.tree_index = node;

				mountSubDir.sub_dirs.push_back(node);

				recurseDir(currDepth, d);
				path_tree[currDepth].dirs.push_back(d);
			}
		}

	}
}
