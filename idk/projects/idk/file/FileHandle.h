#pragma once

#include <fstream>

#include "FileSystemInternal.h"
#include "FileSystemIncludes.h"

namespace idk
{
	class FStreamWrapper : public std::fstream
	{
	public:
		FStreamWrapper(FStreamWrapper&& rhs);
		FStreamWrapper& operator=(FStreamWrapper&& rhs);
		~FStreamWrapper();

		friend class FileSystem;
		friend struct FileHandle;
	private:
		FStreamWrapper() = default;

		int64_t _handle_index = -1;
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
		
		string_view			GetFullPath() const;
		string_view			GetRelPath() const;
		string_view			GetMountPath() const;

		string_view			GetExtension() const;
		
		string_view			GetParentFullPath() const;
		string_view			GetParentRelPath() const;
		string_view			GetParentMountPath() const;

		FS_CHANGE_STATUS	GetStatus() const;

		bool				CanOpen() const;
		
		FStreamWrapper		Open(FS_PERMISSIONS perms, bool binary_stream = false);
		FILEWrapper			OpenC(FS_PERMISSIONS perm, bool binary_stream = false);

		explicit	operator bool() const;

		friend class FileSystem;
	private:
		FileHandle(const file_system_detail::fs_key& key, bool is_file = true);

		bool validate() const;
		bool validateFull() const;

		int64_t _ref_count		= -1;
		file_system_detail::fs_key _key{};
		bool _is_regular_file = true;
	};
}