#pragma once
#include <res/ResourceFactory.h>
#include <gfx/ShaderProgram.h>

namespace idk::ogl
{
	class ShaderProgramFactory
		: public ResourceFactory<ShaderProgram>
	{
	public:
		unique_ptr<ShaderProgram> GenerateDefaultResource() override;
		unique_ptr<ShaderProgram> Create(FileHandle filepath) override;
	};
}