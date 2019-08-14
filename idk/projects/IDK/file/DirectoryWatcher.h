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

		void CheckFilesDeleted(file_system_internal::dir_t& dir);

	private:
		file_t& addInternalFile(string_view full_path);
	};
	
}
