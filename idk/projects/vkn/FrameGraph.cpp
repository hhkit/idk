#include "pch.h"
#include "FrameGraph.h"
#include <vkn/VknTexture.h>
#include <vkn/VulkanView.h>
#include <vkn/VknTextureView.h>
#include <res/ResourceHandle.inl>

#include <vkn/Stopwatch.h>
#include <vkn/time_log.h>

#include <vkn/RenderBundle.h>

#include <vkn/DebugUtil.h>

#include <parallel/ThreadPool.h>
#include <numeric>

namespace idk::vkn
{
	void DoNothing();
	bool frame_graph_debug = false;
	//Process nodes and cull away unnecessary nodes.
	//Figure out dependencies and synchronization points
	//Insert new nodes to help transit concrete resources to their corresponding virtual resources
	//Generate dependency graph

	void FrameGraph::MarkRegion(string region_name)
	{
		graph_builder.MarkNodeRegion(std::move(region_name));
	}

	FrameGraphNode& FrameGraph::StoreNode(FrameGraphNode&& node)
	{
		node_lookup.emplace(node.id, nodes.size());
		auto& stored_node = nodes.emplace_back(std::move(node));
		tmp_graph.in_nodes.emplace(node.id, node.input_resources);
		for (auto& written_rsc : node.GetOutputSpan())
			tmp_graph.src_node.emplace(written_rsc.id, node.id);
		return stored_node;
	}

	void FrameGraph::ComputeLifetimes(const ActualGraph& ag, ResourceLifetimeManager& manager)
	{
		auto& exec_order = execution_order;
		auto& graph_nodes = nodes;

		hash_table<fg_id, size_t> fat_order;
		//Because of the sorting of the execution order, 
		//we can count on having ordered all dependencies before encountering the current node.
		size_t max_order = 0;
		auto& rsc_manager = GetResourceManager();
		auto original_id = [&rsc_manager](fgr_id id) {return rsc_manager.GetOriginal(id); };
		size_t serial_order=0;
		for (auto index : exec_order)
		{
			auto& curr_node = graph_nodes[index];
			size_t order = 0;
			//TODO Get the dep_nodes from curr_node
			auto dep_rscs = curr_node.GetInputSpan();
			for (auto& dep_rsc : dep_rscs)
			{
				auto src_itr = ag.src_node.find(dep_rsc.id);
				if (src_itr != ag.src_node.end())
				{
					auto dep_node = src_itr->second;
					order = std::max(fat_order[dep_node], order);
				}
			}
			order = order + 1;
			fat_order[curr_node.id] = serial_order++; //Change back to order to revert to fat ordering.
			max_order = std::max(order, max_order);
		}

		hash_table<fgr_id, size_t> ordering;
		for (auto index : exec_order)
		{
			auto& curr_node = graph_nodes[index];
			for (auto& input_rsc : curr_node.GetInputSpan())
			{
				auto rsc_id = input_rsc.id;
				auto order = fat_order[curr_node.id];
				manager.ExtendLifetime(rsc_id, order);
				ordering.emplace(rsc_id, order);
				ordering[rsc_id] = std::min(ordering[rsc_id],order);
			}
			for (auto& output_rsc : curr_node.GetOutputSpan())
			{
				auto rsc_id = output_rsc.id;
				auto order = fat_order[curr_node.id];
				manager.ExtendLifetime(rsc_id, order);
				ordering.emplace(rsc_id, order);
				ordering[rsc_id] = std::min(ordering[rsc_id], order);
			}
		}
		vector<std::pair<string, ResourceLifetime>> lifetimes;
		for (auto& [id, index] : manager.map)
		{
			auto odesc = rsc_manager.GetResourceDescription(id);
			auto lifetime = manager.resource_lifetimes.at(index);
			lifetimes.emplace_back(odesc->name, lifetime);
		}


		manager.CollapseLifetimes(original_id,ordering);
		manager.DebugCollapsed(rsc_manager);
		auto get_size = [&rsc_manager](fgr_id rsc_id) ->uvec2
		{
			return rsc_manager.GetResourceDescription(rsc_id)->size;
		};
		manager.CombineAllLifetimes(std::bind(&FrameGraphResourceManager::IsCompatible, &GetResourceManager(), std::placeholders::_1, std::placeholders::_2),get_size);
		
		
		//auto& rsc_manager = GetResourceManager();
		manager.DebugArrange(rsc_manager);
	}

