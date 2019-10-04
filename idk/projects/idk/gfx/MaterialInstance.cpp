#include "stdafx.h"
#include "MaterialInstance.h"
#include <meta/variant.h>

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
        // https://www.khronos.org/registry/OpenGL/specs/gl/glspec45.core.pdf pg 138

        auto& mat = *material;
        const shadergraph::ValueType type = std::stoi(name.data() + sizeof("_UB") - 1);
        string data;

        switch (type)
        {
        case shadergraph::ValueType::FLOAT: // align contiguous (N)
            for (auto& [uni, val] : mat.uniforms)
            {
                if (val.index() == index_in_variant_v<float, UniformInstance>)
                {
                    auto sz = data.size();
                    data.resize(sz + sizeof(float));
                    memcpy_s(data.data() + sz, sizeof(float), &std::get<index_in_variant_v<float, UniformInstance>>(val), sizeof(float));
                }
            }
            return data;

        case shadergraph::ValueType::VEC2: // align contiguous (2N)
            for (auto& [uni, val] : mat.uniforms)
            {
                if (val.index() == index_in_variant_v<vec2, UniformInstance>)
                {
                    auto sz = data.size();
                    data.resize(sz + sizeof(vec2));
                    memcpy_s(data.data() + sz, sizeof(vec2), &std::get<index_in_variant_v<vec2, UniformInstance>>(val), sizeof(vec2));
                }
            }
            return data;

        case shadergraph::ValueType::VEC4: // align contiguous (4N)
            for (auto& [uni, val] : mat.uniforms)
            {
                if (val.index() == index_in_variant_v<vec4, UniformInstance>)
                {
                    auto sz = data.size();
                    data.resize(sz + sizeof(vec4));
                    memcpy_s(data.data() + sz, sizeof(vec4), &std::get<index_in_variant_v<vec4, UniformInstance>>(val), sizeof(vec4));
                }
            }
            return data;

        case shadergraph::ValueType::VEC3: // align to vec4 (4N)
            for (auto& [uni, val] : mat.uniforms)
            {
                if (val.index() == index_in_variant_v<vec3, UniformInstance>)
                {
                    auto sz = data.size();
                    data.resize(sz + sizeof(vec4));
                    memcpy_s(data.data() + sz, sizeof(vec3), &std::get<index_in_variant_v<vec3, UniformInstance>>(val), sizeof(vec3));
                }
            }
            return data;

        default:
            break;
        }

		return data;
	}
}
