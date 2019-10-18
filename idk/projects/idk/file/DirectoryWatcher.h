#pragma once
#include "FileSystemInternal.h"

namespace idk::file_system_detail
{
	class DirectoryWatcher
	{
	public:
		void WatchDirectory(fs_dir& dir);
		void UpdateWatchedDir(fs_dir& dir);

		// Calling this will also clear all the changed files.
		void ResolveAllChanges();

		friend class FileSystem;
	private:
		
		vector<fs_key> changed_files;
		vector<fs_key> changed_dirs;

		string print_log;

		void CheckAllPathChanges(file_system_detail::fs_dir& dir);
		void CheckFileChanges(file_system_detail::fs_dir& dir);
		void CheckDirChanges(file_system_detail::fs_dir& dir);

		void checkFilesCreated	(file_system_detail::fs_dir& dir);
		void checkFilesDeleted	(file_system_detail::fs_dir& dir);
		void checkFilesRenamed	(file_system_detail::fs_dir& dir);
		void checkFilesWritten	(file_system_detail::fs_dir& dir);

		void checkDirCreated	(file_system_detail::fs_dir& dir);
		void checkDirDeleted	(file_system_detail::fs_dir& dir);
		void checkDirRenamed	(file_system_detail::fs_dir& dir);

		void recurseAndAdd		(file_system_detail::fs_dir& dir);
		void recurseAndDelete	(file_system_detail::fs_dir& dir);
		void recurseAndRename	(file_system_detail::fs_dir& dir);

		file_system_detail::fs_key	fileCreate(file_system_detail::fs_dir& dir, const std::filesystem::path& p);
		void						fileDelete(file_system_detail::fs_file& file);
		void						fileRename(file_system_detail::fs_dir& dir, file_system_detail::fs_file& file, const std::filesystem::path& p);

		file_system_detail::fs_key	dirCreate(file_system_detail::fs_dir& dir, const std::filesystem::path& p);
		void						dirDelete(file_system_detail::fs_dir& dir);
		void						dirRename(file_system_detail::fs_dir& Mountdir, file_system_detail::fs_dir& dir, const std::filesystem::path& p);
	};
	
}
