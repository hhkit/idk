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
	vector<RscHandle<Texture>> MaterialInstance::GetImageBlock(const string& name) const
	{
		//TODO actually get a block
		vector<RscHandle<Texture>> result;
		auto uni = GetUniform(name);
		if (uni)
		{
			//Todo, replace name with the individual names in the block
			result.emplace_back(std::get<RscHandle<Texture>>(*uni));
		}
		else
		{
			int i = 0;
			bool should_stop = false;
			while (!should_stop)
			{
				auto opt= GetUniform(name + "[" + std::to_string(i) + "]");
				if (opt)
				{
					result.emplace_back(std::get<RscHandle<Texture>>(*opt));
				}
				should_stop = !opt;
				++i;
			}
		}
		return result;
	}
	std::optional<UniformInstanceValue> MaterialInstance::GetUniform(const string& name) const
	{
		std::optional<UniformInstanceValue> result;
		auto itr = uniforms.find(name);
		if (itr == uniforms.end())
		{
			auto& mat = *material;
			auto itr2 = mat.uniforms.find(name);
			if (itr2 != mat.uniforms.end())
				result = itr2->second.value;
		}
		else
		{
			result = itr->second;
		}
		return result;
	}
	bool MaterialInstance::IsUniformBlock(string_view name) const
	{
		return name.starts_with("_UB");
	}
	string MaterialInstance::GetUniformBlock(const string& name) const
	{
        // https://www.khronos.org/registry/OpenGL/specs/gl/glspec45.core.pdf pg 138

		string data;
        if (!IsUniformBlock(name))
            return data;

		auto& mat = *material;
        const shadergraph::ValueType type = std::stoi(name.data() + sizeof("_UB") - 1);

        switch (type)
        {
        case shadergraph::ValueType::FLOAT: // align contiguous (N)
            for (const auto& pair : mat.uniforms)
            {
                const auto& val = pair.second.value;
                if (val.index() == index_in_variant_v<float, UniformInstanceValue>)
                {
                    auto sz = data.size();
                    data.resize(sz + sizeof(float));
                    memcpy_s(data.data() + sz, sizeof(float), &std::get<index_in_variant_v<float, UniformInstanceValue>>(val), sizeof(float));
                }
            }
            for (const auto& uni : mat.hidden_uniforms)
            {
                const auto& val = uni.value;
                if (val.index() == index_in_variant_v<float, UniformInstanceValue>)
                {
                    auto sz = data.size();
                    data.resize(sz + sizeof(float));
                    memcpy_s(data.data() + sz, sizeof(float), &std::get<index_in_variant_v<float, UniformInstanceValue>>(val), sizeof(float));
                }
            }
            return data;

        case shadergraph::ValueType::VEC2: // align contiguous (2N)
            for (const auto& pair : mat.uniforms)
            {
                const auto& val = pair.second.value;
                if (val.index() == index_in_variant_v<vec2, UniformInstanceValue>)
                {
                    auto sz = data.size();
                    data.resize(sz + sizeof(vec2));
                    memcpy_s(data.data() + sz, sizeof(vec2), &std::get<index_in_variant_v<vec2, UniformInstanceValue>>(val), sizeof(vec2));
                }
            }
            for (const auto& uni : mat.hidden_uniforms)
            {
                const auto& val = uni.value;
                if (val.index() == index_in_variant_v<vec2, UniformInstanceValue>)
                {
                    auto sz = data.size();
                    data.resize(sz + sizeof(vec2));
                    memcpy_s(data.data() + sz, sizeof(vec2), &std::get<index_in_variant_v<vec2, UniformInstanceValue>>(val), sizeof(vec2));
                }
            }
            return data;

        case shadergraph::ValueType::VEC4: // align contiguous (4N)
            for (const auto& pair : mat.uniforms)
            {
                const auto& val = pair.second.value;
                if (val.index() == index_in_variant_v<vec4, UniformInstanceValue>)
                {
                    auto sz = data.size();
                    data.resize(sz + sizeof(vec4));
                    memcpy_s(data.data() + sz, sizeof(vec4), &std::get<index_in_variant_v<vec4, UniformInstanceValue>>(val), sizeof(vec4));
                }
            }
            for (const auto& uni : mat.hidden_uniforms)
            {
                const auto& val = uni.value;
                if (val.index() == index_in_variant_v<vec4, UniformInstanceValue>)
                {
                    auto sz = data.size();
                    data.resize(sz + sizeof(vec4));
                    memcpy_s(data.data() + sz, sizeof(vec4), &std::get<index_in_variant_v<vec4, UniformInstanceValue>>(val), sizeof(vec4));
                }
            }
            return data;

        case shadergraph::ValueType::VEC3: // align to vec4 (4N)
            for (const auto& pair : mat.uniforms)
            {
                const auto& val = pair.second.value;
                if (val.index() == index_in_variant_v<vec3, UniformInstanceValue>)
                {
                    auto sz = data.size();
                    data.resize(sz + sizeof(vec4));
                    memcpy_s(data.data() + sz, sizeof(vec3), &std::get<index_in_variant_v<vec3, UniformInstanceValue>>(val), sizeof(vec3));
                }
            }
            for (const auto& uni : mat.hidden_uniforms)
            {
                const auto& val = uni.value;
                if (val.index() == index_in_variant_v<vec3, UniformInstanceValue>)
                {
                    auto sz = data.size();
                    data.resize(sz + sizeof(vec4));
                    memcpy_s(data.data() + sz, sizeof(vec3), &std::get<index_in_variant_v<vec3, UniformInstanceValue>>(val), sizeof(vec3));
                }
            }
            return data;

        default:
            break;
        }

		return data;
	}
}
