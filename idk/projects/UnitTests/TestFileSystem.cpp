//////////////////////////////////////////////////////////////////////////////////
//@file		TestFileSystem.cpp
//@author	Joseph Cheng
//@param	Email : 
//@date		17 AUG 2019
//@brief	Unit test for file system
//////////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <fstream>
#include <filesystem>

#include <core/Core.h>
#include <file/FileSystem.h>

namespace FS = std::filesystem;

static const idk::string READ_DATA{ "0123456789\nqwerty" };

#define INIT_FILESYSTEM_UNIT_TEST()\
using namespace idk;\
Core c;\
FileSystem& vfs = Core::GetSystem<FileSystem>();\
vfs.Init();\
const string EXE_DIR = vfs.GetExeDir().data();\
std::filesystem::remove_all(string{ EXE_DIR + "/FS_UnitTests/" }.c_str());\
FS::create_directories(EXE_DIR + "/FS_UnitTests/depth_1/depth_2");\
FS::create_directories(EXE_DIR + "/FS_UnitTests/multiple_dir_1");\
FS::create_directories(EXE_DIR + "/FS_UnitTests/depth_1/multiple_dir_2");\
{\
	std::ofstream{ EXE_DIR + "/FS_UnitTests/test_d0.txt", std::ios::out};\
	std::ofstream{ EXE_DIR + "/FS_UnitTests/depth_1/test_d1.txt", std::ios::out};\
	std::ofstream{ EXE_DIR + "/FS_UnitTests/depth_1/depth_2/test_d2.txt", std::ios::out};\
\
	std::ofstream r{ EXE_DIR + "/FS_UnitTests/test_read.txt", std::ios::out};\
	r << READ_DATA;\
	std::ofstream{ EXE_DIR + "/FS_UnitTests/test_write.txt", std::ios::out};\
\
	std::ofstream{ EXE_DIR + "/FS_UnitTests/multiple_dir_1/test_d1.txt", std::ios::out};\
	std::ofstream{ EXE_DIR + "/FS_UnitTests/depth_1/multiple_dir_2/test_d2.txt", std::ios::out};\
};\
vfs.Mount( EXE_DIR + "/FS_UnitTests/", "/FS_UnitTests");\
vfs.Update();

TEST(FileSystem, TestMount)
{
	INIT_FILESYSTEM_UNIT_TEST();
	// Duplicate mount not supported yet
	bool test_mount_dup = false;
	try
	{
		vfs.Mount(EXE_DIR + "FS_UnitTests/", "/FS_UnitTests");
	}
	catch (...)
	{
		test_mount_dup = true;
	}
	EXPECT_TRUE(test_mount_dup);

	// Should not work but should not crash too.
	auto bad_file = vfs.GetFile("/blah/haha.txt");

	// vfs.DumpMounts();
	vfs.Update();
}

bool WatchUpdateCheck(idk::FileSystem& vfs, idk::seconds time, idk::FS_CHANGE_STATUS status)
{
	using namespace idk;
	auto start_time = Clock::now();

	while (time > seconds{0})
	{
		auto curr_time = Clock::now();

		vfs.Update();

		// Checking if querying is correct
		auto changes = vfs.QueryFileChangesAll();
		bool all_check = changes.size() == 1;

		changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED);
		bool create_check = (status == FS_CHANGE_STATUS::CREATED) ? changes.size() == 1 : changes.size() == 0;

		changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN);
		bool write_check = (status == FS_CHANGE_STATUS::WRITTEN) ? changes.size() == 1 : changes.size() == 0;

		changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::DELETED);
		bool delete_check = (status == FS_CHANGE_STATUS::DELETED) ? changes.size() == 1 : changes.size() == 0;
		
		if (all_check && create_check && write_check && delete_check)
			return true;

		time -= duration_cast<seconds>(curr_time - start_time);
		start_time = curr_time;
	}

	return false;
}

void WatchClearCheck(idk::FileSystem& vfs)
{
	using namespace idk;
	// Checking if we resolved all changes properly
	vfs.Update();
	auto changes = vfs.QueryFileChangesAll();
	EXPECT_TRUE(changes.size() == 0);

	changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED);
	EXPECT_TRUE(changes.size() == 0);
	changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN);
	EXPECT_TRUE(changes.size() == 0);
	changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::DELETED);
	EXPECT_TRUE(changes.size() == 0);
}

