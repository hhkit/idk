#pragma once
#include <cstdint>
#include <math/rect.h>
#include <gfx/pipeline_config.h>
#include <vkn/utils/Flags.h>
#include <ds/ranged_for.inl>
#include <stack>
#include <queue>
namespace idk::vkn
{
	template<typename Node, typename Key, typename Policy>
	struct TraversePolicy
	{
		using node_t = Node;
		using key_t = Key;
		TraversePolicy(Policy& pol) :policy{ pol } {}

		//Returns true to continue getting deeper edges, or false to not explore the node any further.
		bool Visit(const Node& node)
		{
			return policy.Policy::Visit(node);
		}
		//Returns a range of edges that lead deeper into the graph. Edge is used by GetNodeInfo, and hence must be compatible.
		auto DeeperEdges(const Node& node)
		{
			return policy.Policy::DeeperEdges(node);
		}
		//Returns an info_t object by copy, that can be passed int GetKeyFromInfo and GetNodeFromInfo which can return the Key and Node by reference respectively.
		template<typename Edge>
		auto GetNodeInfo(const Node& node, const Edge& edge)
		{
			return policy.Policy::GetNodeInfo(node, edge);
		}
		//Extracts the Key from the info
		template<typename Info>
		const Key& GetKeyFromInfo(const Info& info)
		{
			return policy.Policy::GetKeyFromInfo(info);
		}
		//Extracts the Node from the info
		template<typename Info>
		const Node& GetNodeFromInfo(const Info& info)
		{
			return policy.Policy::GetNodeFromInfo(info);
		}
		Policy& policy;
	};
	template<typename Node>
	using DepthFirst = std::stack<Node, vector<Node>>;
	template<typename Node>
	using BreadthFirst = std::queue<Node, vector<Node>>;

	template<typename Node, typename Key = Node, template <typename Node_t> typename TraverseType = DepthFirst>
	struct GraphTraverser
	{
		template<typename Policy>
		using VisitPolicy = TraversePolicy<Node, Key, Policy>;
		void Push(Node node)
		{
			frontier.push(node);
		}
		std::optional<Node> Pop()
		{
			if (!frontier.empty())
				return {};
			auto result = frontier.top();
			frontier.pop();
			return result;
		}
		template<typename Policy>
		bool VisitNextNode(Policy& policy_detail)
		{
			VisitPolicy<Policy> policy{ policy_detail };
			auto next = Pop();
			if (next)
			{
				auto& node = *next;
				if (policy.Visit(node))
				{
					auto&& input_span = policy.DeeperEdges(node);
					for (auto& edge : input_span)
					{
						auto&& info = policy.GetNodeInfo(node, edge);
						auto&& key = policy.GetKeyFromInfo(info);
						//Not opened
						if (opened_nodes.find(key) == opened_nodes.end())
						{
							//Add to list of nodes to visit
							Push(policy.GetNodeFromInfo(info));
						}
					}
				}
			}
			return next.operator bool();
		}

		TraverseType<Node> frontier;
		hash_set<Key> opened_nodes;
	};



	enum LoadOp {};
	enum StoreOp {};
	struct Framebuffer;
	struct VertexBuffer;
	struct ShaderModule;
	struct Texture;
	using TextureID  = Guid;
	using CullFaceFlags = Flags<CullFace>;

	struct indexed_draw_info
	{
		uint32_t num_indices;
		uint32_t num_instances;
		uint32_t first_vertex;
		uint32_t first_index;
		uint32_t first_instance;
	};
	struct vertex_draw_info
	{
		uint32_t num_vertices;
		uint32_t num_instances;
		uint32_t first_vertex;
		uint32_t first_instance;
	};

	using draw_info = std::variant<indexed_draw_info, vertex_draw_info>;

	struct Shaders {};

	struct index_span
	{
		size_t begin = 0, end = 0;
	};
	struct AttachmentDescription
	{
		string_view name;
		ivec2 size;
		//Format format;
	};
	struct AttachmentInfo
	{
		TextureID target;
		std::optional<AttachmentBlendConfig> config;
	};

	/*
	****************
	Objective:
		- Provide an interface to deal with a sub renderpass
		- Be processed by a system to generate the appropriate vulkan render pass and pipeline information
	****************
	*/

