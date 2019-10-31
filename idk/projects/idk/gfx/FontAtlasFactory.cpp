#pragma once
#include "stdafx.h"
#include "MeshFactory.h"
#include <iostream>
#include "FontAtlasFactory.h"

namespace idk
{
	/*unique_ptr<FontAtlas> FontAtlasFactory::GenerateDefaultResource()
	{
	
		return unique_ptr<FontAtlas>();
	}
	unique_ptr<FontAtlas> FontAtlasFactory::Create()
	{
		return unique_ptr<FontAtlas>();
	}*/
	void FontAtlasFactory::Init()
	{
		GenerateDefaultFontAtlas();

		auto i = 0;
		for (auto& elem : FontAtlas::defaults)
		{
			if (!elem)
				std::cout << "Default font " << FontDefault{ i }.to_string() << " not created\n";
			elem->Name(string{ "Default" } +string{ FontDefault{ i }.to_string() });
			++i;
		}
	}
}