void TestCreateWatch(idk::FileSystem& vfs)
{
	using namespace idk;
	// Create the test_watch file
	std::ofstream{ string{vfs.GetExeDir()} +"/FS_UnitTests/test_watch.txt", std::ios::out };

	// Checking if querying is correct
	EXPECT_TRUE(WatchUpdateCheck(vfs, seconds{ 2.0f }, FS_CHANGE_STATUS::CREATED));
	WatchClearCheck(vfs);
}

void TestWriteWatch(idk::FileSystem& vfs)
{
	using namespace idk;
	vfs.Update();
	auto time = FS::last_write_time(string{ vfs.GetExeDir() } +"/FS_UnitTests/test_watch.txt");
	// Write to the file
	{
		std::fstream of{ string{vfs.GetExeDir()} + "/FS_UnitTests/test_watch.txt", std::ios::out };
		of << "Test Write" << std::endl;
		of.close();
	}
	EXPECT_TRUE(time != FS::last_write_time(string{ vfs.GetExeDir() } +"/FS_UnitTests/test_watch.txt"));
	// Checking if querying is correct
	EXPECT_TRUE(WatchUpdateCheck(vfs, seconds{ 5.0f }, FS_CHANGE_STATUS::WRITTEN));
	WatchClearCheck(vfs);
}

void TestDeleteWatch(idk::FileSystem& vfs)
{
	using namespace idk;
	vfs.Update();

	string remove_file = string{ vfs.GetExeDir() } + "/FS_UnitTests/test_write.txt";
	EXPECT_TRUE(remove(remove_file.c_str()) == 0);

	// Checking if querying is correct
	EXPECT_TRUE(WatchUpdateCheck(vfs, seconds{ 2.0f }, FS_CHANGE_STATUS::DELETED));
	WatchClearCheck(vfs);
}

TEST(FileSystem, TestDirectoryWatch)
{
	INIT_FILESYSTEM_UNIT_TEST();
	TestCreateWatch(vfs);
	TestWriteWatch(vfs);
	TestDeleteWatch(vfs);
}

TEST(FileSystem, TestPathHandle)
{
	INIT_FILESYSTEM_UNIT_TEST();

	// Constructors and assignments
	{
		auto valid_handle = vfs.Open("/FS_UnitTests/test_write.txt", FS_PERMISSIONS::WRITE);		// Should create a file
		auto invalid_handle = vfs.Open("/FS_UnitTests/test_write.txt", FS_PERMISSIONS::WRITE);	// Should be an invalid handle

		// Checking if handles are valid or not
		EXPECT_TRUE(valid_handle.is_open());
		EXPECT_FALSE(invalid_handle.is_open());

		// Test move constructor
		auto move_construct{ std::move(valid_handle) };
		EXPECT_TRUE(move_construct.is_open());
		EXPECT_FALSE(valid_handle.is_open());

		// Test move assignment
		auto move_asssign = std::move(move_construct);
		EXPECT_TRUE(move_asssign.is_open());
		EXPECT_FALSE(move_construct.is_open());
		move_asssign.close();

		vfs.Update();
	}

	// Get files at varying depths
	{
		auto depth0_handle = vfs.GetFile("/FS_UnitTests/test_d0.txt");
		auto depth1_handle = vfs.GetFile("/FS_UnitTests/depth_1/test_d1.txt");
		auto depth2_handle = vfs.GetFile("/FS_UnitTests/depth_1/depth_2/test_d2.txt");
		EXPECT_TRUE(depth0_handle && depth1_handle && depth2_handle);
		
		EXPECT_TRUE(depth0_handle.GetFullPath() == EXE_DIR + "/FS_UnitTests\\test_d0.txt");
		EXPECT_TRUE(depth1_handle.GetFullPath() == EXE_DIR + "/FS_UnitTests\\depth_1\\test_d1.txt");
		EXPECT_TRUE(depth2_handle.GetFullPath() == EXE_DIR + "/FS_UnitTests\\depth_1\\depth_2\\test_d2.txt");
	}

	// Get file with same file name but different dir
	{
		auto dup_test1 = vfs.GetFile("/FS_UnitTests/depth_1/test_d1.txt");
		auto dup_test2 = vfs.GetFile("/FS_UnitTests/multiple_dir_1/test_d1.txt");

		EXPECT_FALSE(dup_test1.GetFullPath() == dup_test2.GetFullPath());
		EXPECT_FALSE(dup_test1.GetMountPath() == dup_test2.GetMountPath());
		EXPECT_FALSE(dup_test1.GetParentFullPath() == dup_test2.GetParentFullPath());
		EXPECT_FALSE(dup_test1.SameKeyAs(dup_test2));
		EXPECT_TRUE(dup_test1.GetFileName() == dup_test2.GetFileName());
	}
}

