#include "stdafx.h"

#include "FileHandle.h"
#include "core/Core.h"
#include "file/FileSystem.h"

#include <filesystem>

namespace FS = std::filesystem;

namespace idk
{
	FileHandle::FileHandle(FileHandle&& rhs)
	{
		std::swap(stream, rhs.stream);
		std::swap(fp, rhs.fp);
		std::swap(ref_count, rhs.ref_count);
		std::swap(handle_index, rhs.handle_index);
	}

	FileHandle::~FileHandle()
	{
		if (handle_index < 0)
			return;
		stream.flush();

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file_handle = vfs.file_handles[handle_index];
		auto& file = vfs.getFile(file_handle.internal_id);

		file_handle.Reset();
		file.time = FS::last_write_time(FS::path{ file.full_path });
	}

	FileHandle& FileHandle::operator=(FileHandle&& rhs)
	{
		std::swap(stream, rhs.stream);
		std::swap(fp, rhs.fp);
		std::swap(ref_count, rhs.ref_count);
		std::swap(handle_index, rhs.handle_index);

		return *this;
	}

	FileHandle::operator bool()
	{
		if (fp == nullptr)
			return stream.is_open();
		return false;
	}

	size_t FileHandle::Read(void* buffer, size_t len)
	{
		// Check Handle
		if (Core::GetSystem<FileSystem>().validateHandle(*this) == false)
			return 0;

		if (buffer == nullptr || len == 0)
			return 0;

		size_t before = stream.tellp(); //current pos
		stream.read(static_cast<char*>(buffer), len);
		return static_cast<size_t>(stream.tellp()) - before;
	}

	size_t FileHandle::GetLine(void* buffer, size_t len, const char delim)
	{
		// Check Handle
		if (Core::GetSystem<FileSystem>().validateHandle(*this) == false)
			return 0;

		size_t before = stream.tellp(); //current pos
		stream.getline(static_cast<char*>(buffer), len, delim);
		return static_cast<size_t>(stream.tellp()) - before;
	}

	size_t FileHandle::Write(const void* data, size_t len)
	{
		// Check Handle
		if (Core::GetSystem<FileSystem>().validateHandle(*this) == false)
			return 0;

		size_t before = stream.tellp(); //current pos
		stream.write(static_cast<const char*>(data), len);
		return static_cast<size_t>(stream.tellp()) - before;
	}

	string_view FileHandle::GetMountPath() const
	{
		// Check Handle
		if (Core::GetSystem<FileSystem>().validateHandle(*this) == false)
			return string_view{};

		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file_handle = vfs.file_handles[handle_index];
		auto& file = vfs.getFile(file_handle.internal_id);

		return file.mount_path;
	}

	string_view FileHandle::GetFullPath() const
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file_handle = vfs.file_handles[handle_index];
		auto& file = vfs.getFile(file_handle.internal_id);

		return file.full_path;
	}

	string_view FileHandle::GetParentMountPath() const
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file_handle = vfs.file_handles[handle_index];
		auto& file = vfs.getFile(file_handle.internal_id);
		auto& parent = vfs.getFile(file.parent);

		return parent.mount_path;
	}

	string_view FileHandle::GetParentFullPath() const
	{
		auto& vfs = Core::GetSystem<FileSystem>();
		auto& file_handle = vfs.file_handles[handle_index];
		auto& file = vfs.getFile(file_handle.internal_id);
		auto& parent = vfs.getFile(file.parent);

		return parent.full_path;
	}

}