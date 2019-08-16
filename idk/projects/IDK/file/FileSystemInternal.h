#pragma once
#include <Windows.h>
#include <filesystem>
namespace idk::file_system_detail
{
	enum CHANGE_STATUS
	{
		NO_CHANGE,
		CREATED,
		DELETED,
		RENAMED,
		WRITTEN
	};

	enum OPEN_FORMAT
	{
		READ_ONLY	= 0x02,
		WRITE_ONLY	= 0x04,
		READ_WRITE	= 0x08,
	};			  
				  
	struct fs_key
	{
		fs_key() = default;
		fs_key(int8_t m, int8_t d, int8_t i);

		bool operator == (const fs_key& rhs) const;

		int8_t mount_id = -1;
		int8_t depth = -1;
		int8_t index = -1;
	};

	struct fs_file
	{
		string full_path;
		string filename;
		string extension;

		int64_t handle_index = -1;

		fs_key parent;
		fs_key tree_index;

		bool valid = true;

		// For file watching
		CHANGE_STATUS change_status = NO_CHANGE;
		std::filesystem::file_time_type time;
	};

	struct fs_dir
	{
		string full_path;
		string filename;

		hash_table<string, fs_key> sub_dirs;
		hash_table<string, fs_key> files_map;
		
		fs_key parent;
		fs_key tree_index;

		// For file watching
		HANDLE dir_handle;
		HANDLE watch_handle[3];
		DWORD status;

		bool valid = true;

		CHANGE_STATUS change_status = NO_CHANGE;
	};

	struct fs_collated
	{
		// hash_table<string, uint64_t> files_map;
		vector<fs_file> files;
		vector<fs_dir> dirs;

		int8_t depth;
	};

	struct fs_mount
	{
		// Each index signifies the depth of the tree
		vector<fs_collated> path_tree;

		string full_path;
		string mount_path;

		int8_t mount_index;
		bool watching = true;

		size_t AddDepth();
		fs_key RequestFileSlot(int8_t depth);
	};

	struct fs_file_handle
	{
		fs_file_handle(int8_t mount, int8_t depth, int8_t index);
		fs_file_handle(fs_key node);

		void Reset();
		void Invalidate();
		bool IsOpenAndValid() const;
		void SetOpenFormat(OPEN_FORMAT format);
		
		// First bit = valid?
		// Others = open type?
		byte mask = byte{0x00};

		fs_key internal_id;
		int64_t ref_count = 0;
		bool allow_write = false;
	};

	
}