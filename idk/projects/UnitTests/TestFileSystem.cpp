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

#define INIT_FILESYSTEM_UNIT_TEST()\
			using namespace idk;\
			Core c;\
			FileSystem& vfs = Core::GetSystem<FileSystem>();\
			vfs.Init();\
			string exe_dir = string{ vfs.GetExeDir().data() };\
			FS::create_directories(exe_dir + "/resource/FS_UnitTests/test_sub_dir1/recurse_dir");\
			{	std::ofstream{ exe_dir + "/resource/FS_UnitTests/test_sub_dir1/test_sub_file.txt" }; \
				std::ofstream{exe_dir + "/resource/FS_UnitTests/test_read.txt"};\
				std::ofstream{exe_dir + "/resource/FS_UnitTests/test_sub_dir1/recurse_dir/recurse_sub_file.txt"};};\
			std::filesystem::remove_all(string{ exe_dir + "/resource/FS_UnitTests/test_dir_2/" }.c_str());\
			remove(string {exe_dir + "/resource/FS_UnitTests/test_watch.txt"	}.c_str());\
			remove(string {exe_dir + "/resource/FS_UnitTests/test_write.txt"	}.c_str());\
			vfs.Mount( exe_dir + "/resource/FS_UnitTests/", "/FS_UnitTests");


TEST(FileSystem, TestMount)
{
	INIT_FILESYSTEM_UNIT_TEST();

	vfs.Mount(exe_dir + "resource/FS_UnitTests/", "/FS_UnitTests");
	// Should not work but should not crash too.
	auto bad_file = vfs.GetFile("/blah/haha.txt");

	vfs.DumpMounts();
	vfs.Update();
}

TEST(FileSystem, TestFileWatchBasic)
{
	INIT_FILESYSTEM_UNIT_TEST();

	// Test create:
	{
		std::ofstream{ exe_dir + "/resource/FS_UnitTests/test_watch.txt", std::ios::out };

		vfs.Update();

		// Checking if querying is correct
		auto changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 1);
		changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED);
		EXPECT_TRUE(changes.size() == 1);
		// No files deleted
		changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::DELETED);
		EXPECT_TRUE(changes.size() == 0);

		// Checking if we resolved all changes properly
		vfs.Update();
		changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 0);
	}
	
	
	// Test Write
	{
		std::ofstream of{ exe_dir + "/resource/FS_UnitTests/test_watch.txt", std::ios::out };
		of << "Test_Watch" << std::endl;
		vfs.Update();

		// Checking if querying is correct
		auto changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 1);
		changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN);
		EXPECT_TRUE(changes.size() == 1);
		// No files deleted
		changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::DELETED);
		EXPECT_TRUE(changes.size() == 0);

		// Checking if we resolved all changes properly
		vfs.Update();
		changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 0);
	}

	// Test Delete
	{
		string remove_file = exe_dir + "/resource/FS_UnitTests/test_watch.txt";
		EXPECT_TRUE(remove(remove_file.c_str()) == 0);
		vfs.Update();

		// Checking if querying is correct
		auto changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 1);
		changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::DELETED);
		EXPECT_TRUE(changes.size() == 1);
		// No files deleted
		changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED);
		EXPECT_TRUE(changes.size() == 0);

		// Checking if we resolved all changes properly
		vfs.Update();
		changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 0);
	}

	// Create Dir then create file inside dir
	{
		FS::create_directories(exe_dir + "/resource/FS_UnitTests/test_dir_2/");
		std::ofstream{ exe_dir + "/resource/FS_UnitTests/test_dir_2/blah.txt", std::ios::out };
		vfs.Update();

		auto changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 1);
		vfs.Update();
		changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 0);

		remove(string(exe_dir + "/resource/FS_UnitTests/test_dir_2/blah.txt").c_str());
		vfs.Update();
		changes = vfs.QueryFileChangesAll();
		EXPECT_TRUE(changes.size() == 1);
	}
	
	// Test Create 2 Files-> Delete 1st one -> Create 1 File (Should have the same handle and prev handle should be invalidated)
}

TEST(FileSystem, TestFileHandle)
{
	INIT_FILESYSTEM_UNIT_TEST();

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
}