	void FrameGraph::CreateConcreteResources(ResourceLifetimeManager& rlm, FrameGraphResourceManager& rm)
	{
		auto resource_templates = rlm.GetActualResources();
		
		for (auto& resource_template : resource_templates)
		{
			auto desc_copy = *rm.GetResourceDescription(resource_template.base_rsc);
			desc_copy.size = resource_template.size;
			auto new_base = rm.CreateTexture(desc_copy);
			rm.Instantiate(resource_template.index, new_base.id);
		}
		rm.FinishInstantiation();
		auto& aliases = rlm.Aliases();
		//Register the aliases.
		for (auto& [r_id, ar_id] : aliases)
		{
			rm.Alias(r_id, ar_id);
		}
	}
	const ResourceLifetimeManager& FrameGraph::GetLifetimeManager() const
	{
		return rsc_lifetime_mgr;
	}
	void FrameGraph::Reset()
	{
		for (auto& p_ubo_managers : _ubo_managers)
		{
			if (p_ubo_managers)
			{
				p_ubo_managers->Clear();
			}
		}
		nodes.clear();
		render_passes.clear();
		execution_order.clear();
		graph_builder.Reset();
		rsc_lifetime_mgr.ClearLifetimes();
		GetResourceManager().Reset();
	}

	namespace dbg
	{
		namespace fg
		{

			struct NodeDependencies
			{
				string node_name;
				vector<string> resource_names;
			};

			struct DependencyDebugger
			{
				vector<NodeDependencies> nodes;

				vector<std::pair<string, vector<string>>> named_dependencies;

				template<typename FgConvFunc, typename FgrConvFunc>
				DependencyDebugger(const ActualGraph& graph, FgConvFunc&& fgid_name, FgrConvFunc&& fgrid_name)
				{
					if(frame_graph_debug)
						for (auto [node_id, rsc_span] : graph.in_rsc_nodes)
						{
							auto& [name,rsc_names]=nodes.emplace_back(NodeDependencies{ fgid_name(node_id) });
							for (auto rsc: rsc_span)
							{
								rsc_names.emplace_back(fgrid_name(rsc.id));
							}
						}

				}
				void Debug(const graph_theory::IntermediateGraph& igraph, span<const FrameGraphNode> nodes_)
				{
					hash_table<size_t, vector<size_t>> dependencies;
					std::set<size_t> indices;
					for (size_t i = 0; i < igraph.adjacency_list.size(); ++i)
					{
						for (auto adj_node : igraph.AdjacencyList(i))
						{
							dependencies[adj_node].emplace_back(i);
							indices.emplace(adj_node);
						}
					}
					for (auto index : indices)
					{
						auto& list = dependencies.at(index);
						auto& [name, deps] = named_dependencies.emplace_back();
						name = nodes_[index].name;
						for (auto& dep_index : list)
						{
							deps.emplace_back(nodes_[dep_index].name);
						}
					}

				}
			};
		}

	}
	void FrameGraph::Compile()
	{
		tmp_graph.buffer = &graph_builder.consumed_resources;
		auto graph = ConvertTempGraph(std::move(tmp_graph));
		graph_theory::IntermediateGraph dependency_graph{ nodes.size() };
		hash_table<fg_id, size_t> id_to_indices;

		for (size_t i = 0; i < nodes.size(); ++i)
		{
			id_to_indices[nodes[i].id] = i;
		}
		auto& nodes_ = nodes;
		auto& rsc_manager = GetResourceManager();
		auto fg_id_to_name = [&id_to_indices, &nodes_](fg_id id) ->string { return nodes_.at(id_to_indices.at(id)).name; };
		auto fgr_id_to_name = [&rsc_manager,&nodes_](fgr_id id) ->string { return rsc_manager.GetResourceDescription(id)->name; };

		{
			vector<size_t> index_buffer;

			//src,dst
			hash_table<fg_id, hash_set<fg_id>> dependant_nodes;
			hash_table<fg_id, std::pair<string,vector<string>>> dbg_dependant_nodes;

			for (auto [dst, index] : id_to_indices)
			{
				auto oid_adj_buffer = graph.get_input_nodes(dst); //input resources
				auto id_adj_buffer = *oid_adj_buffer;
				for (auto in_rsc : id_adj_buffer)
				{
					auto src_node = in_rsc;
					if (src_node != dst)
						dependant_nodes[src_node].emplace(dst);
				}
			}
			auto get_name = [this,&id_to_indices](fgr_id node_id) {return nodes[id_to_indices.at(node_id)].name; };
			if(frame_graph_debug)
				for (auto [node_id, dep_nodes] : dependant_nodes)
				{
					if (node_id)
					{
						dbg_dependant_nodes[node_id].first = get_name(node_id);
							for (auto dep_node_id : dep_nodes)
						{

								if (dep_node_id)
									dbg_dependant_nodes[node_id].second.emplace_back(get_name(dep_node_id));
						}
					}
				}

			index_buffer.reserve(nodes.size());
			for (auto& [id, src_index] : id_to_indices)
			{
				//auto oid_adj_buffer = graph.get_input_nodes(id); //input resources
				//auto id_adj_buffer = *oid_adj_buffer;
				//auto& lookup_table = graph_builder.origin_nodes;
				auto& id_adj_buffer = dependant_nodes[id];
				index_buffer.resize(id_adj_buffer.size());
				std::transform(id_adj_buffer.begin(), id_adj_buffer.end(), index_buffer.begin(), [&id_to_indices](fg_id id) {return id_to_indices.find(id)->second; });
				dependency_graph.SetAdjacentNodes(src_index, index_buffer.begin(), index_buffer.end());
			}
			vector<string_view> names(nodes.size());
			for (size_t i = 0; i < nodes.size(); ++i)
			{
				names[i] = nodes_[i].name;
			}
			if (frame_graph_debug)
			{
				dbg::fg::DependencyDebugger dep_dbg{ graph,fg_id_to_name,fgr_id_to_name };
				dep_dbg.Debug(dependency_graph, nodes_);
			}
			dependency_graph.names = names;
			auto [sorted_order, success] = graph_theory::KahnsAlgorithm(dependency_graph);
			IDK_ASSERT_MSG(success, "Cyclic dependency detected.");
			//Maybe use copy_if to filter out useless nodes.
			execution_order = std::move(sorted_order);
			_dbg_execution_order.clear();
			if(frame_graph_debug)
				for (auto index : execution_order)
				{
					auto& node = nodes[index];
					_dbg_execution_order.emplace_back(node.name,&*render_passes.at(node.id),&node);
				}
		}
		ComputeLifetimes(graph, rsc_lifetime_mgr);
	}

