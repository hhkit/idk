#pragma once

#include <fstream>

#include "FileSystemInternal.h"
#include "FileSystemIncludes.h"

namespace idk
{
	// struct PathHandle;
	class FStreamWrapper : public std::fstream
	{
	public:
		FStreamWrapper(FStreamWrapper&& rhs);
		FStreamWrapper& operator=(FStreamWrapper&& rhs);
		~FStreamWrapper();

		// PathHandle GetHandle() const;

		// Override some files.
		void close();

		friend class FileSystem;
		friend struct PathHandle;
	private:
		FStreamWrapper() = default;
		
		file_system_detail::fs_key _file_key{};
	};

	class FILEWrapper
	{
	public:
		FILE* data() { return _fp; }

		friend struct PathHandle;
	private:
		FILE* _fp = nullptr;
	};

	struct PathHandle
	{
		PathHandle() = default;
		PathHandle(string_view mountPath);
		PathHandle(const char* mountPath);
		PathHandle(const string& mountPath);

		bool operator == (const PathHandle& rhs) const;
        bool operator != (const PathHandle& rhs) const;

		// Path Variable Getters
		// ====================================================================================
		string_view			GetFileName()	const; //Returns with extension
		
		string_view			GetFullPath()	const;
		string_view			GetRelPath()	const;
		string_view			GetMountPath()	const;

		string_view			GetExtension()	const;
		
		string_view			GetParentFullPath()  const;
		string_view			GetParentRelPath()	 const;
		string_view			GetParentMountPath() const;

		FS_CHANGE_STATUS	GetStatus() const;

		// Directory specific getters
		// ====================================================================================
		vector<PathHandle> GetFilesWithExtension(string_view ext, FS_FILTERS filters = FS_FILTERS::RECURSE_DIRS) const;
		vector<PathHandle> GetEntries(FS_FILTERS filters = FS_FILTERS::FILE | FS_FILTERS::DIR, string_view ext = string_view{}) const;

		// Bool checks
		// ====================================================================================
		bool				IsFile()	const { return _is_regular_file; }
		bool				IsDir()		const { return !_is_regular_file; }
		bool				CanOpen()	const;
		bool				SameKeyAs(const PathHandle& other) const;
		
		// Auxiliary functions
		// ====================================================================================
		FStreamWrapper		Open(FS_PERMISSIONS perms, bool binary_stream = false);
		FILEWrapper			OpenC(FS_PERMISSIONS perm, bool binary_stream = false);

		bool				Rename(string_view new_file_name);

		explicit	operator bool() const;

		friend class FileSystem;
		friend class FStreamWrapper;
	private:
		PathHandle(const file_system_detail::fs_key& key, bool is_file = true);

		// Use this if you don't care if the file really exists or not. 
		bool validate() const;
		// Use this if the file MUST exists
		bool validateFull() const;

		void renameDirUpdate();
		// void getFilesExtRecurse(const file_system_detail::fs_dir& dir, string_view ext, vector<PathHandle>& out) const;
		void getPathsRecurse(const file_system_detail::fs_dir& dir, FS_FILTERS filters, string_view ext, vector<PathHandle>& out) const;

		file_system_detail::fs_key _key{};
		int16_t _ref_count = -1;
		bool _is_regular_file = true;
	};
}