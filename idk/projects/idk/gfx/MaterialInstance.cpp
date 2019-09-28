#include "stdafx.h"
#include "MaterialInstance.h"

namespace idk
{
	bool MaterialInstance::IsImageBlock(const string& name) const
	{
		bool ret = false;
		try
		{
			auto itr = uniforms.find(name);
			if (itr != uniforms.end())
			{
				auto& val = std::get<RscHandle<Texture>>(itr->second);
				val;
				ret = true;
			}
		}
		catch (...)
		{

		}
		return ret;
	}
	hash_table<string, RscHandle<Texture>> MaterialInstance::GetImageBlock(const string& name) const
	{
		//TODO actually get a block
		hash_table<string, RscHandle<Texture>> result;
		auto itr = uniforms.find(name);
		if (itr != uniforms.end())
		{
			//Todo, replace name with the individual names in the block
			result.emplace(name, std::get<RscHandle<Texture>>(itr->second));
		}
		return result;
	}
	string MaterialInstance::GetUniformBlock(const string& name) const
	{
		auto& uniform_block = uniforms;
		string result;
		auto itr = uniform_block.find(name);
		if (itr != uniform_block.end())
		{
			//Replace this with something that grabs all the data in the block and formats it according to the interface block rules.(Padding etc)
			result = std::visit([](auto& value) { return string{ r_cast<const char*>(&value),sizeof(value) }; }, itr->second);
		}
		return result;
	}
}
