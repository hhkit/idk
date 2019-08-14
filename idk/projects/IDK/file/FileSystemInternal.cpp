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

	file_system_internal::file_t& file_system_internal::mount_t::GetFile(node_t node)
	{
		return path_tree[node.depth].files[node.index];
	}

	file_system_internal::file_handle_t::file_handle_t(int8_t mount, int8_t depth, int8_t index)
		: internal_id{ mount, depth, index }, is_open{ false }, open_type{ 0x0 }
	{
	}

	file_system_internal::file_handle_t::file_handle_t(node_t node)
		: internal_id{ node }, is_open{ false }, open_type{ 0x0 }
	{
	}

	file_system_internal::node_t::node_t(int8_t m, int8_t d, int8_t i)
		: mount_id{ m }, depth{ d }, index{ i }
	{
	}
}