	void FrameGraph::AllocateResources()
	{
		CreateConcreteResources(rsc_lifetime_mgr, this->graph_builder.rsc_manager);
	}
	void FrameGraph::BuildRenderPasses()
	{
		CreateRenderPasses();
	}

	void FrameGraph::SetDefaultUboManager(UboManager& ubo_manager)
	{
		_default_ubo_manager = &ubo_manager;
	}

	void FrameGraph::SetPipelineManager(PipelineManager& pipeline_manager)
	{
		_default_pipeline_manager = &pipeline_manager;
	}

	void TransitionResource([[maybe_unused]] FrameGraph::Context_t context, [[maybe_unused]] TransitionInfo info)
	{
		//TODO: Actually transition
	}
	bool ValidateImageLayout(vk::ImageLayout layout)
	{
		bool valid = true;
		switch (layout)
		{
		case vk::ImageLayout::eUndefined:
			break;
		case vk::ImageLayout::eGeneral:
			break;
		case vk::ImageLayout::eColorAttachmentOptimal:
			break;
		case vk::ImageLayout::eDepthStencilAttachmentOptimal:
			break;
		case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
			break;
		case vk::ImageLayout::eShaderReadOnlyOptimal:
			break;
		case vk::ImageLayout::eTransferSrcOptimal:
			break;
		case vk::ImageLayout::eTransferDstOptimal:
			break;
		case vk::ImageLayout::ePreinitialized:
			break;
		case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
			break;
		case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
			break;
		case vk::ImageLayout::ePresentSrcKHR:
			break;
		case vk::ImageLayout::eSharedPresentKHR:
			break;
		case vk::ImageLayout::eShadingRateOptimalNV:
			break;
		case vk::ImageLayout::eFragmentDensityMapOptimalEXT:
			break;
		default:
			valid = false;
			break;
		}
		return valid;
	}
	dbg::time_log& GetGfxTimeLog();
	void FrameGraph::Execute()
	{
		dbg::stopwatch timer;
	GetGfxTimeLog().start("Reset Ctx");
		timer.start();
		/*
		if (_uniform_managers.empty())
			_uniform_managers.emplace_back();
		/*/
		//if (_uniform_managers.size() < execution_order.size())
		//{
		//	_uniform_managers.resize(execution_order.size());
		//}
		//for (auto& um : _uniform_managers)
		//{
		//	um.Reset();
		//}
		//*/
		//GetGfxTimeLog().push_level();
		//GetGfxTimeLog().pop_level();
		dbg::get_rendertask_durations().clear();
		for (auto& ctx : _contexts)
		{
			ctx.Reset();
		}
	GetGfxTimeLog().end();// "Reset Ctx");
		//_contexts.clear();
		if (_contexts.size() < execution_order.size())
			_contexts.resize(execution_order.size());
		auto& rsc_manager = GetResourceManager();
		hash_table<string, dbg::milliseconds> lut;
		std::array<dbg::milliseconds, 12> lut2{};
		std::array<string_view, 12> lut2s{};
		vector<mt::Future<void>> futures;
		size_t i = 0;
		for (auto index : execution_order)
		{
			i = 0;
			futures.emplace_back( Core::GetThreadPool().Post(
				[&rsc_manager,index, this]()
				{
					auto& node = nodes[index];
					auto& rp = *render_passes[node.id];
					auto& p_ubo_manager = _ubo_managers.at(mt::thread_id());
					if (!p_ubo_manager)
						p_ubo_manager = std::make_unique<UboManager>(View());
					//TODO: Thread this
					{
						//auto& context = _contexts.emplace_back(_uniform_managers.back());
						auto& context = _contexts.at(index);
						context.FlagUsed();
						context.SetPipelineManager(*_default_pipeline_manager);
						context.SetUboManager(*p_ubo_manager);
						//Transition all the resources that are gonna be read (and are not input attachments)
						auto input_span = node.GetReadSpan();
						for (auto& input : input_span)
						{
							TransitionResource(context, rsc_manager.TransitionInfo(input));
						}
						context.SetRscManager(rsc_manager);
						rp.PreExecute(node, context);
						//lut[] +=timer.lap();
						//lut2s[i] = "Pre Execute";
						//{
						//	auto lap = timer.lap();
						//	lut2[i++] += lap;
						//
						//	GetGfxTimeLog().log(lut2s[i - 1], lap);
						//}
						auto copy_span = node.GetCopySpan();
						for (auto& copy_req : copy_span)
						{
							CopyCommand cmd;
							auto src_layout = GetSourceLayout(copy_req.src.id);
							if (!ValidateImageLayout(src_layout))
								src_layout = src_layout;
							auto dst_layout = copy_req.options.dest_layout;
							if (!ValidateImageLayout(dst_layout))
								dst_layout = dst_layout;
							context.Copy(CopyCommand{ rsc_manager.Get<VknTextureView>(copy_req.src.id),src_layout,copy_req.options.src_range,rsc_manager.Get<VknTextureView>(copy_req.dest.id),dst_layout,copy_req.options.dst_range,copy_req.options.regions });
						}
						//lut2s[i] = "Copy";
						//{
						//	auto lap = timer.lap();
						//	lut2[i++] += lap;
						//
						//	GetGfxTimeLog().log(lut2s[i - 1], lap);
						//}
						//lut["Copy"] += timer.lap();
						rp.Execute(context);
						//lut2s[i] = "Execute";
						//{
						//	auto lap = timer.lap();
						//	lut2[i++] += lap;
						//
						//	GetGfxTimeLog().log(lut2s[i - 1], lap);
						//}
						//lut["Execute"] += timer.lap();
						rp.PostExecute(node, context);
						//lut2s[i] = "Post Execute";
						//{
						//	auto lap = timer.lap();
						//	lut2[i++] +=lap;
						//
						//	GetGfxTimeLog().log(lut2s[i-1], lap);
						//}

						//lut["PostExecute"] += timer.lap();

					}
				}
			));
		}
		for (auto& future : futures)
		{
			future.get();
		}
		//for (size_t j = 0; j < i; ++j)
		//{
		//	GetGfxTimeLog().log(lut2s[j], lut2[j]);
		//}
		//auto& duray = dbg::get_rendertask_durations();
		//dbg::milliseconds total{};
		//for (auto& [name, fduration] : duray)
		//{
		//	auto duration = dbg::milliseconds{ fduration };
		//	total += duration;
		//	GetGfxTimeLog().log_n_store(name, duration);
		//}
		//GetGfxTimeLog().log_n_store("render task extras", total);


	}
	struct count_duration
	{
		int count{};
		dbg::milliseconds total_duration;
		count_duration& operator+=(count_duration rhs)
		{
			count += rhs.count;
			total_duration += rhs.total_duration;
			return *this;
		}
	};

