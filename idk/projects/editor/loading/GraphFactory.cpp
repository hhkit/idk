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
	unique_ptr<Material> GraphFactory::Create(PathHandle p)
	{
		auto stream = p.Open(FS_PERMISSIONS::READ);
		auto mat = std::make_unique<shadergraph::Graph>();
		if (stream)
		{
			parse_text(stringify(stream), *mat);
			(void) Core::GetResourceManager().Create<ShaderProgram>(mat->_shader_program.guid);
		}
		mat->Compile();
		return mat;
	}
}
