#include "stdafx.h"
#include "FileSystemInternal.h"
#include "FileSystem.h"

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
			auto& file_handle = Core::GetSystem<FileSystem>().file_handles[check_free_index->handle_index];
			file_handle.Reset();
			file_handle.internal_id = check_free_index->tree_index;

			return check_free_index->tree_index;
		}
		else
		{
			path_tree[depth].files.push_back(file_t{});
			auto& file = path_tree[depth].files.back();

			file.tree_index.mount_id = mount_index;
			file.tree_index.depth = depth;
			file.tree_index.index = static_cast<int8_t>(path_tree[depth].files.size() - 1);
			file.handle_index = Core::GetSystem<FileSystem>().addFileHandle(file.tree_index);

			return file.tree_index;
		}
	}

	file_system_internal::file_handle_t::file_handle_t(int8_t mount, int8_t depth, int8_t index)
		: internal_id{ mount, depth, index }
	{
	}

	file_system_internal::file_handle_t::file_handle_t(node_t node)
		: internal_id{ node }
	{
	}

	void file_system_internal::file_handle_t::Reset()
	{
		mask = byte{ 0x00 };
	}

	void file_system_internal::file_handle_t::Invalidate()
	{
		// Set first bit to 1
		mask ^= byte{0x01};

		// Inc this because the opaque handle will check against this.
		++ref_count;
	}

	bool file_system_internal::file_handle_t::IsOpenAndValid() const
	{
		return (mask & byte{ 0x00 }) == byte{ 0x00 };
	}

	void file_system_internal::file_handle_t::SetOpenFormat(OPEN_FORMAT format)
	{
		mask |= byte{ static_cast<unsigned char>(format) };
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


