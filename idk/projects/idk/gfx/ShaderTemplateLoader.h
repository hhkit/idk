#pragma once
#include <res/FileLoader.h>

namespace idk
{
	class ShaderTemplateLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle path, const MetaBundle&) override;
	};
}