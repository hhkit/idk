#pragma once

#include <fstream>

#include "FileSystemInternal.h"
#include "FileSystemIncludes.h"

namespace idk
{
	// struct FileHandle;
	class FStreamWrapper : public std::fstream
	{
	public:
		FStreamWrapper(FStreamWrapper&& rhs);
		FStreamWrapper& operator=(FStreamWrapper&& rhs);
		~FStreamWrapper();

		// FileHandle GetHandle() const;

		// Override some files.
		void close();

		friend class FileSystem;
		friend struct FileHandle;
	private:
		FStreamWrapper() = default;
		
		file_system_detail::fs_key _file_key{};
	};

	class FILEWrapper
	{
	public:
		FILE* data() { return _fp; }

		friend struct FileHandle;
	private:
		FILE* _fp = nullptr;
	};

	struct FileHandle
	{
		FileHandle() = default;
		FileHandle(string_view mountPath);
		FileHandle(const char* mountPath);

		bool operator == (const FileHandle& rhs) const;

		string_view			GetFileName()	const;
		
		string_view			GetFullPath()	const;
		string_view			GetRelPath()	const;
		string_view			GetMountPath()	const;

		string_view			GetExtension()	const;
		
		string_view			GetParentFullPath()  const;
		string_view			GetParentRelPath()	 const;
		string_view			GetParentMountPath() const;

		FS_CHANGE_STATUS	GetStatus() const;
		bool				IsFile()	const { return _is_regular_file; }
		bool				IsDir()		const { return !_is_regular_file; }
		bool				CanOpen()	const;
		bool				SameKeyAs(const FileHandle& other) const;
		
		FStreamWrapper		Open(FS_PERMISSIONS perms, bool binary_stream = false);
		FILEWrapper			OpenC(FS_PERMISSIONS perm, bool binary_stream = false);

		bool				Rename(string_view new_file_name);

		explicit	operator bool() const;

		

		friend class FileSystem;
		friend class FStreamWrapper;
	private:
		FileHandle(const file_system_detail::fs_key& key, bool is_file = true);

		bool validate() const;
		bool validateFull() const;

		int64_t _ref_count		= -1;
		file_system_detail::fs_key _key{};
		bool _is_regular_file = true;
	};
}