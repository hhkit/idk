#pragma once
#include "FileSystemInternal.h"


namespace idk::file_system_detail
{
	class DirectoryWatcher
	{
	public:
		void WatchDirectory(fs_dir& dir);
		void UpdateWatchedDir(fs_mount& mount, fs_dir& dir);

		void RefreshDir(file_system_detail::fs_dir& dir);
		void RefreshTree(file_system_detail::fs_dir& dir);

		// Calling this will also clear all the changed files.
		void ResolveAllChanges();

	private:
		vector<fs_key> changed_files;
		vector<fs_key> changed_dirs;

		void checkFilesCreated(file_system_detail::fs_dir& dir);
		void checkFilesDeleted(file_system_detail::fs_dir& dir);
		void checkFilesRenamed(file_system_detail::fs_dir& dir);
		void checkFilesWrittenTo(file_system_detail::fs_dir& dir);

		// fs_file& addInternalFile(string_view full_path);
	};
	
}
