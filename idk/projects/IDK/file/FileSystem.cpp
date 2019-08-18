#include "stdafx.h"

#include "FileSystem.h"

#include <filesystem>
#include <utility>
#include <direct.h>
#include <tchar.h>
#include <stdio.h>  
#include <Shlwapi.h>
#include <iostream>
#include <utility>

namespace FS = std::filesystem;


namespace idk {
	
	void FileSystem::Init()
	{
		// Get the base directory. This is where the prog is run from.
		char base_dir_c[MAX_PATH] = { 0 };
		int bytes = GetModuleFileNameA(NULL, base_dir_c, MAX_PATH);
		// if (!_getcwd(base_dir_c, sizeof(base_dir_c)))
		if(bytes == 0)
		{
			std::cout << "[File System] Unable to get base directory." << std::endl;
		}
		base_dir = base_dir_c;
		auto pos = base_dir.find_last_of("\\");
		base_dir = base_dir.substr(0, pos + 1);

		// Mount(base_dir + "resource/", "/test_mount");
	}

	void FileSystem::Update()
	{
		// Clear all changed files here
		directory_watcher.ResolveAllChanges();

		// For every mount that is watched
		for (auto& mount : mounts)
		{
			if (mount.watching)
			{
				// For every depth
				for (auto& collated : mount.path_tree)
				{
					// For every dir in the current depth
					for (auto& dir : collated.dirs)
					{
						directory_watcher.UpdateWatchedDir(mount, dir);
					}
				}
			}
		}
	}

	void FileSystem::Shutdown()
	{
	}

	string FileSystem::GetFullPath(string_view mountPath) const
	{
		string mount_path{ mountPath.data() };

		if (mount_path[0] != '/')
			return string{};

		auto end_pos = mount_path.find_first_of('/', 1);
		
		string mount_key = mount_path.substr(0, end_pos);
		auto mount_index = validateMountPath(mountPath);
		if (mount_index >= 0)
		{
			// Check if there are even mounts. If this hits, something is terribly wrong...
			if (mounts.empty())
				throw("Something is terribly wrong. No mounts found.");

			// If the end_pos is the end of the string, it means there is only one dir and that is the mount dir
			if (end_pos == string::npos)
				return mounts[mount_index].full_path;
			else
				return mounts[mount_index].full_path + mount_path.substr(end_pos);
		}
		else
		{
			return string{};
		}
	}

	bool FileSystem::Exists(string_view mountPath) const
	{
		return ExistsFull(GetFullPath(mountPath));
	}

	bool FileSystem::ExistsFull(string_view fullPath) const
	{
		return GetFileAttributesA(fullPath.data()) != INVALID_FILE_ATTRIBUTES;
	}

	FileSystem_ErrorCode FileSystem::Mount(string_view fullPath, string_view mountPath, bool watch)
	{
		size_t curr_mount_index = mounts.size();
		
		string full_path{ fullPath.data() };
		string mount_path{ mountPath.data() };

		// All mountPaths must begin with '/'
		if (mountPath[0] != '/')
			return FILESYSTEM_BAD_ARGUMENT;

		if(mountPath.back() == '/' || mountPath.back() == '\\')
			return FILESYSTEM_BAD_ARGUMENT;

		if (full_path.back() == '/' || full_path.back() == '\\')
			full_path.pop_back();

		mount_table.emplace(mount_path, mounts.size());
		mounts.push_back(file_system_detail::fs_mount{});

		initMount(curr_mount_index, full_path, mount_path, watch);
		
		return FILESYSTEM_NOT_FOUND;
	}

