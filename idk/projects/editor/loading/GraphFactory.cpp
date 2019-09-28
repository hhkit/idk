#include "pch.h"
#include "GraphFactory.h"
#include <res/MetaBundle.h>
#include <gfx/ShaderProgram.h>
#include <util/ioutils.h>

namespace idk
{
	unique_ptr<Material> GraphFactory::GenerateDefaultResource()
	{
		return Create();
	}
	unique_ptr<Material> GraphFactory::Create()
	{
		auto retval = std::make_unique<shadergraph::Graph>();
		retval->_shader_program = Core::GetResourceManager().Create<ShaderProgram>();
		return retval;
	}
	ResourceBundle GraphLoader::LoadFile(PathHandle p)
	{
		auto stream = p.Open(FS_PERMISSIONS::READ);
		auto mat = Core::GetResourceManager().LoaderEmplaceResource<shadergraph::Graph>();
		if (stream)
		{
			parse_text(stringify(stream), *mat);
			auto discardme = Core::GetResourceManager().LoaderCreateResource<ShaderProgram>(mat->_shader_program.guid);
			(discardme);
		}
		mat->Compile();
		return mat;
	}

	ResourceBundle GraphLoader::LoadFile(PathHandle p, const MetaBundle& m)
	{
		auto stream = p.Open(FS_PERMISSIONS::READ);
		auto mat = Core::GetResourceManager().LoaderEmplaceResource<shadergraph::Graph>(m.metadatas[0].guid);

		if (stream)
		{
			parse_text(stringify(stream), *mat);
			auto discardme = Core::GetResourceManager().LoaderCreateResource<ShaderProgram>(mat->_shader_program.guid);
			(discardme);
		}

		auto meta = m.FetchMeta<Material>();
		if (meta)
			mat->SetMeta(*meta->GetMeta<Material>());

		mat->Compile();
		return mat;
	}
}
