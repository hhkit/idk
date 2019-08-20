#pragma once
#include <Windows.h>
#include <filesystem>
namespace idk::file_system_detail
{
	enum class CHANGE_STATUS
	{
		NO_CHANGE,
		CREATED,
		DELETED,
		RENAMED,
		WRITTEN
	};

	enum class OPEN_FORMAT
	{
		READ_ONLY	= 0x02,
		WRITE_ONLY	= 0x04,
		READ_WRITE	= 0x08,
	};			  
				  
	struct fs_key
	{
		fs_key() = default;
		fs_key(int8_t m, int8_t d, int8_t i);

		bool IsValid() const;
		bool operator == (const fs_key& rhs) const;
		
		int8_t _mount_id = -1;
		int8_t _depth = -1;
		int8_t _index = -1;
	};

	struct fs_file
	{
		string _full_path;
		string _rel_path;
		string _mount_path;
		string _filename;
		string _extension;

		int64_t _handle_index = -1;

		fs_key _parent;
		fs_key _tree_index;

		bool _valid = true;

		// For file watching
		CHANGE_STATUS _change_status = CHANGE_STATUS::NO_CHANGE;
		std::filesystem::file_time_type _time;
	};

	struct fs_dir
	{
		string _full_path;
		string _mount_path;
		string _rel_path;
		string _filename;

		hash_table<string, fs_key> _sub_dirs;
		hash_table<string, fs_key> _files_map;
		
		fs_key _parent;
		fs_key _tree_index;

		// For file watching
		HANDLE	_dir_handle;
		HANDLE	_watch_handle[3];
		DWORD	_status;

		bool _valid = true;

		CHANGE_STATUS _change_status = CHANGE_STATUS::NO_CHANGE;
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

	struct fs_file_handle
	{
		fs_file_handle(int8_t mount, int8_t depth, int8_t index);
		fs_file_handle(const fs_key& node);

		// First bit = valid?
		// Others = open type?
		byte	_mask = byte{0x00};

		fs_key	_internal_id;
		int64_t _ref_count = 0;
		bool	_allow_write = false;

		void Reset();
		void Invalidate();
		bool IsOpenAndValid() const;
		void SetOpenFormat(OPEN_FORMAT format);
	};

	
}