TEST(FileSystem, TestPathHandleInvalidate)
{
	INIT_FILESYSTEM_UNIT_TEST();
	// Invalidate handle by file deletion
	// Create a file
	auto valid_handle1 = vfs.Open("/FS_UnitTests/invalidate1.txt", FS_PERMISSIONS::WRITE);
	EXPECT_TRUE(valid_handle1.is_open());
	valid_handle1.close();

	// Get the file handle to the above file
	auto file_handle1 = vfs.GetFile("/FS_UnitTests/invalidate1.txt");
	EXPECT_TRUE(file_handle1);

	// file_handle1 is now in created status
	vfs.Update();

	// Delete the above file
	auto res = remove(string{ EXE_DIR + "/FS_UnitTests/invalidate1.txt" }.c_str());

	// After this update, file_handle1 is now under delete status
	vfs.Update();
	EXPECT_FALSE(file_handle1);

	// Create a new file
	auto valid_handle2 = vfs.Open("/FS_UnitTests/invalidate2.txt", FS_PERMISSIONS::WRITE);
	EXPECT_TRUE(valid_handle2.is_open());
	valid_handle2.close();

	// Get the file handle to the above file
	auto file_handle2 = vfs.GetFile("/FS_UnitTests/invalidate2.txt");
	EXPECT_TRUE(file_handle2);

	// file_handle2 should not reuse the same handle as file_handle1.
	EXPECT_FALSE(file_handle1.SameKeyAs(file_handle2));

	// Now file_handle1 is free to be reused as its change was resolved in this update
	vfs.Update();

	auto valid_handle3 = vfs.Open("/FS_UnitTests/invalidate3.txt", FS_PERMISSIONS::WRITE);
	EXPECT_TRUE(valid_handle3.is_open());
	valid_handle3.close();

	// Get the file handle to the above file
	auto file_handle3 = vfs.GetFile("/FS_UnitTests/invalidate3.txt");
	EXPECT_TRUE(file_handle3);

	// file_handle3 should reuse the first PathHandle that was created.
	EXPECT_TRUE(file_handle1.SameKeyAs(file_handle3));

	// Delete all files created
	remove(string{ EXE_DIR + "/FS_UnitTests/invalidate2.txt" }.c_str());
	remove(string{ EXE_DIR + "/FS_UnitTests/invalidate3.txt" }.c_str());
}

TEST(FileSystem, TestFileOpen)
{
	INIT_FILESYSTEM_UNIT_TEST();
	// Test Open bad file path
	{
		PathHandle bad_file_handle = vfs.GetFile("/FS_UnitTests/bad_path/test_read.txt");
		EXPECT_FALSE(bad_file_handle);

		auto bad_stream1 = vfs.Open("/FS_UnitTests/bad_path/test_read.txt", FS_PERMISSIONS::READ);
		auto bad_stream2 = bad_file_handle.Open(FS_PERMISSIONS::READ);

		auto bad_stream3 = vfs.Open("/FS_UnitTests/bad_path/test_read.txt", FS_PERMISSIONS::WRITE);
		auto bad_stream4 = bad_file_handle.Open(FS_PERMISSIONS::WRITE);

		EXPECT_FALSE(bad_stream1.is_open());
		EXPECT_FALSE(bad_stream2.is_open());
		EXPECT_FALSE(bad_stream3.is_open());
		EXPECT_FALSE(bad_stream4.is_open());
	}
}

TEST(FileSystem, TestFileRead)
{
	INIT_FILESYSTEM_UNIT_TEST();
	// Read file
	auto valid_handle = vfs.Open("/FS_UnitTests/test_read.txt", FS_PERMISSIONS::READ);
	auto invalid_handle = vfs.Open("/FS_UnitTests/test_read.txt", FS_PERMISSIONS::READ);
	EXPECT_TRUE(valid_handle.is_open());
	EXPECT_FALSE(invalid_handle.is_open());

	// Cant think of any more tests. Tell me if you find a bug
	std::stringstream ss;
	ss << valid_handle.rdbuf();
	string read_data = ss.str();

	valid_handle >> read_data;
	EXPECT_TRUE(read_data == READ_DATA);
	valid_handle.close();

	invalid_handle = vfs.Open("/FS_UnitTests/test_read.txt", FS_PERMISSIONS::READ);
	EXPECT_TRUE(invalid_handle.is_open());
}

