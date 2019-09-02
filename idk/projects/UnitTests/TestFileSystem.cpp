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
				std::ofstream{exe_dir + "/resource/FS_UnitTests/test_write.txt"};\
				std::ofstream{exe_dir + "/resource/FS_UnitTests/test_sub_dir1/recurse_dir/recurse_sub_file.txt"};};\
			std::filesystem::remove_all(string{ exe_dir + "/resource/FS_UnitTests/test_dir_2/" }.c_str());\
			remove(string {exe_dir + "/resource/FS_UnitTests/test_watch.txt"	}.c_str());\
			vfs.Mount( exe_dir + "/resource/FS_UnitTests/", "/FS_UnitTests");\
			vfs.Update();\


TEST(FileSystem, TestMount)
{
	INIT_FILESYSTEM_UNIT_TEST();
	bool test_mount_dup = false;
	try
	{
		vfs.Mount(exe_dir + "resource/FS_UnitTests/", "/FS_UnitTests");
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

TEST(FileSystem, TestCreate)
{
	INIT_FILESYSTEM_UNIT_TEST();

	std::ofstream{ exe_dir + "/resource/FS_UnitTests/test_watch.txt", std::ios::out };

	vfs.Update();

	// Checking if querying is correct
	auto changes = vfs.QueryFileChangesAll();
	EXPECT_TRUE(changes.size() == 1);
	changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED);
	EXPECT_TRUE(changes.size() == 1);
	// No files deleted or written
	changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::DELETED);
	EXPECT_TRUE(changes.size() == 0);
	changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN);
	EXPECT_TRUE(changes.size() == 0);

	// Checking if we resolved all changes properly
	vfs.Update();
	changes = vfs.QueryFileChangesAll();
	EXPECT_TRUE(changes.size() == 0);

}

TEST(FileSystem, TestWrite)
{
	INIT_FILESYSTEM_UNIT_TEST();

	std::ofstream of{ exe_dir + "/resource/FS_UnitTests/test_write.txt", std::ios::out };
	of << "Test Write" << std::endl;
	vfs.Update();

	// Checking if querying is correct
	auto changes = vfs.QueryFileChangesAll();
	EXPECT_TRUE(changes.size() == 1);
	changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN);
	EXPECT_TRUE(changes.size() == 1);

	// No files deleted or created
	changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::DELETED);
	EXPECT_TRUE(changes.size() == 0);
	changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED);
	EXPECT_TRUE(changes.size() == 0);

	// Checking if we resolved all changes properly
	vfs.Update();
	changes = vfs.QueryFileChangesAll();
	EXPECT_TRUE(changes.size() == 0);
}

TEST(FileSystem, TestDelete)
{
	INIT_FILESYSTEM_UNIT_TEST();

	string remove_file = exe_dir + "/resource/FS_UnitTests/test_write.txt";
	EXPECT_TRUE(remove(remove_file.c_str()) == 0);
	vfs.Update();

	// Checking if querying is correct
	auto changes = vfs.QueryFileChangesAll();
	EXPECT_TRUE(changes.size() == 1);
	changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::DELETED);
	EXPECT_TRUE(changes.size() == 1);

	// No files deleted or written
	changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED);
	EXPECT_TRUE(changes.size() == 0);
	changes = vfs.QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN);
	EXPECT_TRUE(changes.size() == 0);

	// Checking if we resolved all changes properly
	vfs.Update();
	changes = vfs.QueryFileChangesAll();
	EXPECT_TRUE(changes.size() == 0);
}

TEST(FileSystem, TestSpecial)
{
	INIT_FILESYSTEM_UNIT_TEST();
	FS::create_directories(exe_dir + "/resource/FS_UnitTests/test_dir_2/");
	std::ofstream{ exe_dir + "/resource/FS_UnitTests/test_dir_2/blah.txt", std::ios::out };
	vfs.Update();

	auto changes = vfs.QueryFileChangesAll();
	EXPECT_TRUE(changes.size() == 1);

	vfs.Update();
	changes = vfs.QueryFileChangesAll();
	EXPECT_TRUE(changes.size() == 0);
}


TEST(FileSystem, TestFileHandle)
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

		vfs.Update();
	}

	// Invalidate handle by file deletion
	{
		// Create a file
		auto valid_handle1 = vfs.Open("/FS_UnitTests/invalidate.txt", FS_PERMISSIONS::WRITE);
		EXPECT_TRUE(valid_handle1.is_open());
		valid_handle1.close();

		// Get the file handle to the above file
		auto file_handle1 = vfs.GetFile("/FS_UnitTests/invalidate.txt");
		EXPECT_TRUE(file_handle1);

		// file_handle1 is now in created status
		vfs.Update();
		
		// Delete the above file
		auto res = remove(string{ exe_dir + "/resource/FS_UnitTests/invalidate.txt" }.c_str());

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

		// file_handle3 should reuse the first FileHandle that was created.
		EXPECT_TRUE(file_handle1.SameKeyAs(file_handle3));

		// Delete all files created
		remove(string{ exe_dir + "/resource/FS_UnitTests/invalidate2.txt" }.c_str());
		remove(string{ exe_dir + "/resource/FS_UnitTests/invalidate3.txt" }.c_str());
	}
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

