#include "stdafx.h"
#include "MaterialFactory.h"
#include <util/ioutils.h>
#include <res/MetaBundle.h>
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

	ResourceBundle MaterialLoader::LoadFile(PathHandle p)
	{
		auto stream = p.Open(FS_PERMISSIONS::READ);
		auto mat = Core::GetResourceManager().LoaderEmplaceResource<Material>();
		if (stream)
		{
			parse_text(stringify(stream), *mat);
			auto discardme = Core::GetResourceManager().LoaderCreateResource<ShaderProgram>(mat->_shader_program.guid);
			(discardme);
		}
		return mat;
	}
	ResourceBundle MaterialLoader::LoadFile(PathHandle p, const MetaBundle& m)
	{
		auto stream = p.Open(FS_PERMISSIONS::READ);
		auto mat = Core::GetResourceManager().LoaderEmplaceResource<Material>(m.metadatas[0].guid);

		if (stream)
		{
			parse_text(stringify(stream), *mat);
			auto discardme = Core::GetResourceManager().LoaderCreateResource<ShaderProgram>(mat->_shader_program.guid);
			(discardme);
		}

		auto meta = m.FetchMeta<Material>();
		if (meta)
			mat->SetMeta(*meta->GetMeta<Material>());

		return mat;
	}
}