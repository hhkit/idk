#pragma once
#include <cstdint>
#include <gfx/pipeline_config.h>
#include <vkn/utils/Flags.h>
#include <ds/ranged_for.inl>
namespace idk::vkn
{
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
	using fgr_id = uint32_t;
	struct FrameGraphResource 
	{
		fgr_id id;
		bool readonly = true;
	};
	using FrameGraphResourceReadOnly = FrameGraphResource;
	using FrameGraphResourceMutable  = FrameGraphResource;
	using fg_id = uint32_t;
	struct FrameGraphNode 
	{
		fg_id id;
		
		const vector<FrameGraphResource>* buffer;

		index_span input_resources;
		index_span output_resources;

		bool Reads(FrameGraphResource rsc)const
		{
			auto span = input_resources;
			auto begin = buffer->begin() + span.begin;
			auto end = buffer->begin() + span.end;
			auto itr = std::find(begin, end, rsc);
			return itr != end;
		}
		bool Writes(FrameGraphResource rsc)const 
		{
			auto span = output_resources;
			auto begin = buffer->begin() + span.begin;
			auto end = buffer->begin() + span.end;
			auto itr = std::find(begin, end, rsc);
			return itr != end;
		}

	};

	struct FrameGraphResourceManager
	{
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
			return FrameGraphResource{ next_id };
		}
		string_view Name(FrameGraphResource fg)const
		{
			auto itr = resource_handles.find(fg.id);
			if (itr == resource_handles.end())
				return "";
			return resources.at(itr->second).name;
		}
		//Generate the next id.
		fgr_id NextID();

		using rsc_index_t = size_t;
		vector<AttachmentDescription> resources;
		hash_table<fgr_id, rsc_index_t> resource_handles;
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
		FrameGraphResource write(FrameGraphResourceMutable target_rsc)
		{
			auto rsc = rsc_manager.Rename(target_rsc);
			if (target_rsc.readonly)
			{
				LOG_TO(LogPool::GFX, "Attempting to write to Read only resource %s[%u] !",rsc_manager.Name(target_rsc).data(),target_rsc.id);
			}
			curr_input_resources.emplace_back(target_rsc);
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
			auto input_span = consumed_resources.StoreResources(curr_input_resources);
			auto output_span = consumed_resources.StoreResources(curr_output_resources);
			curr_input_resources.clear();
			curr_output_resources.clear();
			return FrameGraphNode{ NextID(),&consumed_resources.resources,input_span,output_span };
		}

		FrameGraphResourceManager rsc_manager;


		NodeBuffer consumed_resources;

		//Consumed resources
		vector<FrameGraphResource> curr_input_resources;
		vector<FrameGraphResource> curr_output_resources;
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
		span<decltype(*std::data(container))> sp{ std::data(container) + ispan.begin , std::data(container)+ispan.end};
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

		void EndLifetime(FrameGraphResource rsc,const FrameGraphNode& node)
		{
			GetOrCreate(rsc.id).end = node.id;
		}
		void InbetweenLifetime(FrameGraphResource rsc, const FrameGraphNode& node)
		{
			GetOrCreate(rsc.id).inbetween.emplace_back(node.id);
		}
		void StartLifetime(FrameGraphResource rsc, const FrameGraphNode& node)
		{
			GetOrCreate(rsc.id).start = node.id;
		}
	};

	void FindLifetime(ResourceLifetimeManager& manager,const vector<FrameGraphNode>& nodes, const NodeBuffer& rscs, const FrameGraphNode& root)
	{
		auto input_span = GetSpan(root.input_resources  , rscs.resources);
		auto output_span = GetSpan(root.output_resources, rscs.resources);
		//TODO: Figure out how to construct the entire tree.
		for (auto& input : input_span)
		{
			manager.EndLifetime(input,root);
			for (auto& node : reverse(nodes))
			{
				if (node.Reads(input))
				{
					manager.InbetweenLifetime(input, node);
				}
				else if (node.Writes(input))
				{
					manager.EndLifetime(input, node);
					break;
				}
			}
		}



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
			nodes.emplace_back(graph_builder.EndNode());
			T& obj = *render_pass;
			render_passes.emplace_back(std::move(render_pass));
			return obj;

		}

		//Process nodes and cull away unnecessary nodes.
		//Figure out dependencies and synchronization points
		//Insert new nodes to help transit concrete resources to their corresponding virtual resources
		//Generate dependency graph
		void Compile(); 

		//using the resultant graph, allocate the concrete resources needed.
		void Allocate(TransientResourceManager& rsc);

		//Execute the renderpasses.
		//Use the dependency graph to split the appropriate jobs into separate threads and sync those.
		void Execute(Context_t context);

		FrameGraphBuilder graph_builder;
		vector<FrameGraphNode> nodes;
		vector<std::unique_ptr<BaseRenderPass>> render_passes;
	};


}