	FileHandle FileSystem::OpenRead(string_view mountPath, bool binary_stream)
	{
		auto file_index = getFile(mountPath);
		if (file_index.mount_id < 0)
			return FileHandle{};

		auto& internal_file = getFile(file_index);
		auto& file_handle = file_handles[internal_file.handle_index];
		if (file_handle.IsOpenAndValid() == false)
			return FileHandle();

		FileHandle handle;
		handle.ref_count = file_handle.ref_count;
		handle.handle_index = internal_file.handle_index;

		auto permissions = binary_stream ? std::ios::in | std::ios::binary : std::ios::in;
		handle.stream.open(internal_file.full_path, permissions);
		if (!handle.stream.is_open())
			return FileHandle();

		file_handle.SetOpenFormat(file_system_detail::OPEN_FORMAT::READ_ONLY);

		return handle;
	}

	FileHandle FileSystem::OpenAppend(string_view mountPath, bool binary_stream)
	{
		UNREFERENCED_PARAMETER(binary_stream);
		auto file_index = getFile(mountPath);
		if (file_index.mount_id < 0)
		{
			// Means we need to create the file...
			string mount_path{ mountPath.data() };
			auto end_pos = mount_path.find_last_of('/');
			auto dir_index = getDir(mount_path.substr(0, end_pos));

			if (dir_index.mount_id < 0)
				return FileHandle();

			auto& dir = getDir(dir_index);
			string full_path = dir.full_path + mount_path.substr(end_pos + 1);

			std::ofstream{ full_path };

			FS::path p{ full_path };

			// Request a slot from mounts
			// Check if there are even mounts. If this hits, something is terribly wrong...
			if (mounts.empty())
				throw("Something is terribly wrong. No mounts found.");

			// initializing the fs_file
			auto slot = mounts[dir.tree_index.mount_id].RequestFileSlot(dir.tree_index.depth + 1);
			auto& f = getFile(slot);
			initFile(f, dir, p);

			auto& file_handle = file_handles[f.handle_index];

			FileHandle handle;
			handle.ref_count = file_handle.ref_count;
			handle.handle_index = f.handle_index;
			auto permissions = binary_stream ? std::ios::app | std::ios::binary : std::ios::app;
			handle.stream.open(f.full_path, permissions);
			if (!handle.stream.is_open())
				return FileHandle();

			file_handle.SetOpenFormat(file_system_detail::OPEN_FORMAT::WRITE_ONLY);
			return handle;
		}
		else
		{
			auto& internal_file = getFile(file_index);
			auto& file_handle = file_handles[internal_file.handle_index];
			if (file_handle.IsOpenAndValid() == false)
				return FileHandle();

			FileHandle handle;
			handle.ref_count = file_handle.ref_count;
			handle.handle_index = internal_file.handle_index;
			auto permissions = binary_stream ? std::ios::app | std::ios::binary : std::ios::app;
			handle.stream.open(internal_file.full_path, permissions);
			if (!handle.stream.is_open())
				return FileHandle();

			file_handle.SetOpenFormat(file_system_detail::OPEN_FORMAT::WRITE_ONLY);

			return handle;
		}
	}

	FileHandle FileSystem::OpenWrite(string_view mountPath, bool binary_stream)
	{
		auto file_index = getFile(mountPath);
		if (file_index.mount_id < 0)
		{
			// Means we need to create the file...
			string mount_path{ mountPath.data() };
			auto end_pos = mount_path.find_last_of('/');
			auto dir_index = getDir(mount_path.substr(0, end_pos));

			if (dir_index.mount_id < 0)
				return FileHandle();

			auto& dir = getDir(dir_index);
			string full_path = dir.full_path + mount_path.substr(end_pos);

			std::ofstream { full_path };

			FS::path p{ full_path };

			// Request a slot from mounts
			// Check if there are even mounts. If this hits, something is terribly wrong...
			if (mounts.empty())
				throw("Something is terribly wrong. No mounts found.");

			// Initializing the fs_file
			auto slot = mounts[dir.tree_index.mount_id].RequestFileSlot(dir.tree_index.depth + 1);
			auto& f = getFile(slot);
			initFile(f, dir, p);

			auto& file_handle = file_handles[f.handle_index];

			FileHandle handle;
			handle.ref_count = file_handle.ref_count;
			handle.handle_index = f.handle_index;
			auto permissions = binary_stream ? std::ios::out | std::ios::binary : std::ios::out;
			handle.stream.open(f.full_path, permissions);
			if (!handle.stream.is_open())
				return FileHandle();

			file_handle.SetOpenFormat(file_system_detail::OPEN_FORMAT::WRITE_ONLY);
			return handle;
		}
		else
		{
			auto& internal_file = getFile(file_index);
			auto& file_handle = file_handles[internal_file.handle_index];
			if (file_handle.IsOpenAndValid() == false)
				return FileHandle();

			FileHandle handle;
			handle.ref_count = file_handle.ref_count;
			handle.handle_index = internal_file.handle_index;
			auto permissions = binary_stream ? std::ios::out | std::ios::binary : std::ios::out;
			handle.stream.open(internal_file.full_path, permissions);
			if (!handle.stream.is_open())
				return FileHandle();

			file_handle.SetOpenFormat(file_system_detail::OPEN_FORMAT::WRITE_ONLY);

			return handle;
		}
	}

