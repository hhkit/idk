#pragma once

#include <fstream>

namespace idk
{
	class FStreamWrapper : public std::fstream
	{
	public:
		FStreamWrapper(FStreamWrapper&& rhs);
		FStreamWrapper& operator=(FStreamWrapper&& rhs);
		~FStreamWrapper();

		friend class FileSystem;
		friend struct FileHandle;
	private:
		FStreamWrapper() = default;

		int64_t _handle_index = -1;
	};

	class FILEWrapper
	{
	public:
		FILE* data() { return _fp; }

		friend struct FileHandle;
	private:
		FILE* _fp = nullptr;
	};

	enum class FS_PERMISSIONS
	{
		READ,
		WRITE,
		APPEND
	};


	struct FileHandle
	{
		string_view		GetFullPath() const;
		string_view		GetMountPath() const;
		
		string_view		GetParentFullPath() const;
		string_view		GetParentMountPath() const;

		bool			CanOpen() const;
		FStreamWrapper	Open(FS_PERMISSIONS perms, bool binary_stream = false);
		FILEWrapper		OpenC(FS_PERMISSIONS perm, bool binary_stream);

		explicit	operator bool();

		friend class FileSystem;
	private:
		int64_t _handle_index	= -1;
		int64_t _ref_count		= -1;
	};
}