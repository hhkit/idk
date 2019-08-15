#pragma once
#include <core/ISystem.h>
#include <fstream>

#include "DirectoryWatcher.h"

namespace idk
{
	enum FileSystem_ErrorCode
	{
		FILESYSTEM_OK,
		FILESYSTEM_OTHER,
		FILESYSTEM_NO_MEM,
		FILESYSTEM_NO_INIT,
		FILESYSTEM_IS_INIT,
		FILESYSTEM_PAST_EOF,
		FILESYSTEM_NOT_MOUNTED,
		FILESYSTEM_NOT_FOUND,
		FILESYSTEM_NOT_FILE,
		FILESYSTEM_ONLY_READ,
		FILESYSTEM_ONLY_WRITE,
		FILESYSTEM_NOT_OPEN,
		FILESYSTEM_DUPLICATE,
		FILESYSTEM_FILE_CHANGED,
		FILESYSTEM_BAD_FILENAME
	};

	// Opaque type. Do not touch internals
	struct FileHandle
	{
		void* handle = nullptr;

		FileHandle() = default;
		FileHandle(FileHandle&& rhs);
		~FileHandle();

		FileHandle operator=(FileHandle&& rhs);
		explicit operator FILE* ();
		explicit operator std::fstream();

		string_view GetParentDir() const;

	private:
		FILE* fp = nullptr;
		std::fstream stream;
	};

	class FileSystem : public ISystem
	{
	public:
		void Init() override;
		void Update();
		void Shutdown() override;

		// Getters
		string_view GetRootDir() const { return root_dir; }
		string_view GetBaseDir() const { return base_dir; }
		string_view GetResourceDir() const { return resource_dir; }

		bool Exists(string_view fullPath) const;
		bool ExistsFull(string_view fullPath) const;
		// Setters
		void SetRootDir(string_view dir) { root_dir = dir; }
		void SetBaseDir(string_view dir) { base_dir = dir; }
		void SetResourceDir(string_view dir) { resource_dir = dir; }
		
		// Mounting/dismounting. Mounting adds a virtual path that u can use in all filesystem calls.
		FileSystem_ErrorCode Mount(const string& fullPath, const string& mountPath, bool watch = true);
		FileSystem_ErrorCode Dismount(const string& mountPath);

		// Open/closing files
		FileHandle OpenRead(string_view mountPath) const;
		FileHandle OpenAppend(string_view mountPath) const;
		FileHandle OpenWrite(string_view mountPath) const;

		uint64_t Read(const FileHandle& handle, void* buffer, uint64_t len);
		uint64_t Write(const FileHandle& handle, const void* buffer, uint64_t len);

		FileSystem_ErrorCode Close(FileHandle& handle) const;

		// This functions affect the actual files outside of the system.
		FileSystem_ErrorCode Mkdir(string_view mountPath); // Should create all sub directories if they dont exists
		FileSystem_ErrorCode DelFile(string_view mountPath);
		
		FileSystem_ErrorCode GetLastError() const;


		friend class file_system_internal::DirectoryWatcher;
	private:
		hash_table<string, int> mount_table;
		vector<file_system_internal::mount_t> mounts;
		vector<file_system_internal::file_handle_t> file_handles;
		vector<file_system_internal::node_t> open_files;

		string root_dir;
		string base_dir;
		string resource_dir;

		FileSystem_ErrorCode last_error;

		file_system_internal::DirectoryWatcher directory_watcher;

		// initializtion
		void initMount(size_t index, const string& fullPath, const string& mountPath, bool watch);
		void recurseSubDir(size_t index, int8_t depth, file_system_internal::dir_t& mountSubDir, bool watch);

		// File watching
		
		// Helper Getters
		file_system_internal::file_t& getFile(file_system_internal::node_t& node);
		bool isOpen(const file_system_internal::node_t& n);

		void RefreshDir(file_system_internal::dir_t& dir);
		void RefreshTree(file_system_internal::dir_t& dir);
		
		string getMountToken(const string& mountPath) const;
		bool validateHandle(FileHandle* handle) const;
	};

}