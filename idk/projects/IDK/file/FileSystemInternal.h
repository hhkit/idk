#pragma once
#include <Windows.h>
#include <filesystem>
namespace idk::file_system_internal
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
				  
	struct node_t
	{
		node_t() = default;
		node_t(int8_t m, int8_t d, int8_t i);

		bool operator == (const node_t& rhs) const;

		int8_t mount_id = -1;
		int8_t depth = -1;
		int8_t index = -1;
	};

	struct file_t
	{
		string full_path;
		string filename;
		string extension;

		int64_t handle_index = -1;

		node_t parent;
		node_t tree_index;

		bool valid = true;

		// For file watching
		CHANGE_STATUS change_status = NO_CHANGE;
		std::filesystem::file_time_type time;
	};

	struct dir_t
	{
		string full_path;
		string filename;

		hash_table<string, node_t> sub_dirs;
		hash_table<string, node_t> files_map;
		
		node_t parent;
		node_t tree_index;

		// For file watching
		HANDLE dir_handle;
		HANDLE watch_handle[3];
		DWORD status;

		bool valid = true;

		CHANGE_STATUS change_status = NO_CHANGE;
	};

	struct collated_t
	{
		// hash_table<string, uint64_t> files_map;
		vector<file_t> files;
		vector<dir_t> dirs;

		int8_t depth;
	};

	struct mount_t
	{
		// Each index signifies the depth of the tree
		vector<collated_t> path_tree;

		string full_path;
		string mount_path;

		int8_t mount_index;
		bool watching = true;

		size_t AddDepth();
		node_t RequestFileSlot(int8_t depth);
	};

	struct file_handle_t
	{
		file_handle_t(int8_t mount, int8_t depth, int8_t index);
		file_handle_t(node_t node);

		void Reset();
		void Invalidate();
		bool IsOpenAndValid() const;
		void SetOpenFormat(OPEN_FORMAT format);
		
		// First bit = valid?
		// Others = open type?
		byte mask = byte{0x00};

		node_t internal_id;
		int64_t ref_count = 0;
		bool allow_write = false;
	};

	
}