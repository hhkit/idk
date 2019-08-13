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

	file_system_internal::file_handle_t::file_handle_t(int8_t mount, int8_t depth, int8_t index)
		:mount_id{ mount }, internal_id{ depth, index }, is_open{ false }, open_type{ 0x0 }
	{
	}

	file_system_internal::file_handle_t::file_handle_t(int8_t mount, node_t node)
		: mount_id{ mount }, internal_id{ node }, is_open{ false }, open_type{ 0x0 }
	{
	}

	file_system_internal::node_t::node_t(int8_t d, int8_t i)
		: depth{ d }, index{ i }
	{
	}
}


