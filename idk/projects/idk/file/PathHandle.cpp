#include "stdafx.h"

#include "PathHandle.h"
#include "core/Core.h"
#include "file/FileSystem.h"

#include <iostream>
#include <filesystem>

namespace FS = std::filesystem;

namespace idk
{

#pragma region fstreamWrapper

	FStreamWrapper::FStreamWrapper(FStreamWrapper&& rhs)
		:std::fstream{ std::move(rhs) }
	{
		std::swap(_file_key, rhs._file_key);
	}

	FStreamWrapper& FStreamWrapper::operator=(FStreamWrapper&& rhs)
	{
		std::fstream::operator=(std::move(rhs));
		_file_key = std::move(rhs._file_key);
		return *this;
	}

	FStreamWrapper::~FStreamWrapper()
	{
		if (!_file_key.IsValid())
			return;

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file = vfs.getFile(_file_key);

		// Only reset if this stream object was actually open.
		// If we reset when its not open, we may reset for a different wrapper
		if (is_open())
		{
			flush();
			file.SetOpenMode(FS_PERMISSIONS::NONE);

			// !!! CLOSE THEN UPDATE TIME
			std::fstream::close();
			file._time = FS::last_write_time(FS::path{ file._full_path });
		}
	}

	void FStreamWrapper::close()
	{
		if (!_file_key.IsValid())
			return;

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file = vfs.getFile(_file_key);

		// Only reset if this stream object was actually open.
		// If we reset when its not open, we may reset for a different wrapper
		if (is_open())
		{
			flush();
			file.SetOpenMode(FS_PERMISSIONS::NONE);

			// !!! CLOSE THEN UPDATE TIME
			std::fstream::close();
			file._time = FS::last_write_time(FS::path{ file._full_path });
		}
	}
#pragma endregion fstreamWrapper

#pragma region Contructors/Operators

	PathHandle::PathHandle(const file_system_detail::fs_key& key, bool is_file)
		:_key{ key }, _is_regular_file{ is_file }
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		if (is_file && _key.IsValid())
		{
			_ref_count = vfs.getFile(_key).RefCount();
		}
		else if(_key.IsValid())
		{
			_ref_count = vfs.getDir(_key).RefCount();
		}
	}

	PathHandle::PathHandle(string_view mountPath)
		:PathHandle{ Core::GetSystem<FileSystem>().GetPath(mountPath) }
	{
	}
    PathHandle::PathHandle(const char* mountPath)
        : PathHandle{ Core::GetSystem<FileSystem>().GetPath(mountPath) }
    {
    }
    PathHandle::PathHandle(const string& mountPath)
        : PathHandle{ Core::GetSystem<FileSystem>().GetPath(mountPath) }
    {
    }

	PathHandle::operator bool() const
	{
		return validateFull();
	}

	bool PathHandle::operator==(const PathHandle& rhs) const
	{
		return _key == rhs._key && _ref_count == rhs._ref_count && _is_regular_file == rhs._is_regular_file;
	}

    bool PathHandle::operator!=(const PathHandle& rhs) const
    {
        return !operator==(rhs);
    }

#pragma endregion Contructors/Operators

#pragma region Utility

	FStreamWrapper PathHandle::Open(FS_PERMISSIONS perms, bool binary_stream)
	{
		FStreamWrapper fs;

		if (!CanOpen())
			return fs;

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& internal_file = vfs.getFile(_key);

		internal_file.SetOpenMode(perms);
		fs._file_key = internal_file._tree_index;

		switch (perms)
		{
		case FS_PERMISSIONS::READ:
			fs.open(internal_file._full_path,
				binary_stream ? std::ios::in | std::ios::binary : std::ios::in);
			break;

		case FS_PERMISSIONS::WRITE:
			fs.open(internal_file._full_path,
				binary_stream ? std::ios::out | std::ios::binary : std::ios::out);
			break;

		case FS_PERMISSIONS::APPEND:
			fs.open(internal_file._full_path,
				binary_stream ? std::ios::app | std::ios::binary : std::ios::app);
			break;

		default:
			return FStreamWrapper{};
		}
		return fs;
	}


	bool PathHandle::Rename(string_view new_file_name)
	{
		// Check Handle
		if (validateFull() == false)
			return false;

		auto& vfs = Core::GetSystem<FileSystem>();
		if (_is_regular_file)
		{
			auto& internal_file = vfs.getFile(_key);
			auto& parent_dir = vfs.getDir(internal_file._parent);

			FS::path old_p{ internal_file._full_path };
			FS::path new_p{ parent_dir._full_path + "/" + new_file_name.data() };
			try {
				FS::rename(old_p, new_p);
			}catch(const FS::filesystem_error& e){
				std::cout << "[PathHandle ERROR] Rename: " << e.what() << std::endl;
				return false;
			}
			auto res = parent_dir._files_map.find(internal_file._filename);
			assert(res != parent_dir._files_map.end());
			parent_dir._files_map.erase(res);

			vfs.initFile(internal_file, parent_dir, new_p);
			
			return true;
		}
		else
		{
			// Rename the current dir first
			auto& internal_dir = vfs.getDir(_key);
			if (!internal_dir.IsValid())
				return false;

			auto& parent_dir = vfs.getDir(internal_dir._parent);

			FS::path old_p{ internal_dir._full_path };
			FS::path new_p{ parent_dir._full_path + "/" + new_file_name.data() };
			try {
				FS::rename(old_p, new_p);
			}
			catch (const FS::filesystem_error& e) {
				std::cout << "[PathHandle ERROR] Rename: " << e.what() << std::endl;
				return false;
			}
			auto res = parent_dir._sub_dirs.find(internal_dir._filename);
			assert(res != parent_dir._sub_dirs.end());
			parent_dir._sub_dirs.erase(res);

			vfs.initDir(internal_dir, parent_dir, new_p);

			renameDirUpdate();
			
			return true;
		}
	}

