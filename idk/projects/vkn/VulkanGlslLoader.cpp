#include "pch.h"
#include "VulkanGlslLoader.h"
#include <filesystem>
namespace idk::vkn
{
	string FileName(const FileHandle& path_to_resource)
	{
		//TODO actually get the file name
		return "";
	}
	FileResources VulkanGlslLoader::Create(FileHandle path_to_resource)
	{
		system((std::filesystem::current_path().string() + "tools/glslang.exe" + FileName(path_to_resource) + " -o\"" + FileName(path_to_resource) + "spv\"").c_str());
		return FileResources{};
	}

}