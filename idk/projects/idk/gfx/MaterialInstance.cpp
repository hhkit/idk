#include "stdafx.h"
#include "MaterialInstance.h"

namespace idk
{
	bool MaterialInstance::IsImageBlock(const string& name) const
	{
		bool ret = false;
		try
		{
			auto uni = GetUniform(name);
			if (uni)
			{
				auto& val = std::get<RscHandle<Texture>>(*uni);
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
		auto uni= GetUniform(name);
		if (uni)
		{
			//Todo, replace name with the individual names in the block
			result.emplace(name, std::get<RscHandle<Texture>>(*uni));
		}
		return result;
	}
	std::optional<UniformInstance> MaterialInstance::GetUniform(const string& name) const
	{
		std::optional<UniformInstance> result;
		auto itr = uniforms.find(name);
		if (itr == uniforms.end())
		{
			auto& mat = *material;
			itr = mat.uniforms.find(name);
			if (itr != mat.uniforms.end())
				result = itr->second;
		}
		else
		{
			result = itr->second;
		}
		return result;
	}
	string MaterialInstance::GetUniformBlock(const string& name) const
	{
		auto& uniform_block = uniforms;
		auto& mat = *material;
		string result;
		auto uni = GetUniform(name);
		if (uni)
		{
			//Replace this with something that grabs all the data in the block and formats it according to the interface block rules.(Padding etc)
			result = std::visit([](auto& value) { return string{ r_cast<const char*>(&value),sizeof(value) }; }, *uni);
		}
		return result;
	}
}
