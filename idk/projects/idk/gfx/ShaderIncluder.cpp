#include "stdafx.h"
#include "ShaderIncluder.h"
#include <core/Core.h>
#include <gfx/ShaderSnippet.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <memory/ArenaAllocator.inl>
#include <ds/result.inl>

namespace idk
{

	struct IncludeEntry
	{
		size_t start, len;
		string_view include_name;
	};
	IncludeEntry FindNextIncludes(string_view& str_view, size_t& offset) noexcept;

	string_view GetIncluded(string_view mounted_dir);

	IncludeEntry FindNextIncludes(string_view& str_view, size_t& offset) noexcept
	{
		IncludeEntry result;
		string_view keyword{ "\nimport " };
		result.start = str_view.find(keyword);
		result.len = 0; //in case npos
		if (result.start != string_view::npos)
		{
			auto include_start = str_view.find_first_not_of(string_view{ " \t" }, result.start + (keyword.size() - 1));
			auto include_end = str_view.find_first_of('\n', result.start + keyword.size());
			result.include_name = string_view{ str_view.data() + include_start,include_end - include_start };
			result.len = include_end - result.start;
			//Account for the relative offset
			result.start += offset;
			//Update the running variables
			offset += include_end;
			str_view = string_view{ str_view.data() + include_end,str_view.size() - include_end };
		}
		return result;
	}
	string_view GetIncluded(string_view mounted_dir)
	{
		//auto file = Core::GetSystem<FileSystem>().Open(mounted_dir, FS_PERMISSIONS::READ);
		auto st = Core::GetResourceManager().Load<ShaderSnippet>(mounted_dir,false);
		string_view result = "// not_found";
		if (st)
		{
			//Get the string from resource manager
			result = (*st)->snippet;
			//Store it into result.
		}
		return result;
	}

	template<typename T>
	using Alloc = ArenaAllocator<T>;

	using included_t =std::unordered_set<string_view, std::hash<string_view>, std::equal_to<string_view>, Alloc<string_view>>;
//#pragma optimize("",off)
	void ProcessIncludesImpl(string_view glsl_code, string& final_result, included_t& included)
	{
		unsigned char pool[1 << 16];
		Alloc<char> alloc{ pool };

		std::basic_string<char, std::char_traits<char>, Alloc<char>> result{ alloc };
		vector<IncludeEntry> includes{};
		string_view find_input{ glsl_code };
		size_t offset = 0;
		while (find_input.size())
		{
			auto next = FindNextIncludes(find_input, offset);
			if (next.len == 0)
				break;
			includes.emplace_back(next);
		}
		const char* prev_end = glsl_code.data();
		for (auto& include : includes)
		{
			result.append(prev_end, glsl_code.data() + include.start);
			//If this is not a repeated include
			if (included.emplace(include.include_name).second)
			{
				//Debug annotation
				result.append("\n/* ");
				result.append(include.include_name);
				result.append("begin */\n");
				//End Debug annotation
				auto str_view = GetIncluded(include.include_name);
				result.append(str_view);
				result.append("\n/* ");
				result.append(include.include_name);
				result.append("end */\n");
			}
			else
			{
				result.append("\n/* ");
				result.append(include.include_name);
				result.append(" has already been included before. */\n");
			}
			prev_end = glsl_code.data() + include.start + include.len;//forward seek pos
		}

		//clean up the remaining bit
		result.append(prev_end, glsl_code.size()-(prev_end - glsl_code.data()));
		final_result = string{ result.data(),result.size() }; //Copy, the allocated memory must be destroyed with the arean allocator.
	}

	string ProcessIncludes(string_view glsl_code)
	{
		unsigned char pool[1 << 16];
		Alloc<char> alloc{ pool };

		string result{};
		string old_result{ glsl_code };
		included_t included{ alloc }; //Out  here to prevent recursive expansion
		while (true)
		{
			ProcessIncludesImpl(old_result, result,included); //While there's something left to expand, expand.
			if (old_result.size() == result.size())
				break;
			std::swap(old_result, result);
		}
		return result;
	}


}