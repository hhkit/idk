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
	
#pragma region ISystem Stuff
	void FileSystem::Init()
	{
		// Get the base directory. This is where the prog is run from.
		char buffer[MAX_PATH] = { 0 };

		// Get the program directory
		int bytes = GetModuleFileNameA(NULL, buffer, MAX_PATH);
		if(bytes == 0)
			std::cout << "[File System] Unable to get program directory." << std::endl;
		_exe_dir = buffer;
		auto pos = _exe_dir.find_last_of("\\");
		_exe_dir = _exe_dir.substr(0, pos);

		// Get the solution directory
		if (!_getcwd(buffer, sizeof(buffer)))
			std::cout << "[File System] Unable to get solution directory." << std::endl;
		_sol_dir = buffer;

		// Get App Data directory
		char* env_buff;
		size_t len;
		auto err = _dupenv_s(&env_buff, &len, "appdata");
		if (err)
			std::cout << "[File System] Unable to get solution directory." << std::endl;
		_app_data_dir = env_buff;
		free(env_buff);
	}

	void FileSystem::Update()
	{
		// Clear all changed files here
		_directory_watcher.ResolveAllChanges();

		// For every mount that is watched
		for (auto& mount : _mounts)
		{
			if (mount._watching)
			{
				// For every depth
				for (auto& collated : mount._path_tree)
				{
					// For every dir in the current depth
					for (auto& dir : collated._dirs)
					{
						_directory_watcher.UpdateWatchedDir(mount, dir);
					}
				}
			}
		}
	}

	void FileSystem::Shutdown()
	{
	}
#pragma endregion ISystem Stuff

#pragma region General Getters

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
			if (_mounts.empty())
				throw("Something is terribly wrong. No mounts found.");

			// If the end_pos is the end of the string, it means there is only one dir and that is the mount dir
			if (end_pos == string::npos)
				return _mounts[mount_index]._full_path;
			else
				return _mounts[mount_index]._full_path + mount_path.substr(end_pos);
		}
		else
		{
			return string{};
		}
	}

	PathHandle FileSystem::GetPath(string_view mountPath) const
	{
		PathHandle ret = GetFile(mountPath);
		if (!ret)
			return GetDir(mountPath);
		return ret;
	}

	PathHandle FileSystem::GetFile(string_view mountPath) const
	{
		auto file_index = getFile(mountPath);
		if (file_index.IsValid() == false)
			return PathHandle{};

		return PathHandle{ file_index, true };
	}

	PathHandle FileSystem::GetDir(string_view mountPath) const
	{
		auto dir_index = getDir(mountPath);
		if (dir_index.IsValid() == false)
			return PathHandle{};

	
		return PathHandle{ dir_index, false};
	}

	bool FileSystem::Exists(string_view mountPath) const
	{
		return ExistsFull(GetFullPath(mountPath));
	}

	bool FileSystem::ExistsFull(string_view fullPath) const
	{
		return FS::exists(FS::path{ fullPath.data() });
	}

#pragma endregion General Getters

#pragma region Directory Specific

	vector<PathHandle> FileSystem::GetFilesWithExtension(string_view mountPath, string_view extension, FS_FILTERS filters) const
	{
		auto dir_index = getDir(mountPath);
		PathHandle h{ dir_index, false };
		return h.GetFilesWithExtension(extension, filters);
	}

	vector<PathHandle> FileSystem::GetPaths(string_view mountPath, FS_FILTERS filters, string_view ext) const
	{
		auto dir_index = getDir(mountPath);
		PathHandle h{ dir_index, false };
		return h.GetPaths(filters, ext);
	}

#pragma endregion Directory Specific