	void FileSystem::initMount(size_t index, string_view fullPath, string_view mountPath, bool watch)
	{
		if (!ExistsFull(fullPath))
		{
			std::cout << "[FILE SYSTEM] Bad path given to Mount function" << std::endl;
			return;
		}

		FS::path currPath{ fullPath };
		FS::directory_iterator dir{ currPath };

		// Initializing base variables of the mount
		file_system_detail::fs_mount& mount = mounts[index];
		mount.full_path = fullPath;
		mount.mount_path = mountPath;
		mount.watching = watch;
		mount.mount_index = static_cast<int8_t>(index);
		mount.AddDepth();

		// The root of this mount is depth 0
		file_system_detail::fs_dir d;

		d.full_path = fullPath;
		d.rel_path = currPath.relative_path().string();
		d.mount_path = mountPath;
		d.filename = currPath.filename().string();

		d.tree_index.mount_id = static_cast<int8_t>(index);
		d.tree_index.depth = 0;
		d.tree_index.index = static_cast<int8_t>(mount.path_tree[0].dirs.size());
		if (watch)
			directory_watcher.WatchDirectory(d);

		recurseSubDir(index, 0, d, watch);

		mount.path_tree[0].dirs.push_back(d);
	}

	void FileSystem::initFile(file_system_detail::fs_file& f, file_system_detail::fs_dir& p_dir, std::filesystem::path& p)
	{
		f.full_path = p.string();
		f.rel_path = p.relative_path().string();
		f.filename = p.filename().string();
		f.mount_path = p_dir.mount_path + "/" + f.filename;
		f.extension = p.extension().string();
		f.time = FS::last_write_time(p);
		f.parent = p_dir.tree_index;

		p_dir.files_map.emplace(f.filename, f.tree_index);
	}

	void FileSystem::initDir(file_system_detail::fs_dir& d, file_system_detail::fs_dir& p_dir, std::filesystem::path& p)
	{
		d.full_path = p.string();
		d.rel_path = p.relative_path().string();
		d.filename = p.filename().string();
		d.mount_path = p_dir.mount_path + "/" + d.filename;
		d.parent = p_dir.tree_index;
	}

