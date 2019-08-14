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

	struct node_t
	{
		node_t() = default;
		node_t(int8_t m, int8_t d, int8_t i);

		int8_t mount_id = -1;
		int8_t depth = -1;
		int8_t index = -1;
	};

	struct file_t
	{
		string full_path;
		string filename;
		string extension;

		int64_t handle_index;

		node_t parent;
		node_t tree_index;

		// For file watching
		CHANGE_STATUS change_status = NO_CHANGE;
		std::filesystem::file_time_type time;
	};

	struct dir_t
	{
		string full_path;
		string filename;

		vector<node_t> sub_dirs;
		hash_table<string, node_t> files_map;
		
		node_t parent;
		node_t tree_index;

		// For file watching
		HANDLE dir_handle;
		HANDLE watch_handle[3];
		DWORD status;

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

		size_t num_files;
		bool watching = true;

		size_t AddDepth();
		file_t& GetFile(node_t node);
	};

	struct file_handle_t
	{
		file_handle_t(int8_t mount, int8_t depth, int8_t index);
		file_handle_t(node_t node);
		bool is_open = false;
		byte open_type = byte{0x0};

		node_t internal_id;
	};

	
}