#pragma region File Changes

	vector<PathHandle> FileSystem::QueryFileChangesAll() const
	{
		vector<PathHandle> handles;
		for (auto& key : _directory_watcher.changed_files)
		{
			auto& file = getFile(key);
			
			PathHandle h{ file._tree_index, true };
			handles.emplace_back(h);
		}

		return handles;
	}

	vector<PathHandle> FileSystem::QueryFileChangesByExt(string_view ext) const
	{
		vector<PathHandle> handles;
		for (auto& key : _directory_watcher.changed_files)
		{
			auto& file = getFile(key);

			if (file._extension == ext)
			{
				PathHandle h{ file._tree_index, true };
				handles.emplace_back(h);
			}
		}

		return handles;
	}

	vector<PathHandle> FileSystem::QueryFileChangesByChange(FS_CHANGE_STATUS change) const
	{
		vector<PathHandle> handles;
		for (auto& key : _directory_watcher.changed_files)
		{
			auto& file = getFile(key);
			if (file._change_status == change)
			{
				PathHandle h{ file._tree_index, true };
				handles.emplace_back(h);
			}
		}

		return handles;
	}

#pragma endregion File Changes
	
#pragma region Utility
	void FileSystem::Mount(string_view fullPath, string_view mountPath, bool watch)
	{
		size_t curr_mount_index = _mounts.size();
		
		string full_path{ fullPath.data() };
		string mount_path{ mountPath.data() };

		if (_mount_table.find(mount_path) != _mount_table.end())
		{
			throw(FS_ERROR_CODE::FILESYSTEM_DUPLICATE, "Mount alr exists!");
		}

		// All mountPaths must begin with '/'
		if (mountPath[0] != '/')
			throw(FS_ERROR_CODE::FILESYSTEM_BAD_ARGUMENT, "Mount Path needs to start with '/'.");

		if(mountPath.back() == '/' || mountPath.back() == '\\')
			throw(FS_ERROR_CODE::FILESYSTEM_BAD_ARGUMENT, "Mount Path must only contain one '/' or '\\'");

		if (full_path.back() == '/' || full_path.back() == '\\')
			full_path.pop_back();
		
		_mount_table.emplace(mount_path, _mounts.size());
		_mounts.push_back(file_system_detail::fs_mount{});

		initMount(curr_mount_index, full_path, mount_path, watch);
	}

	FStreamWrapper FileSystem::Open(string_view mountPath, FS_PERMISSIONS perms, bool binary_stream)
	{
		auto file_index = getFile(mountPath);
		// If we cannot find the file and user only wants to read, return an empty stream
		// Else, we should create the file and return the stream for the user to write to
		if (file_index.IsValid() == false)
		{
			if (perms == FS_PERMISSIONS::READ)
				return FStreamWrapper{};
			else
			{
				auto& internal_file = createAndGetFile(mountPath);
				
				PathHandle handle{ internal_file._tree_index, true };
				return handle.Open(perms, binary_stream);
			}
		}
		else
		{
			auto& internal_file = getFile(file_index);
			PathHandle handle{internal_file._tree_index, true};

			return handle.Open(perms, binary_stream);
		}
	}

	int FileSystem::Mkdir(string_view mountPath)
	{
		UNREFERENCED_PARAMETER(mountPath);
		// Testing for now.
		// FS::create_directories(FS::path{ "C:/Users/Joseph/Desktop/GIT/idk_legacy/Koboru/Koboru/resource/editor/test" });
		return 0;
	}

	bool FileSystem::Rename(string_view mountPath, string_view new_name)
	{
		// Try getting the file
		PathHandle handle = GetFile(mountPath);
		if (handle)
			return handle.Rename(new_name);

		// Try getting the directory
		handle = GetDir(mountPath);
		if (handle)
			return handle.Rename(new_name);

		throw(FS_ERROR_CODE::FILESYSTEM_NOT_FOUND, "[FILE SYSTEM] mountPath cannot be found.");
	}

#pragma endregion Utility

