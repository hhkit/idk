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
			string base_dir = string{ vfs.GetBaseDir().data() };\
			FS::create_directories(base_dir + "/resource/FS_UnitTests/");\
			remove(string {base_dir + "/resource/FS_UnitTests/test_watch.txt"	}.c_str());\
			remove(string {base_dir + "/resource/FS_UnitTests/test_open.txt"	}.c_str());\
			remove(string {base_dir + "/resource/FS_UnitTests/test_write.txt"	}.c_str());\
			vfs.Mount(string{ vfs.GetBaseDir().data() } +"/resource/FS_UnitTests/", "/FS_UnitTests");


TEST(FileSystem, TestMount)
{
	INIT_FILESYSTEM_UNIT_TEST();

	vfs.Mount(base_dir + "/resource/FS_UnitTests/", "/FS_UnitTests");
	vfs.Update();
}

struct test
{
	int i;
	double d;
};

TEST(FileSystem, TestFileWatchBasic)
{
	INIT_FILESYSTEM_UNIT_TEST();

	// Test create:
	{
		std::ofstream{ base_dir + "/resource/FS_UnitTests/test_watch.txt", std::ios::out };
	}
	vfs.Update();
	
	// Test Write
	{
		std::ofstream of{ base_dir + "/resource/FS_UnitTests/test_watch.txt", std::ios::out };
		of << "Test_Watch" << std::endl;
	}
	vfs.Update();

	// Test Delete
	{
		string remove_file = base_dir + "/resource/FS_UnitTests/test_watch.txt";
		EXPECT_TRUE(remove(remove_file.c_str()) == 0);
	}
	vfs.Update();

	// Test Create 2 Files-> Delete 1st one -> Create 1 File (Should have the same handle and prev handle should be invalidated)
}

TEST(FileSystem, TestFileHandles)
{
	INIT_FILESYSTEM_UNIT_TEST();

	// Test Write
	{
		auto valid_handle   = vfs.OpenWrite("/FS_UnitTests/test_write.txt");	// Should create a file
		auto invalid_handle = vfs.OpenWrite("/FS_UnitTests/test_write.txt");	// Should be an invalid handle

		// Checking if handles are valid or not
		EXPECT_TRUE(static_cast<bool>(valid_handle));
		EXPECT_FALSE(static_cast<bool>(invalid_handle));

		// Checking if writing returns the correct number of bytes
		size_t write_len = strlen("test_write");
		EXPECT_TRUE(valid_handle.Write("test_write", write_len) == write_len);
		EXPECT_TRUE(invalid_handle.Write("test_write", write_len) == 0);

		// Test move constructor
		auto move_construct{ std::move(valid_handle) };
		EXPECT_TRUE(static_cast<bool>(move_construct));
		EXPECT_FALSE(static_cast<bool>(valid_handle));
		 
		// Test move assignment
		FileHandle move_asssign = std::move(move_construct);
		EXPECT_TRUE(static_cast<bool>(move_asssign));
		EXPECT_FALSE(static_cast<bool>(move_construct));
	}
	vfs.Update();


	// Test Read
	// {
	// 	auto invalid_handle = vfs.OpenRead("/FS_UnitTests/test_read.txt");
	// 	EXPECT_FALSE(invalid_handle);
	// 
	// 	EXPECT_TRUE(invalid_handle.Read(nullptr, 5) == 0);
	// 	{
	// 		auto create_file = vfs.OpenWrite("/FS_UnitTests/test_read.txt");
	// 	}
	// 	auto valid_handle = vfs.OpenRead("/FS_UnitTests/test_read.txt");
	// }
	// 

	
	// SHUTDOWN_FILESYSTEM_UNIT_TEST()
}