	struct SubPass
	{
#pragma region FrameBuffers
		void BindInputAttachment(uint32_t index, const Texture& texture);

		void SetOutputAttachment(uint32_t index, const Texture& texture, LoadOp load_op, StoreOp store_op);
		void SetDepthtAttachment(const Texture& texture, LoadOp load_op, StoreOp store_op);
		void SetAttachmentBlending(uint32_t index, AttachmentBlendConfig blend);
#pragma endregion
		size_t subpass_index; //generated and assigned by a subpass system
	private:
		vector<AttachmentInfo> input_attachments;
		vector<AttachmentInfo> output_attachments;
	};

	struct RenderTask
	{
		void Associate(size_t subpass_index);

		void BindVertexBuffer(uint32_t binding, const VertexBuffer& vertex_buffer, size_t byte_offset);

#pragma region Uniforms
		void BindUniform(string_view name, string_view data);
		void BindUniform(string_view name, const Texture& texture);
#pragma endregion

		void BindShader(const ShaderModule& shader);

#pragma region Draw
		void Draw(uint32_t num_vertices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_instance);
		void DrawIndexed(uint32_t num_indices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_index, uint32_t first_instance);
#pragma endregion

#pragma region PipelineConfigurations
		void Inherit(const pipeline_config& config);
		//Here we only support color, should you wish to do a skybox, please set the color to nullopt and render the skybox yourself.
		void SetClearColor(uint32_t attachment_index, std::optional<color> col);
		void SetClearDepthStencil(std::optional<float> depth, std::optional<uint8_t> stencil);
		void SetScissors(rect);
		void SetViewport(rect);
		void SetFillType(FillType);
		void SetCullFace(CullFace);
		void SetPrimitiveTopology(PrimitiveTopology);
		void SetDepthTest(bool enabled);
		void SetDepthWrite(bool enabled);
		void SetStencilTest(bool enabled);
		void SetStencilWrite(bool enabled);
#pragma endregion 
		const pipeline_config GetCurrentConfig()const noexcept;
	private:


		struct UboData {};
		struct TexData {};
		struct VertexBindingData
		{
			//vk::Buffer buffer;
			uint32_t binding;
			uint32_t offset;
		};
		struct DrawCall
		{
			//Use spans so that this is flat and we don't pay for multiple allocations (versus vector)
			//Use indices so that even if we transform the buffers, as long as the order doesn't change, our indices remain correct.
			index_span ubos;
			index_span texs;
			index_span vertex_buffers;

			draw_info draw_info; //might need to split into two types/vectors of draw calls to prevent branching
		};

		struct RenderBatch
		{
			pipeline_config pipeline;
			Shaders shaders;
			vector<DrawCall> draw_calls;
		};



#pragma region Initial Data
		vector<UboData> ubos;
		vector<TexData> uniform_textures;

		vector<VertexBindingData> vertex_buffers;
#pragma endregion 
		std::optional<vec4> clear;

		pipeline_config curr_config;

		vector<RenderBatch> batches;
	};
	using fgr_id = size_t;
	struct FrameGraphResource 
	{
		fgr_id id;
		bool readonly = true;
	};
	using FrameGraphResourceReadOnly = FrameGraphResource;
	using FrameGraphResourceMutable  = FrameGraphResource;
	using fg_id = size_t;
	struct FrameGraphNode 
	{
		fg_id id;
		
		const vector<FrameGraphResource>* buffer;

		index_span input_resources;
		index_span output_resources;
		index_span modified_resources;

		auto GetInputSpan()const { return GetSpan(input_resources, *buffer); }
		auto GetOutputSpan()const { return GetSpan(output_resources, *buffer); }

		bool resource_present(index_span span, FrameGraphResource rsc)const
		{
			auto begin = buffer->begin() + span.begin;
			auto end = buffer->begin() + span.end;
			auto itr = std::find(begin, end, rsc);
			return itr!=end;
		}

		bool Reads(FrameGraphResource rsc)const
		{
			return resource_present(input_resources, rsc);
		}
		bool Writes(FrameGraphResource rsc)const
		{
			return resource_present(output_resources, rsc);
		}
		bool Modifies(FrameGraphResource rsc)const
		{
			return resource_present(modified_resources, rsc);
		}

	};