#pragma endregion Utility
	
#pragma region Dir Specific Getters

	vector<PathHandle> PathHandle::GetEntries(FS_FILTERS filters, string_view ext) const
	{
		vector<PathHandle> out;
		if (!validateFull() || _is_regular_file)
			return out;

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& dir = vfs.getDir(_key);

		if ((filters & FS_FILTERS::DIR) == FS_FILTERS::DIR)
		{
			for (auto& dir_index : dir._sub_dirs)
			{
				auto& internal_dir = vfs.getDir(dir_index.second);
				if (internal_dir.IsValid())
				{
					PathHandle h{ internal_dir._tree_index, false };
					out.emplace_back(h);

					if ((filters & FS_FILTERS::RECURSE_DIRS) == FS_FILTERS::RECURSE_DIRS)
						getPathsRecurse(internal_dir, filters, ext, out);
				}
			}
		}

		if ((filters & FS_FILTERS::FILE) == FS_FILTERS::FILE)
		{
			for (auto& file_index : dir._files_map)
			{
				auto& internal_file = vfs.getFile(file_index.second);
				if (internal_file.IsValid())
				{
					// If extensions are enabled and the extensions don't match, we ignore this file
					if ((filters & FS_FILTERS::EXT) == FS_FILTERS::EXT && internal_file._extension != ext)
						continue;

					PathHandle h{ internal_file._tree_index, true };
					out.emplace_back(h);
				}
			}
		}

		return out;
	}

	vector<PathHandle> PathHandle::GetFilesWithExtension(string_view ext, FS_FILTERS filters) const
	{
		filters |= FS_FILTERS::EXT | FS_FILTERS::FILE;

		return GetEntries(filters, ext);
	}

#pragma endregion Dir Specific Getters
	
#pragma region General Path Getters
	string_view PathHandle::GetFileName() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();

		return _is_regular_file ? vfs.getFile(_key)._filename : vfs.getDir(_key)._filename;
	}

	string_view PathHandle::GetExtension() const
	{
		if (!_is_regular_file)
			return string_view{};;
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file = vfs.getFile(_key);

		return file._extension;
	}
	string_view PathHandle::GetFullPath() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();

		return _is_regular_file ? vfs.getFile(_key)._full_path : vfs.getDir(_key)._full_path;
	}

	string_view PathHandle::GetRelPath() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		return _is_regular_file ? vfs.getFile(_key)._rel_path : vfs.getDir(_key)._rel_path;
	}


	string_view PathHandle::GetMountPath() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();

		return _is_regular_file ? vfs.getFile(_key)._mount_path : vfs.getDir(_key)._mount_path;
	}

	string_view PathHandle::GetParentFullPath() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		file_system_detail::fs_key parent = _is_regular_file ? vfs.getFile(_key)._parent : vfs.getDir(_key)._parent;

		if (parent.IsValid())
			return vfs.getDir(parent)._full_path;

		return string_view{};
	}

	string_view PathHandle::GetParentRelPath() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		file_system_detail::fs_key parent = _is_regular_file ? vfs.getFile(_key)._parent : vfs.getDir(_key)._parent;

		if (parent.IsValid())
			return vfs.getDir(parent)._rel_path;

		return string_view{};
	}


	string_view PathHandle::GetParentMountPath() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		file_system_detail::fs_key parent = _is_regular_file ? vfs.getFile(_key)._parent : vfs.getDir(_key)._parent;

		if (parent.IsValid())
			return vfs.getDir(parent)._mount_path;

		return string_view{};
	}
#pragma endregion General Path Getters

