#include "stdafx.h"

#include "FileHandle.h"
#include "core/Core.h"
#include "file/FileSystem.h"

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


	FileHandle::operator bool()
	{
		return Core::GetSystem<FileSystem>().validateHandle(*this);
	}

	string_view FileHandle::GetMountPath() const
	{
		// Check Handle
		if (Core::GetSystem<FileSystem>().validateHandle(*this) == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file_handle = vfs._file_handles[_handle_index];
		auto& file = vfs.getFile(file_handle._internal_id);

		return file._mount_path;
	}

	string_view FileHandle::GetFullPath() const
	{
		// Check Handle
		if (Core::GetSystem<FileSystem>().validateHandle(*this) == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file_handle = vfs._file_handles[_handle_index];
		auto& file = vfs.getFile(file_handle._internal_id);

		return file._full_path;
	}

	string_view FileHandle::GetParentMountPath() const
	{
		// Check Handle
		if (Core::GetSystem<FileSystem>().validateHandle(*this) == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file_handle = vfs._file_handles[_handle_index];
		auto& file = vfs.getFile(file_handle._internal_id);
		auto& parent = vfs.getFile(file._parent);

		return parent._mount_path;
	}

	bool FileHandle::CanOpen() const
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		// Checking if handle is valid
		if (vfs.validateHandle(*this) == false)
			return false;

		// Checking if internal handle is valid
		auto& file_handle = vfs._file_handles[_handle_index];
		return file_handle.IsOpenAndValid();
	}

	FStreamWrapper FileHandle::Open(FS_PERMISSIONS perms, bool binary_stream)
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		FStreamWrapper fs;

		// Checking if handle is valid
		if (vfs.validateHandle(*this) == false)
			return fs;
		
		// Checking if internal handle is valid
		auto& file_handle = vfs._file_handles[_handle_index];
		if (file_handle.IsOpenAndValid() == false)
			return fs;

		auto& internal_file = vfs.getFile(file_handle._internal_id);
		
		switch (perms)
		{
		case FS_PERMISSIONS::READ:
			fs.open(internal_file._full_path, 
				binary_stream ? std::ios::in | std::ios::binary : std::ios::in);
			fs._handle_index = _handle_index;

			file_handle.SetOpenFormat(file_system_detail::OPEN_FORMAT::READ_ONLY);
			break;
		case FS_PERMISSIONS::WRITE:
			fs.open(internal_file._full_path, 
				binary_stream ? std::ios::out | std::ios::binary : std::ios::out);
			fs._handle_index = _handle_index;

			file_handle.SetOpenFormat(file_system_detail::OPEN_FORMAT::WRITE_ONLY);
			break;
		case FS_PERMISSIONS::APPEND:
			fs.open(internal_file._full_path, 
				binary_stream ? std::ios::app | std::ios::binary : std::ios::app);
			fs._handle_index = _handle_index;

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
		if (Core::GetSystem<FileSystem>().validateHandle(*this) == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file_handle = vfs._file_handles[_handle_index];
		auto& file = vfs.getFile(file_handle._internal_id);
		auto& parent = vfs.getFile(file._parent);

		return parent._full_path;
	}

	

}