	void FileSystem::recurseSubDir(size_t index, int8_t currDepth, file_system_detail::fs_dir& mountSubDir, bool watch)
	{
		file_system_detail::fs_mount& mount = mounts[index];
		size_t curr_handle_index = file_handles.size();

		// Increase the depth if this expands the tree
		++currDepth;
		if (currDepth >= mount.path_tree.size() - 1)
			mount.AddDepth();

		FS::path currPath{ mountSubDir.full_path };
		FS::directory_iterator dir{ currPath };

		// initializing all the paths from this path
		for (auto& elem : dir)
		{
			FS::path tmp{ elem.path() };
			if (FS::is_regular_file(tmp))
			{
				file_system_detail::fs_file f;

				f.tree_index.mount_id = static_cast<int8_t>(index);
				f.tree_index.depth = currDepth;
				f.tree_index.index = static_cast<int8_t>(mount.path_tree[currDepth].files.size());

				f.handle_index = curr_handle_index++;
				file_handles.emplace_back(f.tree_index);

				initFile(f, mountSubDir, tmp);
				mount.path_tree[currDepth].files.push_back(f);
			}
			else
			{
				file_system_detail::fs_dir d;

				file_system_detail::fs_key node;
				node.mount_id = static_cast<int8_t>(index);
				node.depth = currDepth;
				node.index = static_cast<int8_t>(mount.path_tree[currDepth].dirs.size());

				initDir(d, mountSubDir, tmp);

				d.tree_index = node;

				mountSubDir.sub_dirs.emplace(d.filename, node);
				if (watch)
					directory_watcher.WatchDirectory(d);

				recurseSubDir(index, currDepth, d, watch);
				mount.path_tree[currDepth].dirs.push_back(d);
			}
		}
	}

	file_system_detail::fs_file& FileSystem::getFile(file_system_detail::fs_key& node)
	{
		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (mounts.empty())
			throw("Something is terribly wrong. No mounts found.");
		if (!validateKey(node))
			return empty_file;
		return mounts[node.mount_id].path_tree[node.depth].files[node.index];
	}

	file_system_detail::fs_dir& FileSystem::getDir(file_system_detail::fs_key& node)
	{
		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (mounts.empty())
			throw("Something is terribly wrong. No mounts found.");
		if (!validateKey(node))
			return empty_dir;
		return mounts[node.mount_id].path_tree[node.depth].dirs[node.index];
	}

	const file_system_detail::fs_file& FileSystem::getFile(const file_system_detail::fs_key& node) const
	{
		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (mounts.empty())
			throw("Something is terribly wrong. No mounts found.");
		if (!validateKey(node))
			return empty_file;
		return mounts[node.mount_id].path_tree[node.depth].files[node.index];
	}

	const file_system_detail::fs_dir& FileSystem::getDir(const file_system_detail::fs_key& node) const
	{
		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (mounts.empty())
			throw("Something is terribly wrong. No mounts found.");
		if (!validateKey(node))
			return empty_dir;
		return mounts[node.mount_id].path_tree[node.depth].dirs[node.index];
	}

	file_system_detail::fs_key FileSystem::getFile(string_view mountPath)
	{
		file_system_detail::fs_key empty_node;
		// First check if the path exists
		if (!Exists(mountPath))
			return empty_node;

		int mount_index = validateFileMountPath(mountPath);
		if (mount_index < 0)
			return empty_node;

		// Check if there are mounts. If this hits, something is terribly wrong...
		if (mounts.empty())
			throw("Something is terribly wrong. No mounts found.");

		auto& mount = mounts[mount_index];
		auto tokenized_path = tokenizePath(mountPath);

		// The parent directory of the file is one depth higher and we -1 again because we don't count the file token.
		int8_t dir_depth = static_cast<int8_t>(tokenized_path.size() - 2);

		// Get the correct directory
		if (dir_depth == 0)
		{
			// Special case. If depth is 0, there is only 1 directory inside this depth (the mount directory).
			auto& sub_dir = mount.path_tree[dir_depth].dirs[0];

			// Find file within the sub_dir
			auto result = sub_dir.files_map.find(tokenized_path[1]);
			if (result != sub_dir.files_map.end())
				return result->second;
		}
		else
		{
			for (auto& sub_dir : mount.path_tree[dir_depth].dirs)
			{
				// Find file within the sub_dir
				auto result = sub_dir.files_map.find(tokenized_path.back());
				if (result != sub_dir.files_map.end())
					return result->second;
			}
		}

		return empty_node;
	}

