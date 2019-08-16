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

TEST(FileSystem, TestMount)
{
	using namespace idk;
	Core c;
	
	auto& vfs = Core::GetSystem<FileSystem>();
	vfs.Init();
	vfs.Mount(string{ vfs.GetBaseDir().data() } +"/resource", "/TestMount");
}

TEST(FileSystem, TestFileWatch)
{
	using namespace idk;
	Core c;

	auto& vfs = Core::GetSystem<FileSystem>();
	vfs.Init();
	string base_dir = string{ vfs.GetBaseDir().data() };

	// Remove the file if it exists already
	if (FS::exists(FS::path{ base_dir + "/resource/test_watch.txt" }))
	{
		string remove_file = base_dir + "/resource/test_watch.txt";
		EXPECT_TRUE(remove(remove_file.c_str()) == 0);
	}

	// Mounting the directory. 
	vfs.Mount(base_dir +"/resource", "/TestMount");

	// Test create:
	{
		std::ofstream{ base_dir + "/resource/test_watch.txt", std::ios::out };
	}
	vfs.Update();
	
	// Test Write
	{
		std::ofstream of{ base_dir + "/resource/test_watch.txt", std::ios::out };
		of << "Test_Watch" << std::endl;
	}
	vfs.Update();

	// Test Delete
	{
		string remove_file = base_dir + "/resource/test_watch.txt";
		EXPECT_TRUE(remove(remove_file.c_str()) == 0);
	}
	vfs.Update();
	
}