#include "stdafx.h"
#include "MaterialInstance.h"
#include <gfx/ShaderGraph_data.h>
#include <meta/variant.h>
#include <serialize/text.inl>
#include <res/ResourceHandle.inl>

namespace idk
{
	bool IsImageBlock(const UniformInstanceValue& uni)
	{
		return index_in_variant_v<RscHandle<Texture>, UniformInstanceValue> == uni.index();
	}
	bool MaterialInstance::IsImageBlock(const string& name) const
	{
		auto uni = GetUniform(name);
		return uni && idk::IsImageBlock(*uni);
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
				auto opt= GetUniform(name + "[" + serialize_text(i) + "]");
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
    void MaterialInstance::SetUniform(const string& name, UniformInstanceValue value)
    {
        if (!material)
            return;
        auto& mat = *material;
        auto iter = mat.uniforms.find(name);
        if (iter != mat.uniforms.end())
        {
            if (iter->second.value.index() == value.index())
                uniforms[name] = value;
        }
    }
	bool IsUniformBlock(string_view name)
	{
		auto str = name.substr(0, sizeof("_UB") - 1);
		return  (str[0]== '_') & (str[1]=='U') & (str[2] == 'B');
	}
	bool MaterialInstance::IsUniformBlock(string_view name) const
	{
		return idk::IsUniformBlock(name);
	}

	template<typename T, typename UniFunc>
	string GetUniformBlock(shadergraph::ValueType type, const T& uniforms, UniFunc&& func)
	{
		string data;

		switch (type)
		{
		case shadergraph::ValueType::FLOAT: // align contiguous (N)
			for (const auto& pair : uniforms)
			{
				const auto& val = func(pair.second);
				if (val.index() == index_in_variant_v<float, UniformInstanceValue>)
				{
					auto sz = data.size();
					data.resize(sz + sizeof(float));
					memcpy_s(data.data() + sz, sizeof(float), &std::get<index_in_variant_v<float, UniformInstanceValue>>(val), sizeof(float));
				}
			}
			return data;

		case shadergraph::ValueType::VEC2: // align contiguous (2N)
			for (const auto& pair : uniforms)
			{
				const auto& val = func(pair.second);
				if (val.index() == index_in_variant_v<vec2, UniformInstanceValue>)
				{
					auto sz = data.size();
					data.resize(sz + sizeof(vec2));
					memcpy_s(data.data() + sz, sizeof(vec2), &std::get<index_in_variant_v<vec2, UniformInstanceValue>>(val), sizeof(vec2));
				}
			}
			return data;

		case shadergraph::ValueType::VEC4: // align contiguous (4N)
			for (const auto& pair : uniforms)
			{
				const auto& val = func(pair.second);
				if (val.index() == index_in_variant_v<vec4, UniformInstanceValue>)
				{
					auto sz = data.size();
					data.resize(sz + sizeof(vec4));
					memcpy_s(data.data() + sz, sizeof(vec4), &std::get<index_in_variant_v<vec4, UniformInstanceValue>>(val), sizeof(vec4));
				}
			}
			return data;

		case shadergraph::ValueType::VEC3: // align to vec4 (4N)
			for (const auto& pair : uniforms)
			{
				const auto& val = func(pair.second);
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
	string GetUniformBlock(shadergraph::ValueType type,const std::map<string, UniformInstanceValue>& uniforms)
	{
		return GetUniformBlock(type, uniforms, [](auto& v) {return v; });
	}
	string GetUniformBlock(shadergraph::ValueType type, const std::map<string, UniformInstance>& uniforms)
	{
		return GetUniformBlock(type, uniforms, [](auto& v) {return v.value; });
	}
	shadergraph::ValueType GetBlockType(string_view name)
	{
		return std::stoi(name.data() + sizeof("_UB") - 1);
	}
	string MaterialInstance::GetUniformBlock(const string& name) const
	{
        // https://www.khronos.org/registry/OpenGL/specs/gl/glspec45.core.pdf pg 138

		string data;
		if (IsUniformBlock(name))
		{
			auto& mat = *material;

			const shadergraph::ValueType type = GetBlockType(name);
			data =idk::GetUniformBlock(type, mat.uniforms);
		}
		return data;
	}
	bool IsUniformSubBlock(string_view name)
	{
		return name.substr(0, sizeof("_ub") - 1) == "_ub";
	}
	MaterialInstance::temp_store MaterialInstance::get_cache() const
	{
		temp_store store;

		if (material)
		{
			auto& mat = *material;
			for (auto& [name, uni] : mat.uniforms)
			{
				if (IsUniformSubBlock(uni.name))
				{
					char test[] = "_UBN";
					const size_t N_index = sizeof("_UB") - 1;
					test[N_index] = uni.name[N_index];
					store.uniforms[test];
				}
				store.uniforms[uni.name] = uni.value;
			}
			for (auto& [name, uni] : mat.hidden_uniforms)
			{
				if (IsUniformSubBlock(name))
				{
					char test[] = "_UBN";
					const size_t N_index = sizeof("_UB") - 1;
					test[N_index] = name[N_index];
					store.uniforms[test];
				}
				store.uniforms[name] = uni;
			}
			//No material, no uniforms
			
			for (auto& [var_name, uni] : uniforms)
			{
				auto itr = mat.uniforms.find(var_name);
				if (itr != mat.uniforms.end())
				{
					auto& name = itr->second.name;
					if (IsUniformSubBlock(name))
					{
						char test[] = "_UBN";
						const size_t N_index = sizeof("_UB") - 1;
						test[N_index] = name[N_index];
						store.uniforms[test];
					}
					store.uniforms[name] = uni;
				}
			}
		}
		return store;
	}
	bool is_elem_0(string_view str)
	{
		auto pos = str.find_last_of('[');
		return pos!= str.npos && str.substr(pos) == "[0]";
	}
	bool is_not_array(string_view str)
	{
		return str.find_last_of(']',str.find_last_of('['))<str.size();
	}
	bool MaterialInstance::temp_store::IsImageBlock(uniform_store_t::iterator itr) const
	{
		return idk::IsImageBlock(itr->second) && is_elem_0(itr->first);//Is first index
	}
	bool MaterialInstance::temp_store::IsUniformBlock(uniform_store_t::iterator itr) const
	{
		return idk::IsUniformBlock(itr->first);
	}
	vector<RscHandle<Texture>> MaterialInstance::temp_store::GetImageBlock(uniform_store_t::iterator itr) const
	{
		vector<RscHandle<Texture>> result;
		if (is_not_array(itr->first))
		{
			//Todo, replace name with the individual names in the block
			result.emplace_back(std::get<RscHandle<Texture>>(itr->second));

		}else if(is_elem_0(itr->first))
		{
			auto name = itr->first.substr(0, itr->first.find_last_of('['));
			int i = 0;
			bool should_stop = false;
			while (!should_stop)
			{
				auto u_itr = uniforms.find(name + "[" + serialize_text(i) + "]");
				should_stop = u_itr == uniforms.end();
				if (!should_stop)
				{
					result.emplace_back(std::get<RscHandle<Texture>>(u_itr->second));
				}
				++i;
			}
		}
		return result;
	}
	string MaterialInstance::temp_store::GetUniformBlock(uniform_store_t::iterator itr) const
	{
		return idk::GetUniformBlock(GetBlockType(itr->first),uniforms);
	}
}