	file_system_detail::fs_key FileSystem::getDir(string_view mountPath)
	{
		file_system_detail::fs_key empty_node;
		// First check if the path exists
		if (!Exists(mountPath))
			return empty_node;

		int mount_index = validateDirMountPath(mountPath);
		if (mount_index < 0)
			return empty_node;

		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (mounts.empty())
			throw("Something is terribly wrong. No mounts found.");

		auto& mount = mounts[mount_index];
		auto tokenized_path = tokenizePath(mountPath);

		if (tokenized_path.back().empty())
			tokenized_path.pop_back();

		// The parent directory of the sub directory is one depth higher
		int8_t dir_depth = static_cast<int8_t>(tokenized_path.size() - 2);

		// Get the correct directory
		if (dir_depth < 0)
		{
			// Special case. If depth is 0, that means we are looking for the mount dir
			auto& sub_dir = mount.path_tree[0].dirs[0];

			return sub_dir.tree_index;
		}
		else
		{
			for (auto& sub_dir : mount.path_tree[dir_depth].dirs)
			{
				// Find file within the sub_dir
				auto result = sub_dir.sub_dirs.find(tokenized_path.back());
				if (result != sub_dir.sub_dirs.end())
					return result->second;
			}
		}

		return empty_node;
	}

	size_t FileSystem::addFileHandle(const file_system_detail::fs_key& node)
	{
		file_handles.emplace_back(node);
		return file_handles.size() - 1;
	}

	vector<string> FileSystem::tokenizePath(string_view fullPath) const
	{
		string full_path{ fullPath };
		size_t start = 1, end = 0;
		string token;
		vector<string> output;

		while (end != string::npos)
		{
			end = full_path.find_first_of("/\\", start);
			// If at end, use length=maxLength.  Else use length=end-start.
			token = full_path.substr(start, (end == string::npos) ? string::npos : end - start);
			output.push_back(token);

			start = full_path.find_first_not_of("/\\", end);
			if (start == string::npos) end = string::npos;
		}
		return output;
	}

	bool FileSystem::validateKey(const file_system_detail::fs_key& key) const
	{
		return (key.mount_id >= 0) && (key.depth >= 0) && (key.index >= 0);
	}

	int FileSystem::validateMountPath(string_view mountPath) const
	{
		string mount_path{ mountPath.data() };

		if (mount_path[0] != '/')
			return -1;

		auto end_pos = mount_path.find_first_of('/', 1);
		
		string mount_key = mount_path.substr(0, end_pos);
		auto mount_index = mount_table.find(mount_key);
		if (mount_index != mount_table.end())
			return static_cast<int>(mount_index->second);
		else
			return -1;
	}

	int FileSystem::validateFileMountPath(string_view mountPath) const
	{
		string mount_path{ mountPath.data() };

		if (mount_path[0] != '/')
			return -1;

		auto end_pos = mount_path.find_first_of('/', 1);
		if (end_pos == string::npos)
			return -1;

		string mount_key = mount_path.substr(0, end_pos);
		auto mount_index = mount_table.find(mount_key);
		if (mount_index != mount_table.end())
			return static_cast<int>(mount_index->second);
		else
			return -1;
	}

	int FileSystem::validateDirMountPath(string_view mountPath) const
	{
		string mount_path{ mountPath.data() };

		if (mount_path[0] != '/')
			return -1;

		auto end_pos = mount_path.find_first_of('/', 1);
		
		string mount_key = mount_path.substr(0, end_pos);
		auto mount_index = mount_table.find(mount_key);
		if (mount_index != mount_table.end())
			return static_cast<int>(mount_index->second);
		else
			return -1;
	}

	bool FileSystem::validateHandle(const FileHandle& handle) const
	{
		if(handle.handle_index < 0)
			return false;

		auto& internal_handle = file_handles[handle.handle_index];
		return handle.ref_count == internal_handle.ref_count;
	}

	
	FileSystem_ErrorCode FileSystem::Mkdir(string_view mountPath)
	{
		UNREFERENCED_PARAMETER(mountPath);
		// Testing for now.
		// FS::create_directories(FS::path{ "C:/Users/Joseph/Desktop/GIT/idk_legacy/Koboru/Koboru/resource/editor/test" });
		return FILESYSTEM_OK;
	}