#pragma region Helper Inits

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
		file_system_detail::fs_mount& mount = _mounts[index];
		mount._full_path = fullPath;
		mount._mount_path = mountPath;
		mount._watching = watch;
		mount._mount_index = s_cast<int8_t>(index);
		mount.AddDepth();

		// The root of this mount is depth 0
		file_system_detail::fs_dir d;

		d._full_path = fullPath;
		d._rel_path = currPath.relative_path().string();
		d._mount_path = mountPath;
		d._filename = currPath.filename().string();

		d._tree_index._mount_id = s_cast<int8_t>(index);
		d._tree_index._depth = 0;
		d._tree_index._index = s_cast<int16_t>(mount._path_tree[0]._dirs.size());
		if (watch)
			_directory_watcher.WatchDirectory(d);

		recurseSubDir(index, 0, d, watch);

		mount._path_tree[0]._dirs.push_back(d);
	}

	void FileSystem::initFile(file_system_detail::fs_file& f, file_system_detail::fs_dir& p_dir, std::filesystem::path& p)
	{
		f._full_path	= p.string();
		f._rel_path		= p.relative_path().string();
		f._filename		= p.filename().string();
		f._mount_path	= p_dir._mount_path + "/" + f._filename;
		f._extension	= p.extension().string();
		f._time			= FS::last_write_time(p);
		f._parent		= p_dir._tree_index;

		f.SetValid(true);
		f.SetOpenMode(FS_PERMISSIONS::NONE);

		p_dir._files_map.emplace(f._filename, f._tree_index);
	}

	void FileSystem::initDir(file_system_detail::fs_dir& d, file_system_detail::fs_dir& p_dir, std::filesystem::path& p)
	{
		d._full_path	= p.string();
		d._rel_path		= p.relative_path().string();
		d._filename		= p.filename().string();
		d._mount_path	= p_dir._mount_path + "/" + d._filename;
		d._parent		= p_dir._tree_index;

		d.SetValid(true);

		p_dir._sub_dirs.emplace(d._filename, d._tree_index);
	}

#pragma endregion Helper Inits

