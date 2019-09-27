#include "stdafx.h"
#include "MaterialFactory.h"

namespace idk
{
	unique_ptr<Material> MaterialFactory::GenerateDefaultResource()
	{
		return Create();
	}

	unique_ptr<Material> MaterialFactory::Create()
	{
		auto retval = std::make_unique<Material>();
		// create associated shader program
		retval->_shader_program; 
		return retval;
	}
}