	struct FrameGraphResourceManager
	{
		using rsc_index_t = size_t;
		using actual_rsc_index_t = size_t;
		using actual_resource_t = variant<Texture>;


		FrameGraphResource CreateTexture(AttachmentDescription dsc)
		{
			auto rsc_index = resources.size();
			resources.emplace_back(dsc);
			resource_handles.emplace(NextID(), rsc_index);
			return FrameGraphResource{ rsc_index };
		}
		FrameGraphResource Rename(FrameGraphResource rsc)
		{
			auto next_id = NextID();
			resource_handles.emplace(next_id, resource_handles.find(rsc.id)->second);
			renamed_resources.emplace(rsc.id, next_id);
			return FrameGraphResource{ next_id };
		}
		string_view Name(FrameGraphResource fg)const
		{
			auto itr = resource_handles.find(fg.id);
			if (itr == resource_handles.end())
				return "";
			return resources.at(itr->second).name;
		}

		template<typename ActualResource>
		ActualResource& Get(FrameGraphResource rsc)
		{
			return std::get<ActualResource>(GetVar(rsc));
		}

		actual_resource_t& GetVar(FrameGraphResource rsc)
		{
			return concrete_resources[resource_map.find(rsc.id)->second];
		}
		//Generate the next id.
		fgr_id NextID();


		vector<AttachmentDescription> resources;

		vector<actual_resource_t> concrete_resources;
		hash_table<fgr_id, actual_rsc_index_t> resource_map;

		hash_table<fgr_id, rsc_index_t> resource_handles;
		//Old to new
		hash_table<fgr_id, fgr_id> renamed_resources;
	};

	struct NodeBuffer
	{
		vector<FrameGraphResource> resources;
		index_span StoreResources(vector<FrameGraphResource>& rsc)
		{
			index_span result{ resources.size(),resources.size() };
			resources.reserve(rsc.size() + resources.size());
			std::copy(rsc.begin(), rsc.end(), resources.end());
			result.end = resources.size();
			return result;
		}
	};
	struct WriteOptions
	{
		bool clear = true;
	};

	struct FrameGraphBuilder 
	{
		//Keep track of virtual resources
		//AttachmentInfo CreateTexture(AttachmentDescription desc);
		//AttachmentInfo CreateTexture(const Texture& texture);
		fg_id NextID();

		FrameGraphResource CreateTexture(AttachmentDescription desc)
		{
			return rsc_manager.CreateTexture(desc);
		}

		FrameGraphResource read(FrameGraphResourceReadOnly in_rsc)
		{
			auto rsc = rsc_manager.Rename(in_rsc);
			curr_input_resources.emplace_back(in_rsc);
			return rsc;
		}
		FrameGraphResource write(FrameGraphResourceMutable target_rsc, WriteOptions opt = {})
		{
			auto rsc = rsc_manager.Rename(target_rsc);
			if (target_rsc.readonly)
			{
				LOG_TO(LogPool::GFX, "Attempting to write to Read only resource %s[%u] !",rsc_manager.Name(target_rsc).data(),target_rsc.id);
			}
			if (!opt.clear)
				curr_input_resources.emplace_back(target_rsc);
			else
				curr_modified_resources.emplace_back(target_rsc);
			curr_output_resources.emplace_back(rsc);
			return rsc;
		}

		void BeginNode()
		{
			curr_input_resources.clear();
			curr_output_resources.clear();
		}
		FrameGraphNode EndNode()
		{
			auto id = NextID();
			for (auto& rsc : curr_output_resources)
			{
				origin_nodes.emplace(rsc.id, id);
			}
			auto input_span = consumed_resources.StoreResources(curr_input_resources);
			auto output_span = consumed_resources.StoreResources(curr_output_resources);
			auto modified_span = consumed_resources.StoreResources(curr_modified_resources);
			curr_input_resources.clear();
			curr_output_resources.clear();
			curr_modified_resources.clear();
			return FrameGraphNode{ id,&consumed_resources.resources,input_span,output_span,modified_span };
		}

