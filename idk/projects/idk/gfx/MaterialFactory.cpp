#include "stdafx.h"
#include "MaterialFactory.h"
#include <util/ioutils.h>
#include <res/MetaBundle.h>
#include <gfx/MaterialInstance.h>
#include <gfx/ShaderProgram.h>
namespace idk
{
	unique_ptr<Material> MaterialFactory::GenerateDefaultResource()
	{
		return Create();
	}

	unique_ptr<Material> MaterialFactory::Create()
	{
		auto retval = std::make_unique<Material>();
		retval->_shader_program = Core::GetResourceManager().Create<ShaderProgram>();
		return retval;
	}

	ResourceBundle MaterialLoader::LoadFile(PathHandle p, const MetaBundle& m)
	{
		auto stream = p.Open(FS_PERMISSIONS::READ);
		const auto mat = [&]()
		{
			//auto meta = m.FetchMeta("material");
			//if (meta)
			//{
			//	auto retval = Core::GetResourceManager().LoaderEmplaceResource<Material>(meta->guid);;
			//	retval->SetMeta(*meta->GetMeta<Material>());;
			//	return retval;
			//}
			//else
			{
				auto retval = Core::GetResourceManager().LoaderEmplaceResource<Material>();
				retval->Name("material");
				return retval;
			}
		}();

		auto meta = m.FetchMeta<Material>();
		if (meta)


		mat->_default_instance = [&]()
		{
			auto meta = m.FetchMeta("mat_instance");
			if(meta)
				return Core::GetResourceManager().LoaderEmplaceResource<MaterialInstance>(meta->guid);
			else
			{
				auto retval = Core::GetResourceManager().LoaderEmplaceResource<MaterialInstance>();
				retval->Name("mat_instance");
				return retval;
			}
		}();

		if (stream)
		{
			parse_text(stringify(stream), *mat);
			const auto discardme = Core::GetResourceManager().LoaderCreateResource<ShaderProgram>(mat->_shader_program.guid);
			(discardme);
		}

		ResourceBundle bundle;
		bundle.Add(mat->_default_instance);
		bundle.Add(mat);
		bundle.Add(mat->_shader_program);
		return bundle;
	}
}