	void FileSystem::DumpMounts() const
	{
		std::cout << std::boolalpha;
		for (auto& mount_index : mount_table)
		{
			std::cout << "Total mounts: " << mounts.size() << std::endl;
			std::cout << "Dumping mounts..." << std::endl;
			
			auto& mount = mounts[mount_index.second];
			// std::cout << "Mount: " << mount_index.first << "\n";
			dumpMount(mount);
		}
		std::cout << std::noboolalpha;
	}

	void FileSystem::dumpMount(const file_system_detail::fs_mount& mount) const
	{
		std::cout << "Mount: " << mount.mount_path << "\n";
		dumpDir(mount.path_tree[0].dirs[0], "\t");
		
	}

	void FileSystem::dumpDir(const file_system_detail::fs_dir& sub_dir, string prefix) const
	{
		prefix += "|";
		std::cout	<< prefix << "Directory: " << sub_dir.filename << "\n"
					<< prefix << "Rel Path: "			<< sub_dir.rel_path << "\n"
					<< prefix << "Mount Path: "			<< sub_dir.mount_path << "\n"
					<< prefix << "Parent Full Path: "	<< getDir(sub_dir.parent).full_path << "\n"
					<< prefix << "Parent Mount Path: "	<< getDir(sub_dir.parent).mount_path << "\n"
					<< prefix << "Num Files: "			<< sub_dir.files_map.size() << "\n"
					<< prefix << "Num Sub-Dirs: "		<< sub_dir.sub_dirs.size() << "\n";

		std::cout << prefix << "Files: " << "\n";
		string file_prefix = prefix + "  ";
		for (auto& file_index : sub_dir.files_map)
		{
			auto& file = getFile(file_index.second);
			std::cout << file_prefix  << "*" << "Filename: " << file.filename << "\n";
			std::cout << file_prefix  << "*" << "Extension: " << file.extension << "\n";
			std::cout << file_prefix  << "*" << "Rel Path: " << file.rel_path << "\n";
			std::cout << file_prefix  << "*" << "Mount Path: " << file.mount_path << "\n";
			std::cout << file_prefix  << "*" << "Parent Dir Check: " << (file.parent == sub_dir.tree_index) << "\n";
		}

		if (sub_dir.sub_dirs.size() == 0)
			return;
		std::cout << prefix << "Sub-Dirs: " << "\n";
		for (auto& dir_index : sub_dir.sub_dirs)
		{
			auto& sub_sub_dir = getDir(dir_index.second);
			dumpDir(sub_sub_dir, prefix + "  ");
		}
	}
	

	file_system_detail::fs_key FileSystem::RequestFileSlot(file_system_detail::fs_mount& mount, int8_t depth)
	{
		auto check_free_index = std::find_if( mount.path_tree[depth].files.begin(),
											  mount.path_tree[depth].files.end(),
											  [](const file_system_detail::fs_file& f) { return !f.valid; });

		if (check_free_index != mount.path_tree[depth].files.end())
		{
			auto& file_handle = Core::GetSystem<FileSystem>().file_handles[check_free_index->handle_index];
			file_handle.Reset();
			file_handle.internal_id = check_free_index->tree_index;

			return check_free_index->tree_index;
		}
		else
		{
			mount.path_tree[depth].files.push_back(file_system_detail::fs_file{});
			auto& file = mount.path_tree[depth].files.back();

			file.tree_index.mount_id = mount.mount_index;
			file.tree_index.depth = depth;
			file.tree_index.index = static_cast<int8_t>(mount.path_tree[depth].files.size() - 1);
			file.handle_index = Core::GetSystem<FileSystem>().addFileHandle(file.tree_index);

			return file.tree_index;
		}
	}
}