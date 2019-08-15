#pragma once
#include "FileSystemInternal.h"


namespace idk::file_system_internal
{
	class DirectoryWatcher
	{
	public:
		void WatchDirectory(dir_t& dir);
		void UpdateWatchedDir(mount_t& mount, dir_t& dir);

		void RefreshDir(file_system_internal::dir_t& dir);
		void RefreshTree(file_system_internal::dir_t& dir);

		// Calling this will also clear all the changed files.
		void ResolveAllChanges();

	private:
		void CheckFilesCreated(file_system_internal::dir_t& dir);
		void CheckFilesDeleted(file_system_internal::dir_t& dir);
		void CheckFilesRenamed(file_system_internal::dir_t& dir);
		void CheckFilesWrittenTo(file_system_internal::dir_t& dir);

		file_t& addInternalFile(string_view full_path);

		vector<node_t> changed_files;
		vector<node_t> changed_dirs;
	};
	
}
