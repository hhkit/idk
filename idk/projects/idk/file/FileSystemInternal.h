#pragma once
#include <Windows.h>
#include <filesystem>

#include "FileSystemIncludes.h"

namespace idk::file_system_detail
{
	

	enum class OPEN_FORMAT
	{
		READ_ONLY,
		WRITE_ONLY,
		NOT_OPEN
	};			  
				  
	struct fs_key
	{
		fs_key() = default;
		fs_key(int8_t m, int8_t d, int16_t i);

		bool IsValid() const;
		bool operator == (const fs_key& rhs) const;
		
		int8_t _mount_id = -1;
		int8_t _depth = -1;
		int16_t _index = -1;
	};

	struct fs_file_detail
	{
		// First bit = valid?
		// Others = open type?
		byte	_mask = byte{ 0x00 };

		int16_t _ref_count = 0;

		void Reset();
		void Invalidate();
		bool IsOpenAndValid() const;
		void SetOpenFormat(OPEN_FORMAT format);
	};

	struct fs_file
	{
		string _full_path;
		string _rel_path;
		string _mount_path;
		string _filename;
		string _extension;

		fs_key _parent;
		fs_key _tree_index;

		// For file watching
		FS_CHANGE_STATUS _change_status = FS_CHANGE_STATUS::NO_CHANGE;
		std::filesystem::file_time_type _time;

		int16_t RefCount()	const { return _ref_count; }
		bool	IsOpen()	const { return _open_mode != FS_PERMISSIONS::NONE; }
		bool	IsValid()	const { return _valid; }

		void SetOpenMode(FS_PERMISSIONS perms)	{ _open_mode = perms; }
		void SetValid	(bool is_valid)			{ _valid = is_valid; }

		void IncRefCount()						{ ++_ref_count; }
		void DecRefCount()						{ ++_ref_count; }
		
	private:
		// A PathHandle can be pointing to a valid fs_file but wrong ref_count
		bool _valid = true;
		int16_t _ref_count = 0;

		FS_PERMISSIONS _open_mode = FS_PERMISSIONS::NONE;
	};

	struct fs_dir
	{
		string _full_path;
		string _rel_path;
		string _mount_path;
		string _filename;

		hash_table<string, fs_key> _sub_dirs;
		hash_table<string, fs_key> _files_map;
		
		fs_key _parent;
		fs_key _tree_index;

		// For file watching
		HANDLE	_dir_handle;
		HANDLE	_watch_handle[3];
		DWORD	_status;

		FS_CHANGE_STATUS _change_status = FS_CHANGE_STATUS::NO_CHANGE;

		int16_t RefCount()	const { return _ref_count; }
		bool	IsValid()	const { return _valid; }

		void SetValid(bool is_valid) { _valid = is_valid; }

		void IncRefCount() { ++_ref_count; }
		void DecRefCount() { ++_ref_count; }

	private:
		// A PathHandle can be pointing to a valid fs_file but wrong ref_count
		bool _valid = true;
		int16_t _ref_count = 0;
	};

	struct fs_collated
	{
		// hash_table<string, uint64_t> files_map;
		vector<fs_file> _files;
		vector<fs_dir>	_dirs;

		int8_t			_depth;
	};

	struct fs_mount
	{
		// Each index signifies the depth of the tree
		vector<fs_collated> _path_tree;

		string				_full_path;
		string				_mount_path;

		int8_t				_mount_index;
		bool				_watching = true;

		size_t AddDepth();
		// fs_key RequestFileSlot(int8_t depth);
	};

	

	
}