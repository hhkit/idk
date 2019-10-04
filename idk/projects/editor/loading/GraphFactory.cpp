#include "pch.h"
#include "GraphFactory.h"
#include <res/MetaBundle.h>
#include <gfx/MaterialInstance.h>
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
		const auto mat = Core::GetResourceManager().LoaderEmplaceResource<shadergraph::Graph>();
		if (stream)
		{
			parse_text(stringify(stream), *mat);
			const auto discardme = Core::GetResourceManager().LoaderCreateResource<ShaderProgram>(mat->_shader_program.guid);
			(discardme);
		}
		mat->Compile();
		return mat;
	}

	ResourceBundle GraphLoader::LoadFile(PathHandle p, const MetaBundle& m)
	{
        auto stream = p.Open(FS_PERMISSIONS::READ);

        auto meta = m.FetchMeta<Material>();
        const auto mat = meta 
            ? Core::GetResourceManager().LoaderEmplaceResource<shadergraph::Graph>(meta->guid)
            : Core::GetResourceManager().LoaderEmplaceResource<shadergraph::Graph>();

        //const auto mat = Core::GetResourceManager().LoaderEmplaceResource<shadergraph::Graph>(m.metadatas[0].guid);

        if (stream)
        {
            parse_text(stringify(stream), *mat);
            const auto discardme = Core::GetResourceManager().LoaderCreateResource<ShaderProgram>(mat->_shader_program.guid);
            (discardme);
        }

        if (meta)
            mat->SetMeta(*meta->GetMeta<Material>());

        mat->_default_instance = Core::GetResourceManager().LoaderEmplaceResource<MaterialInstance>(mat.guid);
        mat->_default_instance->material = mat;

        mat->Compile();

        ResourceBundle b;
        b.Add(mat);
        b.Add(mat->_default_instance);
        return b;
	}
}
