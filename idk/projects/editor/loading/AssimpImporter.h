#pragma once
#include <idk.h>
#include <res/FileLoader.h>


namespace idk
{
	class AssimpImporter
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle path_to_resource, const MetaBundle& path_to_meta) override;

		// Keywords
		static const char* root_bone_keyword;
		static const char* bone_end_keyword; 
		static const char* bone_include_keyword;
		static const char* bone_exclude_keyword;
		
		AssimpImporter();
		~AssimpImporter();
	private:
		// For Vulkan meshes
		struct Data;
		unique_ptr<Data> _data;
	};
}