		FrameGraphResourceManager rsc_manager;


		NodeBuffer consumed_resources;
		hash_table<fgr_id, fg_id> origin_nodes;

		//Consumed resources
		vector<FrameGraphResource> curr_input_resources;
		vector<FrameGraphResource> curr_output_resources;
		vector<FrameGraphResource> curr_modified_resources;
	};

	namespace FrameGraphDetail
	{
		using Context_t = int;
	}

	struct BaseRenderPass
	{
		//BaseRenderPass(FrameGraphBuilder&,...); //<-- First parameter required, will be supplemented when created.
		virtual void Execute(FrameGraphDetail::Context_t context) = 0;
		virtual ~BaseRenderPass() = default;
	};

	//Job: Do deallocation, reuse resources where possible. (Basically a pool)
	struct TransientResourceManager
	{
		
	};

	template<typename T>
	auto GetSpan(index_span ispan, T& container)
	{
		span<std::remove_reference_t<decltype(*std::data(container))>> sp{ std::data(container) + ispan.begin , std::data(container)+ispan.end};
		return sp;
	}

	struct ResourceLifetime
	{
		fg_id start, end; //Fixed order
		vector<fg_id> inbetween; //Undetermined order
	};
	struct ResourceLifetimeManager
	{
		using lifetime_index = size_t;
		vector< ResourceLifetime> resource_lifetimes;
		hash_table<fgr_id, lifetime_index> map;

		auto NewNode(fgr_id r_id)
		{
			auto index = resource_lifetimes.size();
			resource_lifetimes.emplace_back(ResourceLifetime{ {},{},{} });
			return map.emplace(r_id, index).first;
		}
		auto& GetOrCreate(fgr_id r_id)
		{
			auto itr = map.find(r_id);
			if (itr == map.end())
			{
				itr = NewNode(r_id);
			}
			return resource_lifetimes.at(itr->second);
		}

		void EndLifetime(fgr_id rsc_id, const fg_id& node_id)
		{
			GetOrCreate(rsc_id).end = node_id;
		}
		void InbetweenLifetime(fgr_id rsc_id, const fg_id& node_id)
		{
			GetOrCreate(rsc_id).inbetween.emplace_back(node_id);
		}
		void StartLifetime(fgr_id rsc_id, const fg_id& node_id)
		{
			GetOrCreate(rsc_id).start = node_id;
		}
	};




