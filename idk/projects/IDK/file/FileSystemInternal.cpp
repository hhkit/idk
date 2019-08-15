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

	size_t file_system_internal::mount_t::AddDepth()
	{
		path_tree.emplace_back(collated_t{});
		path_tree.back().depth = static_cast<int8_t>(path_tree.size() - 1);

		return path_tree.size() - 1;
	}

	file_system_internal::node_t file_system_internal::mount_t::RequestFileSlot(int8_t depth)
	{
		auto check_free_index = std::find_if(	path_tree[depth].files.begin(),
												path_tree[depth].files.end(),
												[](const file_system_internal::file_t& f) { return !f.valid; });

		if (check_free_index != path_tree[depth].files.end())
		{
			return check_free_index->tree_index;
		}
		else
		{
			path_tree[depth].files.push_back(file_t{});
			auto& file = path_tree[depth].files.back();

			file.tree_index.mount_id = mount_index;
			file.tree_index.depth = depth;
			file.tree_index.index = path_tree[depth].files.size() - 1;

			return file.tree_index;
		}
	}

	void file_system_internal::mount_t::AddFile(file_t& file)
	{
		auto check_free_index = std::find_if(	path_tree[file.tree_index.depth].files.begin(),
												path_tree[file.tree_index.depth].files.end(),
												[](const file_system_internal::file_t& f) { return !f.valid; });

		if (check_free_index != path_tree[file.tree_index.depth].files.end())
		{
			file.tree_index = check_free_index->tree_index;
			*check_free_index = file;
		}
		else
		{
			file.tree_index.index = static_cast<int8_t>(path_tree[file.tree_index.depth].files.size());
			path_tree[file.tree_index.depth].files.emplace_back(file);
		}
	}

	file_system_internal::file_t& file_system_internal::mount_t::GetFile(node_t node)
	{
		return path_tree[node.depth].files[node.index];
	}

	file_system_internal::file_handle_t::file_handle_t(int8_t mount, int8_t depth, int8_t index)
		: internal_id{ mount, depth, index }
	{
	}

	file_system_internal::file_handle_t::file_handle_t(node_t node)
		: internal_id{ node }
	{
	}

	void file_system_internal::file_handle_t::Invalidate()
	{
		byte b {0x01};
		mask ^= b;

		// Inc this because the opaque handle will check against this.
		++ref_count;
	}

	file_system_internal::node_t::node_t(int8_t m, int8_t d, int8_t i)
		: mount_id{ m }, depth{ d }, index{ i }
	{
	}
	bool file_system_internal::node_t::operator==(const node_t& rhs) const
	{
		return (mount_id == rhs.mount_id && depth == rhs.depth && index == rhs.index);
	}
}


