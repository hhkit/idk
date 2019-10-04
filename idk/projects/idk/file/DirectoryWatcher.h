#pragma once
#include "FileSystemInternal.h"

namespace idk::file_system_detail
{
	class DirectoryWatcher
	{
	public:
		void WatchDirectory(fs_dir& dir);
		void UpdateWatchedDir(const fs_mount& mount, fs_dir& dir);

		void RefreshDir(file_system_detail::fs_dir& dir);
		void RefreshTree(file_system_detail::fs_dir& dir);

		// Calling this will also clear all the changed files.
		void ResolveAllChanges();

		friend class FileSystem;
	private:

		vector<fs_key> changed_files;
		vector<fs_key> changed_dirs;

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
		// fs_file& addInternalFile(string_view full_path);

		file_system_detail::fs_key	fileCreate(file_system_detail::fs_dir& dir, const std::filesystem::path& p);
		void						fileDelete(file_system_detail::fs_file& file);
		void						fileRename(file_system_detail::fs_dir& dir, file_system_detail::fs_file& file, const std::filesystem::path& p, FS_CHANGE_STATUS status = FS_CHANGE_STATUS::RENAMED);

		file_system_detail::fs_key	dirCreate(file_system_detail::fs_dir& dir, const std::filesystem::path& p);
		void						dirDelete(file_system_detail::fs_dir& dir);
		void						dirRename(file_system_detail::fs_dir& Mountdir, file_system_detail::fs_dir& dir, const std::filesystem::path& p, FS_CHANGE_STATUS status = FS_CHANGE_STATUS::RENAMED);
	};
	
}
