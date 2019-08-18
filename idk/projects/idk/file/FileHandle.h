#pragma once


#include <iostream>
#include <fstream>

namespace idk
{
	// Opaque type. Do not touch internals
	struct FileHandle
	{
		FileHandle() = default;
		FileHandle(const FileHandle& rhs) = delete;
		FileHandle(FileHandle&& rhs);
		~FileHandle();

		FileHandle& operator=(const FileHandle& rhs) = delete;
		FileHandle& operator=(FileHandle&& rhs);
		explicit	operator bool();

		string_view		GetMountPath() const;
		string_view		GetFullPath() const;
		string_view		GetParentMountPath() const;
		string_view		GetParentFullPath() const;

		std::fstream&	GetStream() { return stream; }
		FILE*			GetFilePtr() { return fp; }

		// len is the number of bytes to read
		size_t Read(void* buffer, size_t len);
		// len here is the size of the buffer
		size_t GetLine(void* buffer, size_t len, const char delim = '\n');
		// len is number of bytes to write
		size_t Write(const void* data, size_t len);

		friend class FileSystem;
	private:
		FILE* fp = nullptr;
		std::fstream stream;

		int64_t handle_index = -1;
		int64_t ref_count = -1;

		// FileSystem_ErrorCode error;
	};
}