#pragma region Helper Getters

	file_system_detail::fs_file& FileSystem::getFile(file_system_detail::fs_key& node)
	{
		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (_mounts.empty())
			throw("Something is terribly wrong. No mounts found.");
		if (!node.IsValid())
			return _empty_file;
		return _mounts[node._mount_id]._path_tree[node._depth]._files[node._index];
	}

	file_system_detail::fs_dir& FileSystem::getDir(file_system_detail::fs_key& node)
	{
		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (_mounts.empty())
			throw("Something is terribly wrong. No mounts found.");
		if (!node.IsValid())
			return _empty_dir;
		return _mounts[node._mount_id]._path_tree[node._depth]._dirs[node._index];
	}

	const file_system_detail::fs_file& FileSystem::getFile(const file_system_detail::fs_key& node) const
	{
		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (_mounts.empty())
			throw("Something is terribly wrong. No mounts found.");
		if (!node.IsValid())
			return _empty_file;
		return _mounts[node._mount_id]._path_tree[node._depth]._files[node._index];
	}

	const file_system_detail::fs_dir& FileSystem::getDir(const file_system_detail::fs_key& node) const
	{
		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (_mounts.empty())
			throw("Something is terribly wrong. No mounts found.");
		if (!node.IsValid())
			return _empty_dir;
		return _mounts[node._mount_id]._path_tree[node._depth]._dirs[node._index];
	}

	file_system_detail::fs_key FileSystem::getFile(string_view mountPath) const
	{
		file_system_detail::fs_key empty_node;
		// First check if the path exists
		if (!Exists(mountPath))
			return empty_node;

		int mount_index = validateFileMountPath(mountPath);
		if (mount_index < 0)
			return empty_node;

		// Check if there are mounts. If this hits, something is terribly wrong...
		if (_mounts.empty())
			throw("Something is terribly wrong. No mounts found.");

		auto& mount = _mounts[mount_index];
		auto tokenized_path = tokenizePath(mountPath);

		// The parent directory of the file is one depth higher and we -1 again because we don't count the file token.
		int8_t dir_depth = s_cast<int8_t>(tokenized_path.size() - 2);

		// Get the correct directory
		if (dir_depth == 0)
		{
			// Special case. If depth is 0, there is only 1 directory inside this depth (the mount directory).
			auto& sub_dir = mount._path_tree[dir_depth]._dirs[0];

			// Find file within the sub_dir
			auto result = sub_dir._files_map.find(tokenized_path[1]);
			if (result != sub_dir._files_map.end())
				return result->second;
		}
		else
		{
			for (auto& sub_dir : mount._path_tree[dir_depth]._dirs)
			{
				// Make sure that the parent directory is correct
				if (sub_dir._mount_path + '/' + tokenized_path.back() == mountPath)
				{
					auto result = sub_dir._files_map.find(tokenized_path.back());
					if (result != sub_dir._files_map.end())
						return result->second;
				}
			}
		}

		return empty_node;
	}

	file_system_detail::fs_key FileSystem::getDir(string_view mountPath) const
	{
		file_system_detail::fs_key empty_node;
		// First check if the path exists
		if (!Exists(mountPath))
			return empty_node;

		int mount_index = validateDirMountPath(mountPath);
		if (mount_index < 0)
			return empty_node;

		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (_mounts.empty())
			throw("Something is terribly wrong. No mounts found.");

		auto& mount = _mounts[mount_index];
		auto tokenized_path = tokenizePath(mountPath);

		if (tokenized_path.back().empty())
			tokenized_path.pop_back();

		// The parent directory of the sub directory is one depth higher
		int8_t dir_depth = s_cast<int8_t>(tokenized_path.size() - 2);

		// Get the correct directory
		if (dir_depth < 0)
		{
			// Special case. If depth is 0, that means we are looking for the mount dir
			auto& sub_dir = mount._path_tree[0]._dirs[0];

			return sub_dir._tree_index;
		}
		else
		{
			for (auto& sub_dir : mount._path_tree[dir_depth]._dirs)
			{
				// Make sure that the parent directory is correct
				if (sub_dir._mount_path + '/' + tokenized_path.back() == mountPath)
				{
					// Find file within the sub_dir
					auto result = sub_dir._sub_dirs.find(tokenized_path.back());
					if (result != sub_dir._sub_dirs.end())
						return result->second;
				}
			}
		}

		return empty_node;
	}

#pragma endregion Helper Getters

#pragma region Helper Validate

	int FileSystem::validateMountPath(string_view mountPath) const
	{
		string mount_path{ mountPath.data() };

		if (mount_path[0] != '/')
			return -1;

		auto end_pos = mount_path.find_first_of('/', 1);

		string mount_key = mount_path.substr(0, end_pos);
		auto mount_index = _mount_table.find(mount_key);
		if (mount_index != _mount_table.end())
			return s_cast<int>(mount_index->second);
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
		auto mount_index = _mount_table.find(mount_key);
		if (mount_index != _mount_table.end())
			return s_cast<int>(mount_index->second);
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
		auto mount_index = _mount_table.find(mount_key);
		if (mount_index != _mount_table.end())
			return s_cast<int>(mount_index->second);
		else
			return -1;
	}

#pragma endregion Helper Validate

