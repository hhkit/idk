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

	size_t file_system_detail::fs_mount::AddDepth()
	{
		_path_tree.emplace_back(fs_collated{});
		_path_tree.back()._depth = static_cast<int8_t>(_path_tree.size() - 1);

		return _path_tree.size() - 1;
	}

	/*file_system_detail::fs_key file_system_detail::fs_mount::RequestFileSlot(int8_t depth)
	{
		auto check_free_index = std::find_if(	path_tree[depth].files.begin(),
												path_tree[depth].files.end(),
												[](const file_system_detail::fs_file& f) { return !f.valid; });

		if (check_free_index != path_tree[depth].files.end())
		{
			auto& file_handle = Core::GetSystem<FileSystem>().file_handles[check_free_index->handle_index];
			file_handle.Reset();
			file_handle.internal_id = check_free_index->tree_index;

			return check_free_index->tree_index;
		}
		else
		{
			path_tree[depth].files.push_back(fs_file{});
			auto& file = path_tree[depth].files.back();

			file.tree_index.mount_id = mount_index;
			file.tree_index.depth = depth;
			file.tree_index.index = static_cast<int8_t>(path_tree[depth].files.size() - 1);
			file.handle_index = Core::GetSystem<FileSystem>().addFileHandle(file.tree_index);

			return file.tree_index;
		}
	}*/

	file_system_detail::fs_file_handle::fs_file_handle(int8_t mount, int8_t depth, int8_t index)
		: _internal_id{ mount, depth, index }
	{
	}

	file_system_detail::fs_file_handle::fs_file_handle(const fs_key& node)
		: _internal_id{ node }
	{
	}

	void file_system_detail::fs_file_handle::Reset()
	{
		_mask = byte{ 0x00 };
	}

	void file_system_detail::fs_file_handle::Invalidate()
	{
		// Set first bit to 1
		_mask ^= byte{0x01};

		// Inc this because the opaque handle will check against this.
		++_ref_count;
	}

	bool file_system_detail::fs_file_handle::IsOpenAndValid() const
	{
		return _mask == byte{ 0x00 };
	}

	void file_system_detail::fs_file_handle::SetOpenFormat(OPEN_FORMAT format)
	{
		_mask |= byte{ static_cast<unsigned char>(format) };
	}

	file_system_detail::fs_key::fs_key(int8_t m, int8_t d, int8_t i)
		: _mount_id{ m }, _depth{ d }, _index{ i }
	{
	}
	bool file_system_detail::fs_key::IsValid() const
	{
		return (_mount_id >= 0 && _depth >= 0 && _index >= 0);
	}

	bool file_system_detail::fs_key::operator==(const fs_key& rhs) const
	{
		return (_mount_id == rhs._mount_id && _depth == rhs._depth && _index == rhs._index);
	}
}


