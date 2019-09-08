#include "stdafx.h"

#include "FileHandle.h"
#include "core/Core.h"
#include "file/FileSystem.h"

#include <iostream>
#include <filesystem>

namespace FS = std::filesystem;

namespace idk
{
	FStreamWrapper::FStreamWrapper(FStreamWrapper&& rhs)
		:std::fstream{ std::move(rhs) }
	{
		std::swap(_handle_index, rhs._handle_index);
	}

	FStreamWrapper& FStreamWrapper::operator=(FStreamWrapper&& rhs)
	{
		std::fstream::operator=(std::move(rhs));
		_handle_index = std::move(rhs._handle_index);
		return *this;
	}

	FStreamWrapper::~FStreamWrapper()
	{
		if (_handle_index < 0)
			return;

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file_handle = vfs._file_handles[_handle_index];
		auto& file = vfs.getFile(file_handle._internal_id);

		// Only reset if this stream object was actually open.
		// If we reset when its not open, we may reset for a different wrapper
		if (is_open())
		{
			flush();
			file_handle.Reset();
			file._time = FS::last_write_time(FS::path{ file._full_path });
		}
	}

	// FileHandle FStreamWrapper::GetHandle() const
	// {
	// 	if (_handle_index < 0)
	// 		return FileHandle{};
	// 
	// 	return FileHandle{ Core::GetSystem<FileSystem>()._file_handles[_handle_index]._internal_id };
	// }

	void FStreamWrapper::close()
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file_handle = vfs._file_handles[_handle_index];
		auto& file = vfs.getFile(file_handle._internal_id);

		file_handle.Reset();
		file._time = FS::last_write_time(FS::path{ file._full_path });

		std::fstream::close();
	}

	FileHandle::FileHandle(const file_system_detail::fs_key& key, bool is_file)
		:_key{ key }, _is_regular_file{ is_file }
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		if (is_file && vfs.validateKey(_key))
		{
			_ref_count = vfs._file_handles[vfs.getFile(_key)._handle_index]._ref_count;
		}
	}

	bool FileHandle::validate() const
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		if (_key.IsValid() == false)
			return false;

		if (_is_regular_file)
		{
			auto& file = vfs.getFile(_key);
			auto& internal_handle = vfs._file_handles[file._handle_index];
			return _ref_count == internal_handle._ref_count && file._valid;
		}
		else
			return true;
	}

	bool FileHandle::validateFull() const
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		if (_key.IsValid() == false)
			return false;

		if (_is_regular_file)
		{
			auto& file = vfs.getFile(_key);
			auto& internal_handle = vfs._file_handles[file._handle_index];
			return _ref_count == internal_handle._ref_count && file._valid;
		}
		else
			return vfs.getDir(_key)._valid;
	}


	bool FileHandle::Rename(string_view new_file_name)
	{
		// Check Handle
		if (validate() == false)
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
				std::cout << "[FILEHANDLE] Rename: " << e.what() << std::endl;
				// return false;
			}
			auto res = parent_dir._files_map.find(internal_file._filename);
			assert(res != parent_dir._files_map.end());
			parent_dir._files_map.erase(res);

			vfs.initFile(internal_file, parent_dir, new_p);
			auto res2 = parent_dir._files_map.find(internal_file._filename);

			auto& test = vfs.getFile(res2->second);
			return true;
		}

		return false;
	}

	FileHandle::operator bool() const
	{
		return validateFull();
	}

	string_view FileHandle::GetMountPath() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		
		return _is_regular_file ? vfs.getFile(_key)._mount_path : vfs.getDir(_key)._mount_path;
	}

	string_view FileHandle::GetExtension() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file = vfs.getFile(_key);

		return file._extension;
	}

	FileHandle::FileHandle(string_view mountPath)
		:FileHandle{Core::GetSystem<FileSystem>().GetFile(mountPath)}
	{
	}

	FileHandle::FileHandle(const char* mountPath)
		: FileHandle{ Core::GetSystem<FileSystem>().GetFile(mountPath) }
	{
	}

	bool FileHandle::operator==(const FileHandle& rhs) const
	{
		return _key == rhs._key && _ref_count == rhs._ref_count && _is_regular_file == rhs._is_regular_file;
	}

	string_view FileHandle::GetFileName() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file = vfs.getFile(_key);
		
		return file._filename;
	}

	string_view FileHandle::GetFullPath() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file = vfs.getFile(_key);

		return file._full_path;
	}

	string_view FileHandle::GetRelPath() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file = vfs.getFile(_key);

		return file._rel_path;
	}

	string_view FileHandle::GetParentMountPath() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file = vfs.getFile(_key);
		auto& parent = vfs.getFile(file._parent);

		return parent._mount_path;
	}

	bool FileHandle::CanOpen() const
	{
		if (!_is_regular_file)
			return false;

		auto& vfs = Core::GetSystem<FileSystem>();
		// Checking if handle is valid
		if (validateFull() == false)
			return false;

		// Checking if internal handle is valid
		auto& file = vfs.getFile(_key);
		auto& file_handle = vfs._file_handles[file._handle_index];
		return file_handle.IsOpenAndValid();
	}

	bool FileHandle::SameKeyAs(const FileHandle& other) const
	{
		return _key == other._key;
	}

	FS_CHANGE_STATUS FileHandle::GetStatus() const
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		// Checking if handle is valid
		if (validate() == false)
			return FS_CHANGE_STATUS::INVALID;

		auto& file = vfs.getFile(_key);

		return file._change_status;
	}

	FStreamWrapper FileHandle::Open(FS_PERMISSIONS perms, bool binary_stream)
	{
		FStreamWrapper fs;

		if (!_is_regular_file)
			return fs;

		auto& vfs = Core::GetSystem<FileSystem>();
		
		// Checking if handle is valid
		if (validateFull() == false)
			return fs;
		
		// Checking if internal handle is valid
		auto& file = vfs.getFile(_key);
		auto& file_handle = vfs._file_handles[file._handle_index];
		if (file_handle.IsOpenAndValid() == false)
			return fs;

		auto& internal_file = vfs.getFile(file_handle._internal_id);
		
		switch (perms)
		{
		case FS_PERMISSIONS::READ:
			fs.open(internal_file._full_path, 
				binary_stream ? std::ios::in | std::ios::binary : std::ios::in);
			fs._handle_index = file._handle_index;

			file_handle.SetOpenFormat(file_system_detail::OPEN_FORMAT::READ_ONLY);
			break;
		case FS_PERMISSIONS::WRITE:
			fs.open(internal_file._full_path, 
				binary_stream ? std::ios::out | std::ios::binary : std::ios::out);
			fs._handle_index = file._handle_index;

			file_handle.SetOpenFormat(file_system_detail::OPEN_FORMAT::WRITE_ONLY);
			break;
		case FS_PERMISSIONS::APPEND:
			fs.open(internal_file._full_path, 
				binary_stream ? std::ios::app | std::ios::binary : std::ios::app);
			fs._handle_index = file._handle_index;

			file_handle.SetOpenFormat(file_system_detail::OPEN_FORMAT::WRITE_ONLY);
			break;
		default:
			return fs;
		}
		return fs;
	}

	string_view FileHandle::GetParentFullPath() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file = vfs.getFile(_key);
		auto& parent = vfs.getFile(file._parent);
		
		return parent._full_path;
	}

	string_view FileHandle::GetParentRelPath() const
	{
		// Check Handle
		if (validate() == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file = vfs.getFile(_key);
		auto& parent = vfs.getFile(file._parent);

		return parent._rel_path;
	}

	

}