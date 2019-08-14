#pragma once
#include "ISystem.h"
#include "FileSystemInternal.h"

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
		void* handle;
	};

	class FileSystem : public ISystem
	{
	private:
		hash_table<string, int> mount_table;
		vector<file_system_internal::mount_t> mounts;
		vector<file_system_internal::file_handle_t> file_handles;

		string base_dir;

		FileSystem_ErrorCode last_error;

		void initMount(size_t index, const string& fullPath, const string& mountPath);
		void recurseSubDir(size_t index, int8_t depth, file_system_internal::dir_t& mountSubDir);
		// void addHandle() const;

		string getMountToken(const string& mountPath) const;
		bool validateHandle(FileHandle* handle) const;
	public:
		void Init() override;
		void Run(float dt);
		void Shutdown() override;

		string GetBaseDir() const;
		bool Exists(const string& mountPath) const;
		
		// Mounting/dismounting. Mounting adds a virtual path that u can use in all filesystem calls.
		FileSystem_ErrorCode Mount(const string& fullPath, const string& mountPath); // Mount("yolo");
		FileSystem_ErrorCode Dismount(const string& mountPath);

		// Open/closing files
		FileHandle* OpenRead(const string& mountPath) const;
		FileHandle* OpenAppend(const string& mountPath) const;
		FileHandle* OpenWrite(const string& mountPath) const;

		uint64_t Read(FileHandle* handle, void* buffer, uint64_t len);
		uint64_t Write(FileHandle* handle, const void* buffer, uint64_t len);

		FileSystem_ErrorCode Close(FileHandle* handle) const;

		// This functions affect the actual files outside of the system.
		FileSystem_ErrorCode Mkdir(const string& mountPath); // Should create all sub directories if they dont exists
		FileSystem_ErrorCode DeleteFile(const string& mountPath);
		
		FileSystem_ErrorCode GetLastError() const;
	};

}