	hash_table<string, count_duration>& ProcBatchLut()
	{
		static hash_table<string, count_duration> lut;
		return lut;
	}

	void FrameGraph::ProcessBatches(RenderBundle& bundle)
	{
		dbg::stopwatch timer;
		timer.start();

GetGfxTimeLog().start("Compile");

		if (_duds.empty())
		{
			_duds.emplace_back(DescriptorUpdateData{ alloc });//Consider scaling with threads
		}
	GetGfxTimeLog().start("Dud Stuff");
		dbg::stopwatch timer2;
		timer2.start();
		for (auto& rt : _contexts)
		{
			_duds.back().Reset();
			//GetGfxTimeLog().end_then_start("Dud Reset", timer2.lap());
			rt.PreprocessDescriptors(_duds.back(),bundle._d_manager);
			//GetGfxTimeLog().end_then_start("Dud Pre Proc", timer2.lap());
			_duds.back().SendUpdates();
			//GetGfxTimeLog().end_then_start("Dud SendUpdates", timer2.lap());
		}
	GetGfxTimeLog().end_then_start("Proc Batches");
		GetGfxTimeLog().start("reset cmd clusters");
			ProcBatchLut().clear();
			_cmd_buffers.Reset();
		GetGfxTimeLog().end_then_start("Rsc Reserve");
			//std::mutex mutex;
			using buffer_pair_t = std::pair<RenderTask*, vk::CommandBuffer>;
			FixedArenaAllocator<char> allocator{};
			std::vector<buffer_pair_t,FixedArenaAllocator<buffer_pair_t>> buffers{ allocator };
			hash_table<RenderTask*, buffer_pair_t*> task_buffers;
			vector<mt::Future<void>> futures;
			buffers.resize(_contexts.size());
			auto buffer_itr = buffers.data();
			for (auto& rt : _contexts)
			{
				buffer_itr->first = &rt;
				task_buffers[&rt] = buffer_itr++;
			}
			futures.reserve(_contexts.size());
			std::array<dbg::milliseconds, 10> tmp_duration{};
		GetGfxTimeLog().end_then_start("Threaded stuff");
		for (auto& rt : _contexts)
		{
			futures.emplace_back(Core::GetThreadPool().Post(
				[&tmp_duration,&rt,&task_buffers, this]() {
					dbg::stopwatch tmp_timer;
					tmp_timer.start();
					auto buffer = _cmd_buffers.GetCommandBuffer();
					if (rt.BeginSecondaryCmdBuffer(buffer))
					{
					tmp_timer.stop();
					tmp_duration.at(mt::thread_id()) += tmp_timer.time();
						rt.ProcessBatches(buffer);
						buffer.end();
					}
					{
						task_buffers.at(&rt)->second = buffer;
						//std::lock_guard lock{ mutex };
						////must still place even if BeginSecondaryCmdBuffer returns false, there maybe non-renderpass stuff that needs executing.
						//buffers.emplace_back(buffer,&rt);
					}
				}
			));/*
			();		//*/
		}
		for (auto& future : futures)
			future.get();
		//GetGfxTimeLog().end_then_start("Get Command Buffer & begin secondary buffer",std::reduce(tmp_duration.begin(), tmp_duration.end()));
		GetGfxTimeLog().end_then_start("Execute Secondary Command Buffers");
		
		for (size_t i = 0; i<buffers.size();++i)
		{
			auto& [p_rt, buffer] = buffers.at(i);
			if (p_rt->BeginRenderPass(bundle._cmd_buffer))
			{
				bundle._cmd_buffer.executeCommands(buffer);
				bundle._cmd_buffer.endRenderPass();
			}
		}
		GetGfxTimeLog().end();
		//for (auto& [name, pair] : ProcBatchLut())
		//{
		//	auto& [count, time] = pair;
		//	GetGfxTimeLog().log_n_store(name + string{ std::to_string(count) }, time);
		//}
	GetGfxTimeLog().end();// "Proc Batches");
GetGfxTimeLog().end();// "Compile");

GetGfxTimeLog().start("Update Ubo Managers");// "Compile");
		for (auto& ubo_manager : _ubo_managers)
		{
			if (ubo_manager)
			{
				ubo_manager->UpdateAllBuffers();
			}
		}
GetGfxTimeLog().end();// "Update Ubo Managers");// "Compile");
	}

