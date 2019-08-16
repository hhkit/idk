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
		FILESYSTEM_BAD_FILENAME,
		FILESYSTEM_BAD_ARGUMENT
	};

	// Opaque type. Do not touch internals
	struct FileHandle
	{
		FileHandle() = default;
		FileHandle(FileHandle&& rhs);
		~FileHandle();

		FileHandle operator=(FileHandle&& rhs);
		explicit operator bool();

		std::fstream& GetStream() { return stream; }
		FILE* GetFilePtr() { return fp; }
		
		template<typename T>
		size_t Write(const T& data)
		{
			size_t before = stream.tellp(); //current pos

			stream << data;
			
			return static_cast<size_t>(stream.tellp()) - before;
			
		}

		string_view GetParentDir() const;
		
		friend class FileSystem;
	private:
		FILE* fp = nullptr;
		std::fstream stream;

		int64_t handle_index = -1;
		int64_t ref_count = -1;

		FileSystem_ErrorCode error;
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
		string GetFullPath(string_view mountPath) const;

		bool Exists(string_view fullPath) const;
		bool ExistsFull(string_view fullPath) const;

		// Setters
		void SetRootDir(string_view dir) { root_dir = dir; }
		void SetBaseDir(string_view dir) { base_dir = dir; }
		void SetResourceDir(string_view dir) { resource_dir = dir; }
		
		// Mounting/dismounting. Mounting adds a virtual path that u can use in all filesystem calls.
		FileSystem_ErrorCode Mount(string_view fullPath, string_view mountPath, bool watch = true);
		FileSystem_ErrorCode Dismount(const string& mountPath);

		// Open/closing files
		FileHandle OpenRead(string_view mountPath, bool binary_stream = false);
		FileHandle OpenAppend(string_view mountPath);
		FileHandle OpenWrite(string_view mountPath);

		FileHandle OpenReadC(string_view mountPath);
		FileHandle OpenAppendC(string_view mountPath);
		FileHandle OpenWriteC(string_view mountPath);

		uint64_t Read(const FileHandle& handle, void* buffer, uint64_t len);
		uint64_t Write(const FileHandle& handle, const void* buffer, uint64_t len);

		FileSystem_ErrorCode Close(FileHandle& handle) const;

		// This functions affect the actual files outside of the system.
		FileSystem_ErrorCode Mkdir(string_view mountPath); // Should create all sub directories if they dont exists
		FileSystem_ErrorCode DelFile(string_view mountPath);
		
		FileSystem_ErrorCode GetLastError() const;


		friend class file_system_internal::DirectoryWatcher;
		friend struct file_system_internal::mount_t;
		friend struct FileHandle;
	private:
		hash_table<string, size_t> mount_table;
		vector<file_system_internal::mount_t> mounts;
		vector<file_system_internal::file_handle_t> file_handles;
		vector<file_system_internal::node_t> open_files;

		string root_dir;
		string base_dir;
		string resource_dir;

		FileSystem_ErrorCode last_error;

		file_system_internal::DirectoryWatcher directory_watcher;

		// initializtion
		void initMount(size_t index, string_view fullPath, string_view mountPath, bool watch);
		void recurseSubDir(size_t index, int8_t depth, file_system_internal::dir_t& mountSubDir, bool watch);

		// File watching
		
		// Helper Getters
		file_system_internal::file_t& getFile(file_system_internal::node_t& node);
		file_system_internal::dir_t& getDir(file_system_internal::node_t& node);

		file_system_internal::node_t getFile(string_view mountPath);
		file_system_internal::node_t getDir(string_view mountPath);

		bool isOpen(const file_system_internal::node_t& n);

		// Other auxiliary helpers
		size_t addFileHandle(const file_system_internal::node_t& handle);
		vector<string> tokenizePath(string_view fullPath) const;
		int validateFileMountPath(string_view mountPath) const;
		int validateDirMountPath(string_view mountPath) const;

		string getMountToken(const string& mountPath) const;
		bool validateHandle(FileHandle* handle) const;
	};

}