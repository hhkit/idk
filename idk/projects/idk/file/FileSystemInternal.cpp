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

	void file_system_detail::fs_file_detail::Reset()
	{
		_mask = byte{ 0x00 };
	}

	void file_system_detail::fs_file_detail::Invalidate()
	{
		// Set first bit to 1
		_mask ^= byte{0x01};

		// Inc this because the opaque handle will check against this.
		++_ref_count;
	}

	bool file_system_detail::fs_file_detail::IsOpenAndValid() const
	{
		return _mask == byte{ 0x00 };
	}

	void file_system_detail::fs_file_detail::SetOpenFormat(OPEN_FORMAT format)
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