	RenderPassCreateInfoBundle FrameGraph::CreateRenderPassInfo(span<const std::optional<FrameGraphAttachmentInfo>> input_rscs, span<const std::optional<FrameGraphAttachmentInfo>> output_rscs, std::optional<FrameGraphAttachmentInfo> depth)
	{
		RenderPassCreateInfoBundle bundle{};
		auto total_att = input_rscs.size() + output_rscs.size() + ((depth) ? 1 : 0);
		vector<vk::AttachmentDescription>& attachments = bundle.attachments();
		auto& attachment_input_refs = bundle.attachment_input_refs () =vector<vk::AttachmentReference> (input_rscs.size());
		auto& attachment_output_refs= bundle.attachment_output_refs() =vector<vk::AttachmentReference> (output_rscs.size());
		vk::AttachmentReference& depth_ref = bundle.depth_ref();
		auto& rsc_manager = GetResourceManager();
		auto& nodes_lookup = node_lookup;
		auto& nodes_buffer = nodes;
		attachments.reserve(total_att);
		auto& src_nodes = graph_builder.origin_nodes;
		auto& input_src_nodes = graph_builder.input_origin_nodes;
		auto& copy_src_nodes = graph_builder.copy_origin_nodes;
		auto process_attachment = [&src_nodes, &input_src_nodes,&copy_src_nodes, &nodes_buffer,&nodes_lookup,this,&rsc_manager, &attachments](auto& attachment_info, auto& attachment_ref,vk::ImageLayout actual_layout)
		{
			
			auto find_input_layout = [](const FrameGraphNode& node, fgr_id id)
			{
				for (auto& o_input : node.output_attachments)
				{
					if (o_input && o_input->first ==id)
					{
						return o_input->second.layout;
					}
				}
				if (node.depth_stencil)
					return node.depth_stencil->second.layout;
				return vk::ImageLayout::eGeneral;
			};
			if (attachment_info)
			{
				auto tex = rsc_manager.Get<VknTextureView>(attachment_info->first);
				auto& [rsc_id, att_opt] = *attachment_info;
				attachment_ref.attachment = static_cast<uint32_t>(attachments.size());
				attachment_ref.layout = actual_layout;
				
				auto prev_id = rsc_manager.GetPrevious(rsc_id);
				if (rsc_manager.BeforeWriteRenamed(FrameGraphResource{ rsc_id }))
				{
					auto prev = rsc_manager.GetPrevious(*prev_id);
					if (!prev)
						prev_id = {}; //If we created it in the same node, Create (id1) -> Write (id1 ->id2), there was no previous
				}
				auto get_alt_src = [&input_src_nodes, &copy_src_nodes ](fgr_id id)
				{
					{
						auto itr = input_src_nodes.find(id);
						if (itr != input_src_nodes.end())
							return itr->second;
					}
					{
						auto itr = copy_src_nodes.find(id);
						if (itr != copy_src_nodes.end())
							return itr->second;
					}
					throw;
					return fg_id{};
				};
				vk::ImageLayout prev_layout = att_opt.layout;
				/*if (prev_id)
				{
					auto src_itr = src_nodes.find(*prev_id);
					auto node_id = (src_itr != src_nodes.end()) ? src_itr->second : get_alt_src(*prev_id);
					auto node_to_index = nodes_lookup.find(node_id);
					if (prev_id)
					{
						
						auto node_index = node_to_index->second;
						auto& node = nodes_buffer[node_index];
						auto copy_span = node.GetCopySpan();
						auto outp_span = node.GetOutputSpan();
						auto inpu_span = node.GetInputSpan();
						prev_layout = find_input_layout(node, *prev_id);
					}
				}*/
				if(prev_id)
					prev_layout = graph_builder.GetPostLayout(*prev_id);
				else 
				{
					
					prev_layout = (att_opt.load_op == vk::AttachmentLoadOp::eLoad)?vk::ImageLayout::eGeneral :vk::ImageLayout::eUndefined;
				}
				prev_layout = (att_opt.pre_layout) ? *att_opt.pre_layout : (prev_layout);
				
				attachments.emplace_back(vk::AttachmentDescription
					{
						//vk::AttachmentDescriptionFlagBits::eMayAlias
						{},
						(att_opt.format) ? (*att_opt.format) : tex.Format() ,
						vk::SampleCountFlagBits::e1,
						att_opt.load_op,
						att_opt.store_op,
						att_opt.stencil_load_op,
						att_opt.stencil_store_op,
						prev_layout,
						att_opt.layout,
					});
			}
			else
			{
				attachment_ref.attachment = VK_ATTACHMENT_UNUSED;
			}
		};
		auto process_attachments = [&rsc_manager, &attachments, &process_attachment](auto& attachment_infos, auto& attachment_refs,vk::ImageLayout actual_layout)
		{
			auto ref_itr = attachment_refs.begin();
			for (auto& input_rsc : attachment_infos)
			{
				process_attachment(input_rsc, *ref_itr, actual_layout);
				++ref_itr;
			}
		};


		{
			process_attachments(output_rscs, attachment_output_refs,vk::ImageLayout::eColorAttachmentOptimal);
		}
		if (depth)
		{
			process_attachment(depth, depth_ref,vk::ImageLayout::eDepthStencilAttachmentOptimal);
		}
		{
			process_attachments(input_rscs, attachment_input_refs, vk::ImageLayout::eShaderReadOnlyOptimal);
		}
		vk::SubpassDescription subpass_desc{
			vk::SubpassDescriptionFlags{},
			vk::PipelineBindPoint::eGraphics,
			static_cast<uint32_t>(attachment_input_refs.size()),
			std::data(attachment_input_refs),
			static_cast<uint32_t>(attachment_output_refs.size()),
			std::data(attachment_output_refs),
			nullptr,
			(depth) ? &depth_ref : nullptr,
			0,nullptr,
		};
		std::array<vk::SubpassDependency,1> subpass_dep
		{
			vk::SubpassDependency
			{
				VK_SUBPASS_EXTERNAL,0,
				vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests,  //TODO: Figure this out
				vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests, //TODO: Figure this out
				vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,//TODO: Figure this out
				vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentRead,	//TODO: Figure this out
				vk::DependencyFlagBits::eByRegion
			}
		/*	vk::SubpassDependency{
			VK_SUBPASS_EXTERNAL,0,
			vk::PipelineStageFlagBits::eAllGraphics | vk::PipelineStageFlagBits::eTransfer, //vk::PipelineStageFlagBits::eTransfer | vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests,  //TODO: Figure this out
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests, //TODO: Figure this out
			vk::AccessFlagBits::eTransferWrite | vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,//TODO: Figure this out
			vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,	//TODO: Figure this out
			vk::DependencyFlagBits::eByRegion
			}
		,
			vk::SubpassDependency{
			0,VK_SUBPASS_EXTERNAL,
			vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests, //TODO: Figure this out
			vk::PipelineStageFlagBits::eAllGraphics, //vk::PipelineStageFlagBits::eTransfer | vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests,  //TODO: Figure this out
			vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,	//TODO: Figure this out
			vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,//TODO: Figure this out
			vk::DependencyFlagBits::eByRegion
			}
		*/};
		bundle.set(span{ &subpass_desc,&subpass_desc+1 }, span{ subpass_dep });
		return bundle;
	}

