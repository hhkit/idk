#include "pch.h"
#include "TextureManager.h"

namespace idk::ogl
{
	TextureManager::TextureManager()
		: max_textures{}
	{
	}
	TextureManager::TextureUnit TextureManager::Bind(RscHandle<ogl::OpenGLTexture> handle)
	{
		if (texture_units.empty())
		{
			texture_units.emplace(GL_TEXTURE0, handle);
			glActiveTexture(GL_TEXTURE0);
			handle->Bind();
			return GL_TEXTURE0;
		}

		auto itr = texture_units.find_second(handle);
		if (itr != texture_units.end())
			return itr->first;

		

		return TextureUnit();
	}

	void TextureManager::UnbindAll()
	{
		for (auto& [unit, resource] : texture_units)
		{
			glActiveTexture(GL_TEXTURE0 + unit);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}
