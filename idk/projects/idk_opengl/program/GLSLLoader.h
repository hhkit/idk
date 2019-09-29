#pragma once
#include <res/FileLoader.h>
#include <res/ResourceFactory.h>
#include <gfx/ShaderProgram.h>

namespace idk::ogl
{
	class GLSLLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle filepath) override;
		ResourceBundle LoadFile(PathHandle filepath, const MetaBundle&) override;
	};
}