	VknRenderPass FrameGraph::CreateRenderPass(string_view name,span<const std::optional<FrameGraphAttachmentInfo>> input_rscs, span<const std::optional<FrameGraphAttachmentInfo>> output_rscs, std::optional<FrameGraphAttachmentInfo> depth)
	{
		vk::Device device = *View().Device();
		auto info = CreateRenderPassInfo(input_rscs, output_rscs, depth);
		auto result =_rp_pool.GetRenderPass(info);
		dbg::NameObject(*result, name);
		return result;
	}
	std::pair<Framebuffer, uvec2> FrameGraph::CreateFrameBuffer(VknRenderPass rp, span<const std::optional<FrameGraphAttachmentInfo>> input_rscs, span<const std::optional<FrameGraphAttachmentInfo>> output_rscs, std::optional<FrameGraphAttachmentInfo> depth)
	{
		vk::Device d = *View().Device();
		auto& rsc_manager = GetResourceManager();
		vector<vk::ImageView> targets;
		uvec2 size{ std::numeric_limits<uint32_t>::max() }; //TODO get an actual size
		uvec2 virtual_size{std::numeric_limits<uint32_t>::max()}; 
		uint32_t num_layers = std::numeric_limits<uint32_t>::max();
		for (auto& output_rsc : output_rscs)
		{
			if (output_rsc)
			{
				auto tex = rsc_manager.Get<VknTextureView>(output_rsc->first);
				auto desc = rsc_manager.GetResourceDescription(output_rsc->first);
				auto dv_size = (desc->actual_rsc) ? desc->actual_rsc->as<VknTexture>().Size() : desc->size;
				targets.emplace_back(tex.ImageView());
				//Temp: getting min size
				size = min(tex.Size(),size);
				virtual_size = min(dv_size, virtual_size);
				num_layers = min(tex.Layers(),num_layers);
			}
		}
		auto test = min(virtual_size, size);
		if (virtual_size - test != uvec2{})
		{
			DebugBreak();
		}
		if (depth)
		{
			auto tex = rsc_manager.Get<VknTextureView>(depth->first);
			targets.emplace_back(tex.ImageView());
			auto desc = rsc_manager.GetResourceDescription(depth->first);
			auto dv_size = (desc->actual_rsc) ? desc->actual_rsc->as<VknTexture>().Size() : desc->size;
			virtual_size = min(dv_size, virtual_size);
			//Temp: getting min size
			size = min(tex.Size(), size);
			num_layers = min(tex.Layers(), num_layers);
		}
		for (auto& input_rsc : input_rscs)
		{
			if (input_rsc)
			{
				auto tex = rsc_manager.Get<VknTextureView>(input_rsc->first);
				targets.emplace_back(tex.ImageView());
				//Temp: getting min size
				size = min(tex.Size(), size);
				num_layers = min(tex.Layers(), num_layers);
			}
		}
		//TODO shift this into a pool and use unique
		return { _fb_pool.GetFramebuffer(vk::FramebufferCreateInfo
			{
				{},rp,
				static_cast<uint32_t>(targets.size()),
				std::data(targets),
				size.x,size.y,num_layers
			}),virtual_size };
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
			rp.skip_render_pass = node.skip_render_pass;
			if (!rp.skip_render_pass)
			{
				rp.render_pass = CreateRenderPass(node.name, input_attachments, output_attachments, node.depth_stencil);
				auto [fb, size] = CreateFrameBuffer(rp.render_pass, input_attachments, output_attachments, node.depth_stencil);
				rp.frame_buffer = fb;
				rp.fb_size = size;
			}
		}
	}

	const FrameGraphNode* FrameGraph::GetSourceNode(fgr_id id) const
	{
		auto src_node_id = this->graph_builder.GetOutputNode(id);
		if (!src_node_id)
		{
			return nullptr;
		}
		auto node_itr = this->node_lookup.find(*src_node_id);

		if (node_itr == this->node_lookup.end())
			return nullptr;
		
		auto& node = this->nodes[node_itr->second];
		return &node;
	}
	static int dbg_counter = 0;
	vk::ImageLayout FrameGraph::GetSourceLayout(fgr_id id) const
	{
		vk::ImageLayout result = {};
		/*
		auto tmp = GetResourceManager().GetPrevious(id);
		if (!tmp)
		{
			auto info = GetResourceManager().GetResourceDescription(id);
			string_view name = "[Descriptionless]";
			if (info)
				name = info->name;
			LOG_ERROR_TO(LogPool::GFX, "Attempting to get source layout for resource [%s] that has not been written to.", name.data());
			throw std::runtime_error("Attempting to get source layout for a resource that has not been written to.");
		}*/
		auto& rsc_manager = GetResourceManager();
		//auto curr_node_ptr = GetSourceNode(id);

		auto src_id = *GetResourceManager().GetPrevious(id);
		++dbg_counter;
		while (!rsc_manager.IsWriteRenamed(FrameGraphResource{ src_id }))
		{
			auto prv = GetResourceManager().GetPrevious(src_id);
			if (!prv)
				break;
			src_id = *prv;
		}

		
		if (rsc_manager.IsWriteRenamed(FrameGraphResource{ src_id }))
		{
		auto derp = rsc_manager.WriteRenamed(FrameGraphResource{ src_id });

		src_id = derp.id;
		auto node_ptr = GetSourceNode(src_id);
		//while(curr_node_ptr == node_ptr)
		//{
		//	src_id = *GetResourceManager().GetPrevious(src_id);
		//	node_ptr = GetSourceNode(src_id);
		//}
		//if (node_ptr == nullptr)
		//	node_ptr = curr_node_ptr;
		if (node_ptr)
		{
			auto& node = *node_ptr;
			auto output_nodes = node.GetOutputSpan();
			auto output_ptr = std::find(output_nodes.begin(), output_nodes.end(), FrameGraphResource{ src_id });
			auto output_index = output_ptr - output_nodes.begin();
			if (node.depth_stencil && src_id == node.depth_stencil->first || id == node.depth_stencil->first)
			{
				result = node.depth_stencil->second.layout;

				if (!ValidateImageLayout(result))
					DoNothing();
			}
			else
			if (output_ptr < output_nodes.end() && output_index < s_cast<long long>(node.output_attachments.size()))
			{
				auto& att_info = node.output_attachments[output_index];
				if (att_info) //is an output attachment
				{
					result = att_info->second.layout;
					if (!ValidateImageLayout(result))
						DoNothing();
				}
				else //is not an output attachment
				{
					DoNothing();
				}
			}
			else
			{
				DoNothing();
			}
		}
		else
		{
			__debugbreak();
		}
		}
		else
		{
			result = graph_builder.GetPostLayout(id);
		}
		return result;
	}

	FrameGraphResourceManager& FrameGraph::GetResourceManager()
	{
		return graph_builder.rsc_manager;
	}

	const FrameGraphResourceManager& FrameGraph::GetResourceManager() const
	{
		return graph_builder.rsc_manager;
	}
	ActualGraph ConvertTempGraph(TempGraph&& tmp)
	{
		ActualGraph result{
			.src_node = std::move(tmp.src_node)
		};
		vector<fg_id> node_accum;
		for (auto& [id, idx_span] : tmp.in_nodes)
		{
			auto rsc_span = idx_span.to_span(tmp.buffer->resources);
			result.in_rsc_nodes.emplace(id, rsc_span);
			node_accum.clear();
			node_accum.resize(rsc_span.size());
			auto& rsc_origin_map = result.src_node;
			std::transform(rsc_span.begin(), rsc_span.end(), node_accum.begin(), [&rsc_origin_map](auto& rsc) {
				auto itr = rsc_origin_map.find(rsc.id);
				return (itr!=rsc_origin_map.end())?itr->second:0; 
				});
			result.set_input_nodes(id, node_accum.begin(), node_accum.end());
		}
		tmp.in_nodes.clear();
		return result;
	}

	ActualGraph BuildGraph(const vector<FrameGraphNode>& nodes, const NodeBuffer& rscs, const FrameGraphResource& root)
	{
		ActualGraph graph;
		FG_Traverser traverser;
		FrameGraphNode root_node;
		for (auto& node : nodes)
		{
			if (node.Writes(root))
			{
				root_node = node;
				break;
			}
		}
		traverser.Push(root_node);
		ConversionPolicy cp{ &graph, nodes,rscs };
		while (traverser.VisitNextNode(cp));
		return graph;
	}

	NodeDependencies BuildDependencyGraph(const vector<FrameGraphNode>& nodes, const ActualGraph& graph)
	{
		NodeDependencies dep;
		for (auto& node : nodes)
		{
			auto node_id = node.id;
			auto in_itr = graph.in_rsc_nodes.find(node_id);
			if (in_itr != graph.in_rsc_nodes.end())
			{
				for (auto& in_rsc : in_itr->second)
				{
					auto itr = graph.src_node.find(in_rsc.id);
					auto in_src_node_id = itr->second;
					auto& my_deps = dep.flattened_dependencies[node_id];
					my_deps.emplace(in_src_node_id);
					for (auto& sub_dep : dep.flattened_dependencies[in_src_node_id])
					{
						my_deps.emplace(sub_dep);
					}
					auto& rev_deps = dep.reversed_dependencies[node_id];
					for (auto reversed_dep : rev_deps)
						dep.flattened_dependencies[reversed_dep].emplace(in_src_node_id);
					rev_deps.emplace(node_id);
				}
			}
		}
		return dep;
	}

}