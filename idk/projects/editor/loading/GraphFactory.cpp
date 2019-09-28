#include "pch.h"
#include "GraphFactory.h"
#include <res/MetaBundle.h>
#include <gfx/ShaderProgram.h>
#include <gfx/ShaderGraph_helpers.h>
#include <util/ioutils.h>

namespace idk
{
	unique_ptr<Material> GraphFactory::GenerateDefaultResource()
	{
		return Create();
	}
	unique_ptr<Material> GraphFactory::Create()
	{
		auto graph = std::make_unique<shadergraph::Graph>();

        shadergraph::Node node;
        node.name = "master\\PBR";
        node.guid = Guid::Make();
        node.position = vec2{ 500.0f, 200.0f };

        auto sig = shadergraph::NodeTemplate::GetTable().at(node.name).signatures[0];
        for (auto in : sig.ins)
            node.input_slots.push_back({ in, shadergraph::helpers::default_value(in) });
        for (auto out : sig.outs)
            node.output_slots.push_back({ out });

        graph->master_node = node.guid;
        graph->nodes.emplace(node.guid, std::move(node));
		graph->_shader_program = Core::GetResourceManager().Create<ShaderProgram>();
		return graph;
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