	struct TempGraph
	{
		hash_table<fgr_id, fg_id> src_node;
		hash_table<fgr_id, fg_id> end_node;
		using fgr_span = index_span;
		hash_table<fg_id, fgr_span> in_nodes;
		const NodeBuffer *buffer;
	};
	struct ActualGraph
	{
		hash_table<fgr_id, fg_id>  src_node;
		hash_table<fgr_id, fg_id>  end_node;
		hash_table<fg_id, span<const FrameGraphResource>> in_nodes;
	};
	ActualGraph ConvertTempGraph(TempGraph&& tmp)
	{
		ActualGraph result{ 
			 .src_node = std::move(tmp.src_node)
			,.end_node = std::move(tmp.end_node)
		};
		for (auto& [id, idx_span] : tmp.in_nodes)
		{
			result.in_nodes.emplace(id, GetSpan(idx_span, tmp.buffer->resources));
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
			graph->in_nodes.emplace(node.id, node.GetInputSpan());
			for (auto&& written_rsc : node.GetOutputSpan())
			{
				graph->src_node.emplace(written_rsc.id,node);
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
	void ComputeLifetimes(ResourceLifetimeManager& manager,const ActualGraph& graph)
	{
		//TODO: Figure out how to construct the entire tree.
		for (auto& [rsc_id, node_id] : graph.src_node)
		{
			manager.StartLifetime(rsc_id, node_id);
		}
		for (auto& [rsc_id, node_id] : graph.end_node)
		{
			manager.EndLifetime(rsc_id, node_id);
		}
		for (auto& [node_id, resources] : graph.in_nodes)
		{
			for (auto& rsc : resources)
			{
				manager.InbetweenLifetime(rsc.id, node_id);
			}
		}
	}
	struct NodeDependencies
	{
		hash_table<fg_id, set<fg_id>> flattened_dependencies;
		hash_table<fg_id, set<fg_id>> reversed_dependencies;
	};
	NodeDependencies BuildDependencyGraph(const vector<FrameGraphNode>& nodes,const ActualGraph& graph)
	{
		NodeDependencies dep;
		for (auto& node : nodes)
		{
			auto node_id = node.id;
			auto in_itr = graph.in_nodes.find(node_id);
			if (in_itr != graph.in_nodes.end())
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
					for(auto reversed_dep : rev_deps)
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
		using Context_t = FrameGraphDetail::Context_t;
		template<typename T>
		using ExecFunc = std::function<void(T&, Context_t context)>;


		template<typename T>//,...Args
		struct CallbackRenderPass : BaseRenderPass
		{
			T value;
			ExecFunc<T> execute;
			template<typename InitFunc,typename ... Args>
			CallbackRenderPass(FrameGraphBuilder& graph_builder,InitFunc&& init,ExecFunc<T>&& func, Args&& ... args) : value{ std::forward<Args>(args)... },execute { std::move(func) }
			{
				init(graph_builder, value);
			}
			void Execute(Context_t context)override
			{
				execute(value, context);
			}
			/*
			void Execute(ContextTuple_t context)override
			{
				execute(value, context);
			}
			*/
		};
		//Downside: Lambdas are probably out of the question unless we can deduce the appropriate std::function to store them
		//template<typename T, typename Func>
		//struct Entry2;
		//template<typename T, typename ...Args>
		//struct Entry2<T,std::function<void(T&,Args...)>> :BaseRenderPass
		//{
		//	
		//	using ExecFunc = std::function<void(T&,Args...)>;
		//	T value;
		//	ExecFunc execute;
		//	Entry(ExecFunc&& func) :execute{ std::move(func)} {}

		//	/*
		//	void Execute(ContextTuple_t context)override
		//	{
		//		execute(value, std::get<Args>(context)...);
		//	}
		//	*/
		//};

		template<typename T,typename ... T_CtorArgs>
		T& addCallBackPass(string_view name,
			const std::function<void(FrameGraphBuilder& builder, T&)>& init,
			ExecFunc<T> execute,
			T_CtorArgs&& ... args
		)
		{
			auto& render_pass = addRenderPass<CallbackRenderPass<T>>(name, init, std::move(execute), std::forward < T_CtorArgs(args)...);
			return render_pass.value;
		}
		template<typename T,typename ...CtorArgs>
		T& addRenderPass(string_view name,CtorArgs&&...args)
		{
			graph_builder.BeginNode();
			auto render_pass = std::make_unique<T>(graph_builder,std::forward <CtorArgs(args)...);
			auto& node =StoreNode(graph_builder.EndNode());
			T& obj = *render_pass;
			render_passes.emplace(node.id,std::move(render_pass));
			return obj;

		}

		FrameGraphNode& StoreNode(FrameGraphNode&& node)
		{
			auto& stored_node = nodes.emplace_back(std::move(node));
			tmp_graph.in_nodes.emplace(node.id, node.input_resources);
			for (auto& written_rsc : node.GetOutputSpan())
				tmp_graph.src_node.emplace(written_rsc.id);
			return stored_node;
		}

		//Process nodes and cull away unnecessary nodes.
		//Figure out dependencies and synchronization points
		//Insert new nodes to help transit concrete resources to their corresponding virtual resources
		//Generate dependency graph
		void Compile()
		{
			auto graph = ConvertTempGraph(std::move(tmp_graph));
			ComputeLifetimes(rsc_lifetime_mgr,graph);

		}

		//using the resultant graph, allocate the concrete resources needed.
		void Allocate(TransientResourceManager& rsc);

		//Execute the renderpasses.
		//Use the dependency graph to split the appropriate jobs into separate threads and sync those.
		void Execute(Context_t context);

		FrameGraphBuilder graph_builder;
		vector<FrameGraphNode> nodes;
		hash_table<fg_id, std::unique_ptr<BaseRenderPass>> render_passes;

		TempGraph tmp_graph;
		ResourceLifetimeManager rsc_lifetime_mgr;
	};


}