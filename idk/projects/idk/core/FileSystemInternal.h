#pragma once

namespace idk
{
	namespace filesystem_internal
	{
		struct node_t
		{
			int8_t depth = -1;
			int8_t index = -1;
		};

		struct file_t
		{
			string full_path;
			string filename;
			string extension;

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
			size_t Init(const string& fullPath, const string& mountPath);
			size_t AddDepth();
		
			// Each index signifies the depth of the tree
			vector<collated_t> path_tree;

			string full_path;
			string mount_path;

		private:
			void recurseDir(int8_t depth, dir_t& dir);
		};
	}

	
}