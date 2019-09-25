#pragma once
#include <res/FileLoader.h>
#include <res/ResourceFactory.h>
#include <gfx/ShaderProgram.h>

namespace idk::ogl
{
	class ShaderProgramFactory
		: public ResourceFactory<ShaderProgram>
	{
	public:
		unique_ptr<ShaderProgram> GenerateDefaultResource() override;
		unique_ptr<ShaderProgram> Create() override;
	};

	class GLSLLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle filepath) override;
		ResourceBundle LoadFile(PathHandle filepath, const MetaBundle&) override;
	};
}