TEST(FileSystem, TestFileWrite)
{
	INIT_FILESYSTEM_UNIT_TEST();
	auto file = vfs.GetFile("/FS_UnitTests/test_write.txt");
	EXPECT_TRUE(file);

	auto valid_handle = vfs.Open("/FS_UnitTests/test_write.txt", FS_PERMISSIONS::WRITE);
	auto invalid_handle = vfs.Open("/FS_UnitTests/test_write.txt", FS_PERMISSIONS::WRITE);	// Should be an invalid handle
	EXPECT_TRUE(valid_handle.is_open());
	EXPECT_FALSE(invalid_handle.is_open());

	valid_handle << "writing to this file";
	valid_handle.close();
	EXPECT_FALSE(valid_handle.is_open());

	std::ifstream test{ file.GetFullPath() };
	std::stringstream ss;
	ss << test.rdbuf();
	string read_data = ss.str();

	EXPECT_TRUE(read_data == "writing to this file");
	test.close();

	// Should now be valid
	invalid_handle = vfs.Open("/FS_UnitTests/test_write.txt", FS_PERMISSIONS::WRITE);	
	EXPECT_TRUE(invalid_handle.is_open());
}

TEST(FileSystem, TestRename)
{
	INIT_FILESYSTEM_UNIT_TEST();
	vfs.Open("/FS_UnitTests/test_rename.txt", FS_PERMISSIONS::WRITE);
	auto file_handle = vfs.GetFile("/FS_UnitTests/test_rename.txt");

	EXPECT_TRUE(file_handle.Rename("test_rename2.ext"));
	vfs.Update();
	EXPECT_TRUE(vfs.QueryFileChangesAll().size() == 0);
	EXPECT_FALSE(vfs.GetFile("/FS_UnitTests/test_rename.txt"));
	EXPECT_FALSE(vfs.GetFile("/FS_UnitTests/test_rename2.txt"));
	file_handle = vfs.GetFile("/FS_UnitTests/test_rename2.ext");;
	EXPECT_TRUE(file_handle);

	remove(file_handle.GetFullPath().data());
}


TEST(FileSystem, TestSpecialWatch)
{
	INIT_FILESYSTEM_UNIT_TEST();

	// Creating a file using write
	{
		{
			auto valid_handle = vfs.Open("/FS_UnitTests/blank_file.txt", FS_PERMISSIONS::WRITE);
			EXPECT_TRUE(valid_handle.is_open());
		}
		vfs.Update();
		// Creating a file. Should not trigger change
		auto changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 0);
	}
	vfs.Update();

	// Writing to a file
	{
		//Case 1: destructor called
		{
			auto valid_handle = vfs.Open("/FS_UnitTests/blank_file.txt", FS_PERMISSIONS::WRITE);
			EXPECT_TRUE(valid_handle.is_open());
			valid_handle << "write to file";
		}
		vfs.Update();
		// Should not trigger change
		auto changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 0);

		// Case 2: call close
		auto valid_handle = vfs.Open("/FS_UnitTests/blank_file.txt", FS_PERMISSIONS::WRITE);
		EXPECT_TRUE(valid_handle.is_open());
		valid_handle << "write to file";
		valid_handle.close();

		vfs.Update();
		// Should not trigger change
		changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 0);
	}
	vfs.Update();

	// Renaming a file
	{
		// Using PathHandle
		auto file = vfs.GetFile("/FS_UnitTests/blank_file.txt");
		EXPECT_TRUE(file.Rename("blank_file1.txt"));
		vfs.Update();
		// Should not trigger change
		auto changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 0);

		// Using FileSystem
		EXPECT_TRUE(vfs.Rename("/FS_UnitTests/blank_file1.txt", "blank_file2.txt"));
		vfs.Update();
		// Should not trigger change
		changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 0);
	}
}

TEST(FileSystem, CleanUp)
{
	using namespace idk;
	Core c;
	FileSystem& vfs = Core::GetSystem<FileSystem>();
	vfs.Init();
	const string EXE_DIR = vfs.GetExeDir().data();
	std::filesystem::remove_all(idk::string{ EXE_DIR + "/FS_UnitTests/" }.c_str());
}
