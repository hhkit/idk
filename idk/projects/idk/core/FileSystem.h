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
		FILESYSTEM_DUPLICATE,
		FILESYSTEM_FILE_CHANGED,
		FILESYSTEM_BAD_FILENAME
	};

	class FileSystem : public ISystem
	{
		hash_table<string, int> mount_table;
		vector<filesystem_internal::mount_t> mounts;

		FileSystem_ErrorCode last_error;

	public:
		struct FileHandle
		{

		};

		void Init() override;
		void Run(float dt);
		void Shutdown() override;

		void SetRoot(const string& fullPath);
		
		FileSystem_ErrorCode Mount(const string& fullPath, const string& mountPath);
		FileSystem_ErrorCode Dismount(const string& mountPath);
		
		FileSystem_ErrorCode GetLastError() const;
	};

}