TEST(FileSystem, TestFileOpen)
{
	INIT_FILESYSTEM_UNIT_TEST();

	// Test Open bad file path
	{
		FileHandle bad_file_handle = vfs.GetFile("/FS_UnitTests/bad_path/test_read.txt");

		auto bad_stream = vfs.Open("/FS_UnitTests/bad_path/test_read.txt", FS_PERMISSIONS::READ);
		auto bad_stream2 = bad_file_handle.Open(FS_PERMISSIONS::READ);

		EXPECT_FALSE(bad_stream.is_open());
		EXPECT_FALSE(bad_stream2.is_open());
	}

	// Test read empty file
	{
		FileHandle file_handle = vfs.GetFile("/FS_UnitTests/test_read.txt");
		
		auto valid_handle = vfs.Open("/FS_UnitTests/test_read.txt", FS_PERMISSIONS::READ);
		auto invalid_handle = file_handle.Open(FS_PERMISSIONS::READ);
		EXPECT_TRUE(valid_handle.is_open());
		EXPECT_FALSE(invalid_handle.is_open());

		// Should immediately get EOF
		char buffer[100]{ 0 };
		EXPECT_FALSE(valid_handle.read(buffer, 10));
		EXPECT_FALSE(valid_handle.getline(buffer, 10));
	}
	vfs.Update();

	// Test Write
	{
		auto valid_handle   = vfs.Open("/FS_UnitTests/test_write.txt", FS_PERMISSIONS::WRITE);	// Should create a file
		auto invalid_handle = vfs.Open("/FS_UnitTests/test_write.txt", FS_PERMISSIONS::WRITE);	// Should be an invalid handle

		// Checking if handles are valid or not
		EXPECT_TRUE(valid_handle.is_open());
		EXPECT_FALSE(invalid_handle.is_open());

		// Checking if writing returns the correct number of bytes
		size_t write_len = strlen("123\ngetline test\nEOF test\n");
		auto& res = valid_handle.write("123\ngetline test\nEOF test\n", write_len);
		// valid_handle.flush();
		EXPECT_TRUE(res);
		EXPECT_TRUE(valid_handle.write("456", 3));

		// Writing to invalid handle
		EXPECT_TRUE(!invalid_handle.write("invalid", 3));
	}
	vfs.Update();

	// Test Read
	// {
	// 	auto valid_handle = vfs.Open("/FS_UnitTests/test_write.txt");
	// 	auto invalid_handle = vfs.OpenRead("/FS_UnitTests/blah.txt");
	// 	EXPECT_TRUE(static_cast<bool>(valid_handle));
	// 	EXPECT_FALSE(static_cast<bool>(invalid_handle));
	// 
	// 	// Reading from invalid handle
	// 	EXPECT_TRUE(invalid_handle.Read(nullptr, 5) < 0);
	// 
	// 	char buffer[100] { 0 };
	// 	auto res = valid_handle.Read(buffer, 1);
	// 	EXPECT_TRUE(res >= 0);
	// 	EXPECT_TRUE(strcmp(buffer, "1") == 0);
	// 
	// 	res = valid_handle.Read(buffer, 3);
	// 	EXPECT_TRUE(res >= 0);
	// 	EXPECT_TRUE(strcmp(buffer, "23\n") == 0);
	// 
	// 	res = valid_handle.GetLine(buffer, 100);
	// 	EXPECT_TRUE(res >= 0);
	// 	EXPECT_TRUE(strcmp(buffer, "getline test") == 0);
	// }
	// vfs.Update();
	// 
	// // Test Read In Loop
	// {
	// 	{
	// 		auto write_handle = vfs.OpenWrite("/FS_UnitTests/test_write.txt");	// Should create a file
	// 		write_handle.Write("123\n456\n789\n", strlen("123\n456\n789\n"));
	// 	}
	// 
	// 	auto valid_handle = vfs.OpenRead("/FS_UnitTests/test_write.txt");
	// 	EXPECT_TRUE(static_cast<bool>(valid_handle));
	// 
	// 	char buffer[100]{ 0 };
	// 	int res = valid_handle.Read(buffer, 4);
	// 	std::cout << "Testing Read..." << std::endl;
	// 	while (res >= 0)
	// 	{
	// 		std::cout << buffer;
	// 		res = valid_handle.Read(buffer, 4);
	// 	}
	// 
	// 	// Both Read and GetLine should fail after this.
	// 	res = valid_handle.Read(buffer, 100);
	// 	EXPECT_TRUE(res < 0);
	// 
	// 	res = valid_handle.GetLine(buffer, 100);
	// 	EXPECT_TRUE(res < 0);
	// 
	// }
	// vfs.Update();
	// 
	// // Test GetLine In Loop
	// {
	// 	auto valid_handle = vfs.OpenRead("/FS_UnitTests/test_write.txt");
	// 	EXPECT_TRUE(static_cast<bool>(valid_handle));
	// 
	// 	char buffer[100]{ 0 };
	// 	int res = 0;
	// 
	// 	std::cout << "Testing GetLine..." << std::endl;
	// 	for (size_t i = 0; i < 3; ++i)
	// 	{
	// 		int res = valid_handle.GetLine(buffer, 100);
	// 		EXPECT_TRUE(res >= 0);
	// 
	// 		std::cout << buffer << std::endl;
	// 	}
	// 
	// 	// Both Read and GetLine should fail after this.
	// 	res = valid_handle.Read(buffer, 100);
	// 	EXPECT_TRUE(res < 0);
	// 
	// 	res = valid_handle.GetLine(buffer, 100);
	// 	EXPECT_TRUE(res < 0);
	// }
	// 
	// // Test read/write error
	// {
	// 	auto read_handle = vfs.OpenRead("/FS_UnitTests/test_read.txt");
	// 	auto write_handle = vfs.OpenWrite("/FS_UnitTests/test_write.txt");
	// 
	// 	EXPECT_TRUE(static_cast<bool>(read_handle));
	// 	EXPECT_TRUE(static_cast<bool>(write_handle));
	// 
	// 	// Calling Write on read-only handle
	// 	auto res = read_handle.Write("blah", 4);
	// 	EXPECT_TRUE(res < 0);
	// 
	// 	// Calling Read on write-only handle
	// 	char buffer[100]{ 0 };
	// 	res = write_handle.Read(buffer, 5);
	// 	EXPECT_TRUE(res < 0);
	// 
	// 	// Calling GetLine on write-only handle
	// 	res = write_handle.GetLine(buffer, 5);
	// 	EXPECT_TRUE(res < 0);
	// }
	// // SHUTDOWN_FILESYSTEM_UNIT_TEST()
}

