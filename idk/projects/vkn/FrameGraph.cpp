#include "pch.h"
#include "FrameGraph.h"
#include <vkn/VknTexture.h>
namespace idk::vkn
{


	//Process nodes and cull away unnecessary nodes.
	//Figure out dependencies and synchronization points
	//Insert new nodes to help transit concrete resources to their corresponding virtual resources
	//Generate dependency graph

	FrameGraphNode& FrameGraph::StoreNode(FrameGraphNode&& node)
	{
		node_lookup.emplace(node.id, nodes.size());
		auto& stored_node = nodes.emplace_back(std::move(node));
		tmp_graph.in_nodes.emplace(node.id, node.input_resources);
		for (auto& written_rsc : node.GetOutputSpan())
			tmp_graph.src_node.emplace(written_rsc.id,node.id);
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
			//TODO Get the dep_nodes from curr_node
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
		manager.CombineAllLifetimes(std::bind(&FrameGraphResourceManager::IsCompatible,&GetResourceManager(),std::placeholders::_1,std::placeholders::_2));
	}

	void FrameGraph::CreateConcreteResources(ResourceLifetimeManager& rlm, FrameGraphResourceManager& rm)
	{
		auto resource_templates = rlm.GetActualResources();
		for (auto& resource_template : resource_templates)
		{
			rm.Instantiate(resource_template.id, resource_template.base_rsc);
		}
		auto& aliases = rlm.Aliases();
		//Register the aliases.
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

	void TransitionResource(FrameGraph::Context_t context, TransitionInfo info)
	{
		//TODO: Actually transition
	}

	void FrameGraph::Execute(Context_t context)
	{
		auto& rsc_manager = GetResourceManager();
		for (auto index : execution_order)
		{
			auto& node = nodes[index];
			auto& rp = *render_passes[node.id];
			//TODO: Thread this
			{
				//Transition all the resources that are gonna be read (and are not input attachments)
				auto input_span = node.GetReadSpan();
				for (auto& input : input_span)
				{
					TransitionResource(context,rsc_manager.TransitionInfo(input));
				}
				rp.PreExecute(node, context);
				rp.Execute(context);
				rp.PostExecute(node, context);
			}
		}
	}

	vk::RenderPassCreateInfo FrameGraph::CreateRenderPassInfo(span<const std::optional<FrameGraphAttachmentInfo>> input_rscs, span<const std::optional<FrameGraphAttachmentInfo>> output_rscs, std::optional<FrameGraphAttachmentInfo> depth)
	{
		auto total_att = input_rscs.size() + output_rscs.size() + ((depth) ? 1 : 0);
		vector<vk::AttachmentDescription> attachments;
		vector<vk::AttachmentReference> attachment_input_refs(input_rscs.size());
		vector<vk::AttachmentReference> attachment_output_refs(output_rscs.size());
		vk::AttachmentReference depth_ref;
		auto& rsc_manager = GetResourceManager();
		auto& nodes_lookup = node_lookup;
		auto& nodes_buffer = nodes;
		attachments.reserve(total_att);
		auto process_attachment = [&nodes_buffer,&nodes_lookup,&rsc_manager, &attachments](auto& attachment_info, auto& attachment_ref)
		{
			auto find_input_layout = [](const FrameGraphNode& node, fgr_id id)
			{
				for (auto& o_input : node.input_attachments)
				{
					if (o_input && o_input->first ==id)
					{
						return o_input->second.layout;
					}
				}
				return vk::ImageLayout::eGeneral;
			};
			if (attachment_info)
			{
				VknTexture& tex = *rsc_manager.Get<VknTexture>(attachment_info->first);
				auto& [rsc_id, att_opt] = *attachment_info;
				attachment_ref.attachment = static_cast<uint32_t>(attachments.size());
				auto prev_id = rsc_manager.GetPrevious(rsc_id);
				vk::ImageLayout prev_layout = att_opt.layout;
				if (prev_id)
					prev_layout = find_input_layout(nodes_buffer[nodes_lookup.find(*prev_id)->second], *prev_id);
				attachments.emplace_back(vk::AttachmentDescription
					{
						//vk::AttachmentDescriptionFlagBits::eMayAlias
						{},
						(att_opt.format) ? tex.format : (*att_opt.format),
						vk::SampleCountFlagBits::e1,
						att_opt.load_op,
						att_opt.store_op,
						att_opt.stencil_load_op,
						att_opt.stencil_store_op,
						(att_opt.load_op != vk::AttachmentLoadOp::eLoad) ? vk::ImageLayout::eUndefined : prev_layout,
						att_opt.layout,
					});
			}
			else
			{
				attachment_ref.attachment = VK_ATTACHMENT_UNUSED;
			}
		};
		auto process_attachments = [&rsc_manager, &attachments, &process_attachment](auto& attachment_infos, auto& attachment_refs)
		{
			auto ref_itr = attachment_refs.begin();
			for (auto& input_rsc : attachment_infos)
			{

				process_attachment(input_rsc, *ref_itr);
				++ref_itr;
			}
		};

		{
			process_attachments(input_rscs, attachment_input_refs);
		}

		{
			process_attachments(input_rscs, attachment_output_refs);
		}
		if (depth)
		{
			process_attachment(depth, depth_ref);
		}
		vk::SubpassDescription subpass_desc{
			{},
			vk::PipelineBindPoint::eGraphics,
			static_cast<uint32_t>(attachment_input_refs.size()),
			std::data(attachment_input_refs),
			static_cast<uint32_t>(attachment_output_refs.size()),
			std::data(attachment_output_refs),
			nullptr,
			(depth) ? &depth_ref : nullptr,
			0,nullptr,
		};
		vk::SubpassDependency subpass_dep
		{
			VK_SUBPASS_EXTERNAL,0,
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests,  //TODO: Figure this out
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests, //TODO: Figure this out
			vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,//TODO: Figure this out
			vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentRead,	//TODO: Figure this out
			vk::DependencyFlagBits::eByRegion
		};
		vk::RenderPassCreateInfo rpci
		{
			vk::RenderPassCreateFlags{},
			static_cast<uint32_t>(attachments.size()),
			std::data(attachments),
			1,& subpass_desc,
			1,& subpass_dep
		};
		return rpci;
	}

	VknRenderPass FrameGraph::CreateRenderPass(span<const std::optional<FrameGraphAttachmentInfo>> input_rscs, span<const std::optional<FrameGraphAttachmentInfo>> output_rscs, std::optional<FrameGraphAttachmentInfo> depth)
	{
		vk::Device device;
		return device.createRenderPassUnique(CreateRenderPassInfo(input_rscs,output_rscs,depth));
	}
	void FrameGraph::CreateRenderPasses()
	{
		vector < std::tuple<FrameGraphNode*,BaseRenderPass*, vk::RenderPassCreateInfo>> create_info{};
		for (auto node_index : execution_order)
		{
			auto& node = nodes[node_index];
			auto itr = render_passes.find(node.id);
			auto& rp = *itr->second;
			auto input_attachments = span{ node.input_attachments };
			auto output_attachments = span{ node.output_attachments };
			rp.render_pass = CreateRenderPass(input_attachments, output_attachments,node.depth_stencil);
			rp.frame_buffer = CreateFrameBuffer(rp.render_pass, input_attachments, output_attachments, node.depth_stencil);
		}
	}

	FrameGraphResourceManager& FrameGraph::GetResourceManager()
	{
		return graph_builder.rsc_manager;
	}

	const FrameGraphResourceManager& FrameGraph::GetResourceManager() const
	{
		return graph_builder.rsc_manager;
	}

}