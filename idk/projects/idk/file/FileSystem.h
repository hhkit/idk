#pragma once
#include <core/ISystem.h>
#include <fstream>

#include "DirectoryWatcher.h"
#include "PathHandle.h"

namespace idk
{
	class FileSystem : public ISystem
	{
	public:
		void Init() override;
		void Update();
		void Shutdown() override;

		// Files/Directory Getters
		// =====================================================================================================
		PathHandle			GetEntry					(string_view mountPath)				const;	// Tries GetFile then GetDir
		PathHandle			GetFile					(string_view mountPath)				const;
		PathHandle			GetDir					(string_view mountPath)				const;
		
		string				GetFullPath				(string_view mountPath)				const;
		vector<PathHandle>	GetFilesWithExtension	(string_view mountPath,
													 string_view extension, 
													 FS_FILTERS filters = FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS)		const;

		vector<PathHandle>  GetEntries				(string_view mountPath,
													 FS_FILTERS filters = FS_FILTERS::FILE | FS_FILTERS::DIR, string_view ext = string_view{}) const;

		bool		Exists		(string_view mountPath)		const;
		bool		ExistsFull	(string_view fullPath)		const;

		// Get some static paths. These paths aren't changed during the program's lifetime.
		// =====================================================================================================
		string_view GetSolutionDir	()	const { return _sol_dir; }
		string_view GetAppDataDir	()	const { return _app_data_dir; }
		string_view GetExeDir		()	const { return _exe_dir; }
		string_view GetAssetDir		()	const { return _asset_dir; }
		
		// File changes
		// =====================================================================================================
		vector<PathHandle>  QueryFileChangesAll			()								const;
		vector<PathHandle>  QueryFileChangesByExt		(string_view ext)				const;
		vector<PathHandle>  QueryFileChangesByChange	(FS_CHANGE_STATUS change)		const;

		// Some setters
		// =====================================================================================================
		void		SetAssetDir(string_view dir) { _asset_dir = dir; }
		
		// Mounting/dismounting. Mounting adds a virtual path that u can use in all filesystem calls.
		// =====================================================================================================
		void Mount(string_view fullPath, string_view mountPath, bool watch = true);
		void Dismount(const string& mountPath);	// TODO!

		// Open files
		// =====================================================================================================
		FStreamWrapper Open(string_view mountPath, FS_PERMISSIONS perms, bool binary_stream = false);
		
		// Making a directory/renaming
		// =====================================================================================================
		int Mkdir(string_view mountPath); // Should create all sub directories if they dont exists
		bool Rename(string_view mountPath, string_view new_name);
		// TODO: Copy, paste, delete
		
		// Debug
		// =====================================================================================================
		void DumpMounts() const;

		// Friends
		// =====================================================================================================
		friend class	file_system_detail::DirectoryWatcher;
		friend struct	PathHandle;
		friend class	FStreamWrapper;
	private:
		hash_table<string, size_t>					_mount_table;
		vector<file_system_detail::fs_mount>		_mounts;

		string _sol_dir;
		string _app_data_dir;
		string _exe_dir;
		string _asset_dir;

		file_system_detail::fs_file		_empty_file;
		file_system_detail::fs_dir		_empty_dir;

		file_system_detail::DirectoryWatcher _directory_watcher;

		// initializtion
		void initMount		(size_t index, string_view fullPath, string_view mountPath, bool watch);
		void initFile		(file_system_detail::fs_file& f, file_system_detail::fs_dir& p_dir, std::filesystem::path& p);
		void initDir		(file_system_detail::fs_dir& f, file_system_detail::fs_dir& p_dir, std::filesystem::path& p);

		// Helper recursion functions
		void recurseSubDir				(size_t index, int8_t depth, file_system_detail::fs_dir& mountSubDir, bool watch);
		// void recurseSubDirExtensions	(vector<PathHandle>& vec_handles, const file_system_detail::fs_dir& subDir, string_view extension) const;

		// Helper Getters
		file_system_detail::fs_file&		getFile	(file_system_detail::fs_key& node);
		file_system_detail::fs_dir&			getDir	(file_system_detail::fs_key& node);

		const file_system_detail::fs_file&	getFile	(const file_system_detail::fs_key& node) const;
		const file_system_detail::fs_dir&	getDir	(const file_system_detail::fs_key& node) const;

		file_system_detail::fs_key			getFile	(string_view mountPath) const;
		file_system_detail::fs_key			getDir	(string_view mountPath) const;


		// Other auxiliary helpers
		file_system_detail::fs_key		requestFileSlot			(file_system_detail::fs_mount& mount, int8_t depth);
		file_system_detail::fs_key		requestDirSlot			(file_system_detail::fs_mount& mount, int8_t depth);
		file_system_detail::fs_file&	createAndGetFile		(string_view mountPath);

		vector<string>					tokenizePath			(string_view fullPath)					const;
		int								validateMountPath		(string_view mountPath)					const;
		int								validateFileMountPath	(string_view mountPath)					const;
		int								validateDirMountPath	(string_view mountPath)					const;
	
		void							dumpMount				(const file_system_detail::fs_mount& mount)					const;
		void							dumpDir					(const file_system_detail::fs_dir& mount, string prefix)	const;

	};

}