#pragma region Helper Auxiliary

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

	file_system_detail::fs_key FileSystem::requestFileSlot(file_system_detail::fs_mount& mount, int8_t depth)
	{
		auto check_free_index = std::find_if(mount._path_tree[depth]._files.begin(),
			mount._path_tree[depth]._files.end(),
			[](const file_system_detail::fs_file& f)
			{
				// The conditions for reuse of a file_t is that the file is not valid anymore AND the file was not changed this update
				return !f.IsValid() && f._change_status == FS_CHANGE_STATUS::NO_CHANGE;
			});

		// The conditions for reuse of a fs_file is that 
		if (check_free_index != mount._path_tree[depth]._files.end())
		{
			// check_free_index->SetValid(true);
			// check_free_index->SetOpenMode(FS_PERMISSIONS::NONE);
			return check_free_index->_tree_index;
		}
		else
		{
			mount._path_tree[depth]._files.push_back(file_system_detail::fs_file{});
			auto& file = mount._path_tree[depth]._files.back();

			file._tree_index._mount_id = mount._mount_index;
			file._tree_index._depth = depth;
			file._tree_index._index = s_cast<int16_t>(mount._path_tree[depth]._files.size() - 1);

			return file._tree_index;
		}
	}

	file_system_detail::fs_key FileSystem::requestDirSlot(file_system_detail::fs_mount& mount, int8_t depth)
	{
		auto check_free_index = std::find_if(mount._path_tree[depth]._dirs.begin(),
			mount._path_tree[depth]._dirs.end(),
			[](const file_system_detail::fs_dir& d)
			{
				// The conditions for reuse of a file_t is that the file is not valid anymore AND the file was not changed this update
				return !d.IsValid() && d._change_status == FS_CHANGE_STATUS::NO_CHANGE;
			});

		if (check_free_index != mount._path_tree[depth]._dirs.end())
		{
			// check_free_index->SetValid(true);
			return check_free_index->_tree_index;
		}
		else
		{
			mount._path_tree[depth]._dirs.push_back(file_system_detail::fs_dir{});
			auto& dir = mount._path_tree[depth]._dirs.back();

			dir._tree_index._mount_id = mount._mount_index;
			dir._tree_index._depth = depth;
			dir._tree_index._index = s_cast<int16_t>(mount._path_tree[depth]._dirs.size() - 1);

			return dir._tree_index;
		}
	}

	file_system_detail::fs_file& FileSystem::createAndGetFile(string_view mountPath)
	{
		// Means we need to create the file...
		string mount_path{ mountPath.data() };
		auto end_pos = mount_path.find_last_of('/');
		auto dir_index = getDir(mount_path.substr(0, end_pos));

		if (dir_index.IsValid() == false)
			return _empty_file;

		auto& dir = getDir(dir_index);
		string full_path = dir._full_path + "/" + mount_path.substr(end_pos);

		std::ofstream{ full_path };

		FS::path p{ full_path };

		// Request a slot from mounts
		// Check if there are even mounts. If this hits, something is terribly wrong...
		if (_mounts.empty())
			throw("Something is terribly wrong. No mounts found.");

		// initializing the fs_file
		auto slot = requestFileSlot(_mounts[dir._tree_index._mount_id], dir._tree_index._depth + 1);
		auto& f = getFile(slot);
		initFile(f, dir, p);

		return f;
	}

#pragma endregion Helper Auxiliary

#pragma region Helper Recurse

	void FileSystem::recurseSubDir(size_t index, int8_t currDepth, file_system_detail::fs_dir& mountSubDir, bool watch)
	{
		file_system_detail::fs_mount& mount = _mounts[index];

		// Increase the depth if this expands the tree
		++currDepth;
		if (currDepth >= mount._path_tree.size() - 1)
			mount.AddDepth();

		FS::path currPath{ mountSubDir._full_path };
		FS::directory_iterator dir{ currPath };

		// initializing all the paths from this path
		for (auto& elem : dir)
		{
			FS::path tmp{ elem.path() };
			if (FS::is_regular_file(tmp))
			{
				file_system_detail::fs_file f;

				f._tree_index._mount_id = s_cast<int8_t>(index);
				f._tree_index._depth = currDepth;
				f._tree_index._index = s_cast<int16_t>(mount._path_tree[currDepth]._files.size());

				// f._file_detail is default initialized to have ref_count = 0.

				initFile(f, mountSubDir, tmp);
				mount._path_tree[currDepth]._files.push_back(f);
			}
			else
			{
				file_system_detail::fs_dir d;

				d._tree_index._mount_id = s_cast<int8_t>(index);
				d._tree_index._depth = currDepth;
				d._tree_index._index = s_cast<int16_t>(mount._path_tree[currDepth]._dirs.size());

				initDir(d, mountSubDir, tmp);

				mountSubDir._sub_dirs.emplace(d._filename, d._tree_index);
				if (watch)
					_directory_watcher.WatchDirectory(d);

				recurseSubDir(index, currDepth, d, watch);
				mount._path_tree[currDepth]._dirs.push_back(d);
			}
		}
	}

	// void FileSystem::recurseSubDirExtensions(vector<PathHandle>& vec_handles, const file_system_detail::fs_dir& subDir, string_view extension) const
	// {
	// 	for (auto& file_index : subDir._files_map)
	// 	{
	// 		auto& internal_file = getFile(file_index.second);
	// 		if (internal_file._extension == extension)
	// 		{
	// 			PathHandle h{ internal_file._tree_index, true };
	// 			vec_handles.emplace_back(h);
	// 		}
	// 	}
	// 
	// 	for (auto& dir_index : subDir._sub_dirs)
	// 	{
	// 		auto& internal_dir = getDir(dir_index.second);
	// 		recurseSubDirExtensions(vec_handles, internal_dir, extension);
	// 	}
	// }

