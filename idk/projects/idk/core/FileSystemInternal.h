#pragma once

namespace idk::file_system_internal
{
	struct node_t
	{
		node_t() = default;
		node_t(int8_t d, int8_t i);
		int8_t depth = -1;
		int8_t index = -1;
	};

	struct file_t
	{
		string full_path;
		string filename;
		string extension;

		size_t handle_index;

		node_t parent;
		node_t tree_index;
	};

	struct dir_t
	{
		string full_path;
		string filename;

		vector<node_t> sub_dirs;
		vector<node_t> files;

		node_t parent;
		node_t tree_index;
	};

	struct collated_t
	{
		vector<file_t> files;
		vector<dir_t> dirs;

		int8_t depth;
	};

	struct mount_t
	{
		size_t AddDepth();

		// Each index signifies the depth of the tree
		vector<collated_t> path_tree;

		string full_path;
		string mount_path;

		size_t num_files;
	};

	struct file_handle_t
	{
		file_handle_t(int8_t mount, int8_t depth, int8_t index);
		file_handle_t(int8_t mount, node_t node);
		bool is_open = false;
		byte open_type = byte{0x0};

		int8_t mount_id;
		node_t internal_id;
	};

	
}