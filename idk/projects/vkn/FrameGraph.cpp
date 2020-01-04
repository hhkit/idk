#include "pch.h"
#include "FrameGraph.h"
namespace idk::vkn
{


	//Process nodes and cull away unnecessary nodes.
	//Figure out dependencies and synchronization points
	//Insert new nodes to help transit concrete resources to their corresponding virtual resources
	//Generate dependency graph

	FrameGraphNode& FrameGraph::StoreNode(FrameGraphNode&& node)
	{
		auto& stored_node = nodes.emplace_back(std::move(node));
		tmp_graph.in_nodes.emplace(node.id, node.input_resources);
		for (auto& written_rsc : node.GetOutputSpan())
			tmp_graph.src_node.emplace(written_rsc.id);
		return stored_node;
	}

	void FrameGraph::ComputeLifetimes(ResourceLifetimeManager& manager)
	{
		auto& exec_order = execution_order;
		auto& graph_nodes = nodes;

		hash_table<fg_id, size_t> fat_order;
		//Because of the sorting of the execution order, 
		//we can count on having ordered all dependencies before encountering the current node.
		size_t max_order = 0;
		for (auto index : exec_order)
		{
			auto& curr_node = graph_nodes[index];
			size_t order = 0;
			span<const FrameGraphNode> dep_nodes;
			for (auto& dep_node : dep_nodes)
			{
				order = std::max(fat_order[dep_node.id], order);
			}
			order = order + 1;
			fat_order[index] = order;
			max_order = std::max(order, max_order);
		}

		for (auto index : exec_order)
		{
			auto& curr_node = graph_nodes[index];
			for (auto& input_rsc : curr_node.GetInputSpan())
			{
				manager.ExtendLifetime(input_rsc.id, fat_order[curr_node.id]);
			}
		}
		//TODO: Create a function to check compatiblity between resources
		manager.CombineAllLifetimes();
	}

	void FrameGraph::CreateConcreteResources(ResourceLifetimeManager& rlm, FrameGraphResourceManager& rm)
	{
		auto resource_templates = rlm.GetActualResources();
		for (auto& resource_template : resource_templates)
		{
			rm.Instantiate(resource_template.id, resource_template.base_rsc);
		}
		auto& aliases = rlm.Aliases();
		for (auto& [r_id, ar_id] : aliases)
		{
			rm.Alias(r_id, ar_id);
		}
	}

	void FrameGraph::Compile()
	{
		auto graph = ConvertTempGraph(std::move(tmp_graph));
		graph_theory::IntermediateGraph dependency_graph{ nodes.size() };
		hash_table<fg_id, size_t> id_to_indices;
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			id_to_indices[nodes[i].id] = i;
		}
		{
			vector<size_t> index_buffer;
			index_buffer.reserve(nodes.size());
			for (auto& [id, index] : id_to_indices)
			{
				auto oid_adj_buffer = graph.get_input_nodes(id);
				auto id_adj_buffer = *oid_adj_buffer;
				auto& lookup_table = id_to_indices;
				index_buffer.resize(id_adj_buffer.size());
				std::transform(id_adj_buffer.begin(), id_adj_buffer.end(), index_buffer.begin(), [&lookup_table](fg_id id) {return lookup_table.find(id)->second; });
				dependency_graph.SetAdjacentNodes(index, index_buffer.begin(), index_buffer.end());
			}
			auto [sorted_order, success] = graph_theory::KahnsAlgorithm(dependency_graph);
			IDK_ASSERT_MSG(success, "Cyclic dependency detected.");
			//Maybe use copy_if to filter out useless nodes.
			execution_order = std::move(sorted_order);
		}
		ComputeLifetimes(rsc_lifetime_mgr);
	}

	void FrameGraph::AllocateResources()
	{
		CreateConcreteResources(rsc_lifetime_mgr, this->graph_builder.rsc_manager);
	}
	void FrameGraph::BuildRenderPasses()
	{
		CreateRenderPasses();
	}
	void FrameGraph::CreateRenderPasses()
	{
		for (auto node_index : execution_order)
		{
			auto& node = nodes[node_index];
			auto itr = render_passes.find(node.id);
			auto& rp = *itr->second;
			auto input_attachments = span{ node.input_attachments };
			auto output_attachments = span{ node.output_attachments };
			rp.render_pass = CreateRenderPass(input_attachments, output_attachments);
			rp.frame_buffer = CreateFrameBuffer(rp.render_pass, input_attachments, output_attachments);
		}
	}

}