#pragma region General Bool Checks

	bool PathHandle::CanOpen() const
	{
		// Checking if handle is valid & if it is pointing to a file
		if (!_is_regular_file || !validateFull())
			return false;

		auto& vfs = Core::GetSystem<FileSystem>();
		// Checking if internal handle is valid
		auto& file = vfs.getFile(_key);
		return file.IsValid() && !file.IsOpen();
	}

	bool PathHandle::SameKeyAs(const PathHandle& other) const
	{
		return  (_is_regular_file == other._is_regular_file) &&
			(_key == other._key);
	}

	FS_CHANGE_STATUS PathHandle::GetStatus() const
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		// Checking if handle is valid
		if (validate() == false)
			return FS_CHANGE_STATUS::INVALID;

		return _is_regular_file ? vfs.getFile(_key)._change_status : vfs.getDir(_key)._change_status;
	}

#pragma endregion General Bool Checks

#pragma region Helper Bool Checks

	bool PathHandle::validate() const
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		if (_key.IsValid() == false)
			return false;

		// Only checks if the ref count is correct. Ref count is inc when file deleted
		if (_is_regular_file)
		{
			auto& file = vfs.getFile(_key);
			auto ref_count_actual = file.RefCount();
			return (_ref_count == ref_count_actual);
		}
		else
		{
			auto& dir = vfs.getDir(_key);
			auto ref_count_actual = dir.RefCount();
			return (_ref_count == ref_count_actual);
		}
	}

	bool PathHandle::validateFull() const
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		if (_key.IsValid() == false)
			return false;

		// Checks everything. Essentially, does the file exists?
		if (_is_regular_file)
		{
			auto& file = vfs.getFile(_key);
			auto ref_count_actual = file.RefCount();
			return (_ref_count == ref_count_actual) && file.IsValid();
		}
		else
		{
			auto& dir = vfs.getDir(_key);
			auto ref_count_actual = dir.RefCount();
			return (_ref_count == ref_count_actual) && dir.IsValid();
		}
	}
#pragma endregion Helper Bool Checks

#pragma region Helper Recursion

	void PathHandle::renameDirUpdate()
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		// Change the data in of all the dirs and file
		FS_FILTERS filters = FS_FILTERS::ALL;
		auto paths = GetEntries(filters);
		for (auto& p : paths)
		{
			if (p._is_regular_file)
			{
				auto& internal_file = vfs.getFile(p._key);
				auto& parent_dir = vfs.getDir(internal_file._parent);

				string append = '/' + internal_file._filename;

				internal_file._full_path = parent_dir._full_path + append;
				internal_file._rel_path = parent_dir._rel_path + append;
				internal_file._mount_path = parent_dir._mount_path + append;
			}
			else
			{
				auto& internal_dir = vfs.getDir(p._key);
				auto& parent_dir = vfs.getDir(internal_dir._parent);

				string append = '/' + internal_dir._filename;

				internal_dir._full_path = parent_dir._full_path + append;
				internal_dir._rel_path = parent_dir._rel_path + append;
				internal_dir._mount_path = parent_dir._mount_path + append;
			}
		}
	}
	void PathHandle::getPathsRecurse(const file_system_detail::fs_dir& dir, FS_FILTERS filters, string_view ext, vector<PathHandle>& out) const
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		if ((filters & FS_FILTERS::DIR) == FS_FILTERS::DIR)
		{
			for (auto& dir_index : dir._sub_dirs)
			{
				auto& internal_dir = vfs.getDir(dir_index.second);
				if (internal_dir.IsValid())
				{
					PathHandle h{ internal_dir._tree_index, false };
					out.emplace_back(h);

					if ((filters & FS_FILTERS::RECURSE_DIRS) == FS_FILTERS::RECURSE_DIRS)
						getPathsRecurse(internal_dir, filters, ext, out);
				}
			}
		}

		if ((filters & FS_FILTERS::FILE) == FS_FILTERS::FILE)
		{
			for (auto& file_index : dir._files_map)
			{
				auto& internal_file = vfs.getFile(file_index.second);
				if (internal_file.IsValid())
				{
					// If extensions are enabled and the extensions don't match, we ignore this file
					if ((filters & FS_FILTERS::EXT) == FS_FILTERS::EXT && internal_file._extension != ext)
						continue;

					PathHandle h{ internal_file._tree_index, true };
					out.emplace_back(h);
				}
			}
		}
	}

	// void PathHandle::getFilesExtRecurse(const file_system_detail::fs_dir& dir, string_view ext, vector<PathHandle>& out) const
	// {
	// 	auto& vfs = Core::GetSystem<FileSystem>();
	// 	auto& dir = vfs.getDir(_key);
	// 
	// 	for (auto& file_index : dir._files_map)
	// 	{
	// 		auto& internal_file = vfs.getFile(file_index.second);
	// 		if (internal_file._extension == ext)
	// 		{
	// 			PathHandle h{ internal_file._tree_index, true };
	// 			out.emplace_back(h);
	// 		}
	// 	}
	// 
	// 	for (auto& dir_index : dir._sub_dirs)
	// 	{
	// 		auto& internal_dir = vfs.getDir(dir_index.second);
	// 		getFilesExtRecurse(internal_dir, ext, out);
	// 	}
	// }
#pragma endregion Helper Recursion
}