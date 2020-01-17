#pragma once
#include <idk.h>
#include "FrameGraphNode.h"
#include "FrameGraphResource.h"
#include "FrameGraphResourceManager.h"
#include "FrameGraphBuilder.h"
#include "ResourceLifetimeManager.h"
#include "BaseRenderPass.h"
#include <vkn/utils/GraphTraverser.h>
#include <vkn/topological_sort.h>
namespace idk::vkn
{

	struct TempGraph
	{
		hash_table<fgr_id, fg_id> src_node;
		hash_table<fgr_id, fg_id> end_node;
		using fgr_span = index_span;
		hash_table<fg_id, fgr_span> in_nodes;
		const NodeBuffer* buffer;
	};
	struct ActualGraph
	{
		hash_table<fgr_id, fg_id>  src_node;
		hash_table<fgr_id, fg_id>  end_node;
		hash_table<fg_id, span<const FrameGraphResource>> in_rsc_nodes;
		hash_table<fg_id, index_span> in_nodes;

		auto get_input_nodes(fg_id id)const
		{
			std::optional<span<const fg_id>> result{};
			auto itr = in_nodes.find(id);
			if (itr != in_nodes.end())
				result = itr->second.to_span(in_buffer);
			return result;
		}

		template<typename itr>
		void set_input_nodes(fg_id dst, itr begin, itr end)
		{
			auto& span = in_nodes[dst];
			span.begin = in_buffer.size();
			in_buffer.resize(in_buffer.size() + std::distance(end, begin));
			std::copy(begin, end, in_buffer.begin() + span.begin);
			span.end = in_buffer.size();
		}
		vector<fg_id> in_buffer;
	};
	ActualGraph ConvertTempGraph(TempGraph&& tmp)
	{
		ActualGraph result{
			 .src_node = std::move(tmp.src_node)
			,.end_node = std::move(tmp.end_node)
		};
		vector<fg_id> node_accum;
		for (auto& [id, idx_span] : tmp.in_nodes)
		{
			auto rsc_span = idx_span.to_span(tmp.buffer->resources);
			result.in_rsc_nodes.emplace(id, rsc_span);
			node_accum.clear();
			node_accum.resize(rsc_span.size());
			std::transform(rsc_span.begin(), rsc_span.end(), node_accum.begin(), [](auto& rsc) {return rsc.id; });
			result.set_input_nodes(id, node_accum.begin(), node_accum.end());
		}
		tmp.in_nodes.clear();
		return result;
	}
	using FG_Traverser = GraphTraverser<FrameGraphNode, fg_id>;

	struct ConversionPolicy
	{

		using Node = FG_Traverser::template VisitPolicy<ConversionPolicy>::node_t;
		using Key = FG_Traverser::template VisitPolicy<ConversionPolicy>::key_t;

		//Returns true to continue getting deeper edges, or false to not explore the node any further.
		bool Visit(const Node& node)
		{
			graph->in_rsc_nodes.emplace(node.id, node.GetInputSpan());
			for (auto&& written_rsc : node.GetOutputSpan())
			{
				graph->src_node.emplace(written_rsc.id, node.id);
			}
			return true;
		}
		//Returns a range of edges that lead deeper into the graph. Edge is used by GetNodeInfo, and hence must be compatible.
		auto DeeperEdges(const Node& node)
		{
			return node.GetInputSpan();
		}
		//Returns an info_t object by copy, that can be passed int GetKeyFromInfo and GetNodeFromInfo which can return the Key and Node by reference respectively.
		auto GetNodeInfo(const Node&, const FrameGraphResource& edge)
		{
			std::optional<Node> result;
			for (auto& node : all_nodes)
			{
				if (node.Writes(edge))
				{
					result = node;
					break;
				}
			}
			return result;
		}
		//Extracts the Key from the info
		const Key& GetKeyFromInfo(const std::optional<Node>& info)
		{
			IDK_ASSERT(info);
			return info->id;
		}
		//Extracts the Node from the info
		const Node& GetNodeFromInfo(const std::optional<Node>& info)
		{
			IDK_ASSERT(info);
			return *info;
		}
		ActualGraph* graph;
		const vector<FrameGraphNode>& all_nodes;
		const NodeBuffer& rscs;
	};

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

