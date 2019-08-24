#pragma once
#include <core/ISystem.h>
#include <fstream>

#include "DirectoryWatcher.h"
#include "FileHandle.h"

namespace idk
{
	enum class FileSystem_ErrorCode
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

	class FileSystem : public ISystem
	{
	public:
		void Init() override;
		void Update();
		void Shutdown() override;

		// Getters
		vector<FileHandle>	GetFilesWithExtension	(string_view mountPath,
													 string_view extension, 
													 bool recurse_sub_trees = true)		const;
		FileHandle			GetFile					(string_view mountPath)				const;
		string				GetFullPath				(string_view mountPath)				const;

		string_view GetSolutionDir	()	const { return _sol_dir; }
		string_view GetAppDataDir	()	const { return _app_data_dir; }
		string_view GetExeDir		()	const { return _exe_dir; }
		string_view GetAssetDir		()	const { return _asset_dir; }

		

		bool		Exists(string_view mountPath) const;
		bool		ExistsFull(string_view fullPath) const;

		// Setters
		void		SetAssetDir(string_view dir) { _asset_dir = dir; }
		
		// Mounting/dismounting. Mounting adds a virtual path that u can use in all filesystem calls.
		void Mount(string_view fullPath, string_view mountPath, bool watch = true);
		void Dismount(const string& mountPath);

		// Open/closing files
		FStreamWrapper Open(string_view mountPath, FS_PERMISSIONS perms, bool binary_stream = false);

		// This functions affect the actual files outside of the system.
		int Mkdir(string_view mountPath); // Should create all sub directories if they dont exists
		FileSystem_ErrorCode DelFile(string_view mountPath);
		
		FileSystem_ErrorCode GetLastError() const;

		
		void DumpMounts() const;

		friend class	file_system_detail::DirectoryWatcher;
		friend struct	file_system_detail::fs_mount;
		friend struct	FileHandle;
		friend struct	FileHandleC;
		friend class	FStreamWrapper;
	private:
		hash_table<string, size_t>					_mount_table;
		vector<file_system_detail::fs_mount>		_mounts;
		vector<file_system_detail::fs_file_handle>	_file_handles;

		string _sol_dir;
		string _app_data_dir;
		string _exe_dir;
		string _asset_dir;

		file_system_detail::fs_file		_empty_file;
		file_system_detail::fs_dir		_empty_dir;

		file_system_detail::DirectoryWatcher _directory_watcher;

		// initializtion
		void initMount		(size_t index, string_view fullPath, string_view mountPath, bool watch);
		void recurseSubDir	(size_t index, int8_t depth, file_system_detail::fs_dir& mountSubDir, bool watch);
		void initFile(file_system_detail::fs_file& f, file_system_detail::fs_dir& p_dir, std::filesystem::path& p);
		void initDir(file_system_detail::fs_dir& f, file_system_detail::fs_dir& p_dir, std::filesystem::path& p);
		void recurseSubDirExtensions(vector<FileHandle>& vec_handles, const file_system_detail::fs_dir& subDir, string_view extension) const;

		// Helper Getters
		file_system_detail::fs_file&		getFile	(file_system_detail::fs_key& node);
		file_system_detail::fs_dir&			getDir	(file_system_detail::fs_key& node);

		const file_system_detail::fs_file&	getFile	(const file_system_detail::fs_key& node) const;
		const file_system_detail::fs_dir&	getDir	(const file_system_detail::fs_key& node) const;

		file_system_detail::fs_key			getFile	(string_view mountPath) const;
		file_system_detail::fs_key			getDir	(string_view mountPath) const;


		// Other auxiliary helpers
		size_t							addFileHandle			(const file_system_detail::fs_key& handle);
		file_system_detail::fs_key		requestFileSlot			(file_system_detail::fs_mount& mount, int8_t depth);
		file_system_detail::fs_file&	createAndGetFile		(string_view mountPath);

		vector<string>					tokenizePath			(string_view fullPath)					const;
		bool							validateKey				(const file_system_detail::fs_key& key) const;
		int								validateMountPath		(string_view mountPath)					const;
		int								validateFileMountPath	(string_view mountPath)					const;
		int								validateDirMountPath	(string_view mountPath)					const;
		bool							validateHandle			(const FileHandle& handle)				const;
	
		void							dumpMount				(const file_system_detail::fs_mount& mount)					const;
		void							dumpDir					(const file_system_detail::fs_dir& mount, string prefix)	const;

	};

}