#include "pch.h"
#include "VulkanGlslLoader.h"
#include <filesystem>
namespace idk::vkn
{
	string FileName(const FileHandle& path_to_resource)
	{
		//TODO actually get the file name
		return string(path_to_resource.GetFullPath());
	}
	FileResources VulkanGlslLoader::Create(FileHandle path_to_resource)
	{
		auto cmd = ("..\\tools\\glslc.exe " + FileName(path_to_resource) + " -o \"" + FileName(path_to_resource) + "spv\"");
		system(cmd.c_str());
 		return FileResources{};
	}

}