	struct NodeDependencies
	{
		hash_table<fg_id, set<fg_id>> flattened_dependencies;
		hash_table<fg_id, set<fg_id>> reversed_dependencies;
	};
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

	struct FrameGraph
	{
		//Maybe do some tuple get thing based on the pre-registered context?
		//Holds:
		// - A Command Buffer
		// - A reference to the framegraphresourcemanager (to get resources)
		// - Render Tasks
		using Context_t = FrameGraphDetail::Context_t;
		template<typename T>
		using ExecFunc = function<void(T&, Context_t context)>;


		template<typename T>
		struct CallbackRenderPass : BaseRenderPass
		{
			T value;
			ExecFunc<T> execute;
			template<typename InitFunc, typename ... Args>
			CallbackRenderPass(FrameGraphBuilder& graph_builder, InitFunc&& init, ExecFunc<T>&& func, Args&& ... args) : value{ std::forward<Args>(args)... }, execute{ std::move(func) }
			{
				init(graph_builder, value);
			}
			void Execute(Context_t context)override
			{
				execute(value, context);
			}
		};

		template<typename T, typename ... T_CtorArgs>
		T& addCallBackPass(string_view name,
			const std::function<void(FrameGraphBuilder & builder, T&)>& init,
			ExecFunc<T> execute,
			T_CtorArgs&& ... args
		)
		{
			auto& render_pass = addRenderPass<CallbackRenderPass<T>>(name, init, std::move(execute), std::forward <T_CtorArgs>(args)...);
			return render_pass.value;
		}
		template<typename T, typename ...CtorArgs>
		T& addRenderPass(string_view name, CtorArgs&&...args)
		{
			graph_builder.BeginNode();
			auto render_pass = std::make_unique<T>(graph_builder, std::forward <CtorArgs>(args)...);
			auto& node = StoreNode(graph_builder.EndNode());
			T& obj = *render_pass;
			render_passes.emplace(node.id, std::move(render_pass));
			return obj;

		}

		FrameGraphNode& StoreNode(FrameGraphNode&& node);


		void ComputeLifetimes(ResourceLifetimeManager& manager);

		void CreateConcreteResources(ResourceLifetimeManager& rlm, FrameGraphResourceManager& rm);

		//Process nodes and cull away unnecessary nodes.
		//Figure out dependencies and synchronization points
		//Insert new nodes to help transit concrete resources to their corresponding virtual resources
		//Generate dependency graph
		void Compile();

		//using the resultant graph, allocate the concrete resources needed.
		void AllocateResources();
		void BuildRenderPasses();

		//Execute the renderpasses.
		//Use the dependency graph to split the appropriate jobs into separate threads and sync those.
		void Execute(Context_t context);

		//Check if there's an existing renderpass that is compatible, reuse if compatible.
		vk::RenderPassCreateInfo  CreateRenderPassInfo(span<const std::optional<FrameGraphAttachmentInfo>> input_rscs, span<const std::optional<FrameGraphAttachmentInfo>> output_rscs, std::optional<FrameGraphAttachmentInfo> depth);
		VknRenderPass  CreateRenderPass(span<const std::optional<FrameGraphAttachmentInfo>> input_rscs, span<const std::optional<FrameGraphAttachmentInfo>> output_rscs,std::optional<FrameGraphAttachmentInfo> depth);
		VknFrameBuffer CreateFrameBuffer(VknRenderPass rp, span<const std::optional<FrameGraphAttachmentInfo>> input_rscs, span<const std::optional<FrameGraphAttachmentInfo>> output_rscs, std::optional<FrameGraphAttachmentInfo> depth);

		void CreateRenderPasses();

		FrameGraphResourceManager& GetResourceManager();
		const FrameGraphResourceManager& GetResourceManager()const ;

		FrameGraphBuilder graph_builder;
		vector<FrameGraphNode> nodes;
		//fg id to index in nodes;
		hash_table<fg_id, size_t> node_lookup;
		vector<size_t> execution_order;
		hash_table<fg_id, std::unique_ptr<BaseRenderPass>> render_passes;

		TempGraph tmp_graph;
		ResourceLifetimeManager rsc_lifetime_mgr;
	};

}