#pragma endregion Helper Recurse

#pragma region Helper Dump

	void FileSystem::DumpMounts() const
	{
		std::cout << std::boolalpha;
		for (auto& mount_index : _mount_table)
		{
			std::cout << "Total mounts: " << _mounts.size() << std::endl;
			std::cout << "Dumping mounts..." << std::endl;
			
			auto& mount = _mounts[mount_index.second];
			// std::cout << "Mount: " << mount_index.first << "\n";
			dumpMount(mount);
		}
		std::cout << std::noboolalpha;
	}

	void FileSystem::dumpMount(const file_system_detail::fs_mount& mount) const
	{
		std::cout << "Mount: " << mount._mount_path << "\n";
		dumpDir(mount._path_tree[0]._dirs[0], "\t");
		
	}

	void FileSystem::dumpDir(const file_system_detail::fs_dir& sub_dir, string prefix) const
	{
		prefix += "|";
		std::cout	<< prefix << "Directory: " << sub_dir._filename << "\n"
					<< prefix << "Rel Path: "			<< sub_dir._rel_path << "\n"
					<< prefix << "Mount Path: "			<< sub_dir._mount_path << "\n"
					<< prefix << "Parent Full Path: "	<< getDir(sub_dir._parent)._full_path << "\n"
					<< prefix << "Parent Mount Path: "	<< getDir(sub_dir._parent)._mount_path << "\n"
					<< prefix << "Num Files: "			<< sub_dir._files_map.size() << "\n"
					<< prefix << "Num Sub-Dirs: "		<< sub_dir._sub_dirs.size() << "\n";

		std::cout << prefix << "Files: " << "\n";
		string file_prefix = prefix + "  ";
		for (auto& file_index : sub_dir._files_map)
		{
			auto& file = getFile(file_index.second);
			std::cout << file_prefix  << "*" << "Filename: " << file._filename << "\n";
			std::cout << file_prefix  << "*" << "Extension: " << file._extension << "\n";
			std::cout << file_prefix  << "*" << "Rel Path: " << file._rel_path << "\n";
			std::cout << file_prefix  << "*" << "Mount Path: " << file._mount_path << "\n";
			std::cout << file_prefix  << "*" << "Parent Dir Check: " << (file._parent == sub_dir._tree_index) << "\n";
		}

		if (sub_dir._sub_dirs.size() == 0)
			return;
		std::cout << prefix << "Sub-Dirs: " << "\n";
		for (auto& dir_index : sub_dir._sub_dirs)
		{
			auto& sub_sub_dir = getDir(dir_index.second);
			dumpDir(sub_sub_dir, prefix + "  ");
		}
	}
	
#pragma endregion Helper Dump
	
}