#include "pch.h"
#include <core/Core.h>
#include <gfx/DebugRenderer.h>
#include <gfx/Mesh.h>

#include <vulkan/vulkan.hpp>

#include <math/matrix_transforms.inl>

#include <vkn/VulkanState.h>
#include <vkn/VulkanPipeline.h>
#include <vkn/VulkanView.h>
#include <vkn/BufferHelpers.h>
#include <vkn/RenderState.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

#include <util/ioutils.h>

#include <vkn/vector_buffer.h>

#include "VulkanDebugRenderer.h"

#include <math/shapes/ray.h>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <ds/span.inl>
#include <ds/result.inl>

namespace idk::vkn
{
	
	

	const std::vector<vec3>& GetSquareFace(bool is_line_list = false);
	template<typename V, typename F>
	V Transform(const V& v, F func);
	template<typename U, typename V, typename F>
	U Transform(const V& v, F func);


	template<typename T, typename ... Args>
	T concat(const T& t, const Args& ... args);


	const vector<vec3>& GetUnitCube();

	static vector<debug_vertex> ConvertVecToVert(const vector<vec3>& vec);
	const vector<debug_vertex>& GetDebugCube();



	struct vbo
	{
		uint32_t offset{};
		std::vector< debug_vertex> vertices;

		vbo(std::initializer_list<debug_vertex>&& il) :vertices{ il } {}
		vbo(std::vector<debug_vertex>const& verts) :vertices{ verts } {}
		void SetHandle(uint32_t handle) { offset = handle; }
		vk::ArrayProxy<const unsigned char> ToProxy()const
		{
			return hlp::make_array_proxy(hlp::buffer_size<uint32_t>(vertices),
				idk::r_cast<const unsigned char*>(std::data(vertices))
			);
		}
	};
	enum class DbgPipelineType
	{
		eTri,
		eLine
	};
	struct VulkanDebugRenderer::pimpl
	{
		using update_pair_t =std::pair<hlp::vector_buffer*, string_view>;

		struct DrawCallInfo
		{
			hlp::vector_buffer inst_buffer;
			string             inst_data;
			bool valid = false;
		};

		VulkanView& detail;
		VulkanPipeline pipelines[2]{};
		//uniform_info uniforms{};
		hash_table<DbgShape, vbo> shape_buffers{};
		hash_table<DbgShape, vector<debug_instance>> instance_buffers{};

		vk::UniqueSemaphore buffer_ready;

		uint32_t curr_frame = 0;

		//DbgDrawCall        render_buffer[EnumInfo::DbgShapeI::size()];
		debug_draw_calls_t render_buffer;
		hash_table<RscHandle<Mesh>, DrawCallInfo> buffer_data[2];
		buffer_update_info_t update_view;
		hlp::MemoryAllocator allocator;

		pimpl(VulkanView& deets) :detail{ deets }
		, allocator{ *deets.Device(),deets.PDevice() }
		{};
	};


	VulkanDebugRenderer::VulkanDebugRenderer() 
		: vulkan_{ &Core::GetSystem<vkn::VulkanWin32GraphicsSystem>().GetVulkanHandle() }
	{
	}



	void VulkanDebugRenderer::Init()
	{
		info = std::make_shared<debug_info>();
		using binding_info   = buffer_desc::binding_info;
		using attribute_info = buffer_desc::attribute_info;

		idk::pipeline_config config;
		auto vert_data = []()
		{
			//auto stream = Core::GetSystem<FileSystem>().Open("/engine_data/shaders/dbgvertex.vert".spv", FS_PERMISSIONS::READ, true);
			return Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/dbgvertex.vert").value();
		}();
		auto frag_data = []()
		{
			//auto stream = Core::GetSystem<FileSystem>().Open("/engine_data/shaders/dbgfragment.frag.spv", FS_PERMISSIONS::READ, true);
			//return stringify(stream);
			return Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/dbgfragment.frag").value();
		}();

		config.frag_shader = frag_data;
		config.vert_shader = vert_data;
		config.fill_type = idk::FillType::eLine;
		config.prim_top = idk::PrimitiveTopology::eTriangleList;
		//uniform_layout_t uniform_layout{};
		//uniform_layout.bindings.emplace_back(uniform_layout_t::bindings_t{ 0,1,{uniform_layout_t::eVertex} });
		//uniform_layout.bindings.emplace_back(uniform_layout_t::bindings_t{ 1,1,{uniform_layout_t::eVertex} });
		//config.uniform_layouts.emplace(0, uniform_layout);
		config.buffer_descriptions.emplace_back(
			buffer_desc{
				binding_info{0,sizeof(idk::debug_vertex),idk::VertexRate::eVertex},
				{
					attribute_info{ idk::AttribFormat::eSVec3,0,0 }
				}
			});
		config.buffer_descriptions.emplace_back(
			buffer_desc{
				binding_info{1,sizeof(idk::debug_instance),idk::VertexRate::eInstance},
				{
					 attribute_info{ idk::AttribFormat::eSVec3,0, offsetof(debug_info::inst_data,col) }
					,attribute_info{ idk::AttribFormat::eMat4 ,1, offsetof(debug_info::inst_data,transform) }
				}
			});
		config.render_pass_type = BasicRenderPasses::eRgbaColorDepth;
		auto line_copy = config;
		line_copy.prim_top = idk::PrimitiveTopology::eLineList;
		Init(config,line_copy);
	}

	void VulkanDebugRenderer::Init(const idk::pipeline_config& tri_config, const idk::pipeline_config& line_config)//, const idk::uniform_info& uniform_info)
	{
		auto& system = *vulkan_;
		impl = std::make_unique<pimpl>(system.View());
		impl->pipelines[(int)DbgPipelineType::eTri].Create(tri_config, impl->detail);
		impl->pipelines[(int)DbgPipelineType::eLine].Create(line_config, impl->detail);

		impl->buffer_ready = system.View().Device()->createSemaphoreUnique(vk::SemaphoreCreateInfo{ vk::SemaphoreCreateFlags{} });

		impl->shape_buffers = idk::hash_table<DbgShape, vbo>
		{
			{DbgShape::eCube   ,
				{
					GetDebugCube()
				}
			}
			,
			{DbgShape::eSquare,
				{
					ConvertVecToVert(GetSquareFace())
					//vec3{-0.5f,-0.5f,0.0f},
					//vec3{-0.5f, 0.5f,0.0f},
					//vec3{ 0.5f, 0.5f,0.0f},
					//
					//vec3{ 0.5f, 0.5f,0.0f},
					//vec3{ 0.5f,-0.5f,0.0f},
					//vec3{-0.5f,-0.5f,0.0f},
				}
			}
			,
			{DbgShape::eEqTriangle,
				{
			/* //Line List
			vec3{-0.5f,-0.5f,0.0f},
			vec3{-0.0f, 0.5f,0.0f},

			vec3{-0.0f, 0.5f,0.0f},
			vec3{ 0.5f, 0.5f,0.0f},

			vec3{ 0.5f, 0.5f,0.0f},
			vec3{-0.5f,-0.5f,0.0f},
			/*/ //TriangleList         
			vec3{-0.5f, 0.5f,0.0f},
			vec3{ 0.0f, 0.5f - (sqrt(3.0f / 2.0f)),0.0f},
			vec3{ 0.5f, 0.5f,0.0f},
			//  */
			//vec3{ 0.5f, 0.5f,0.0f},
			//vec3{ 0.5f,-0.5f,0.0f},
			//vec3{-0.5f,-0.5f,0.0f},
		}
	}
		};
	}

	void VulkanDebugRenderer::Shutdown()
	{
		//Wait for the current instructions to be completed. (Makes sure that all our rscs are no longer in use
		impl->detail.WaitDeviceIdle();
		this->info.reset();
		this->impl.reset();
	}


	const VulkanDebugRenderer::buffer_update_info_t &VulkanDebugRenderer::BufferUpdateInfo()
	{
		return impl->update_view;
	}

	const VulkanDebugRenderer::debug_draw_calls_t &VulkanDebugRenderer::DbgDrawCalls()const
	{
		return impl->render_buffer;
	}

	const VulkanPipeline& VulkanDebugRenderer::GetPipeline() const
	{
		return impl->pipelines[0];
	}
	//RscHandle<Mesh> ShapeToMesh(DbgShape shape)
	//{
	//	return Mesh::defaults[MeshType::Box];
	//}

	void VulkanDebugRenderer::GrabDebugBuffer()
	{
		impl->curr_frame = (impl->curr_frame +1)% 2;
		info->render_info.clear();
		info->render_info2.clear();
		impl->update_view.clear();
		for (auto& [mesh,dc] : impl->render_buffer)
		{
			dc.~DbgDrawCall();
			new (&dc) DbgDrawCall();
		}
		//impl->vert_buffer.clear();
		//impl->inst_buffer.clear();
		for (auto& elem : Core::GetSystem<DebugRenderer>().GetWorldDebugInfo())
		{
			//if (elem.mesh == Mesh::defaults[MeshType::Box])
			{
				DrawShape(elem.mesh, elem.transform, elem.color);
			}
		}
		//auto ucmd_buffer = hlp::BeginSingleTimeCBufferCmd(*impl->detail.Device(), *impl->detail.Commandpool());
		//auto cmd_buffer = *ucmd_buffer;
		for (auto& [mesh, inst_data] : info->render_info2)
		{
			//auto&& shape_buffer = impl->shape_buffers.find(shape)->second.vertices;
			//auto&& shape_buffer_proxy = impl->shape_buffers.find(shape)->second.ToProxy();

			//const auto shape_index = EnumInfo::DbgShapeI::map(shape);

			auto& dcall = impl->render_buffer[mesh];
			//RscHandle<Mesh> mesh = ShapeToMesh(shape);

			dcall.pipeline = &impl->pipelines[(int)DbgPipelineType::eLine];
			//Bind vtx buffers
			auto& buffer_data = impl->buffer_data[impl->curr_frame][mesh];
			auto& inst_v_buffer = buffer_data.inst_buffer;
			auto& data = buffer_data.inst_data = std::string{ r_cast<const char*>(inst_data.data()),hlp::buffer_size(inst_data) };
			auto num_inst_bytes = hlp::buffer_size<uint32_t>(data);
			inst_v_buffer.resize(num_inst_bytes);
			impl->update_view.emplace_back(std::make_pair(&inst_v_buffer,string_view{r_cast<const char*>(data.data()),hlp::buffer_size(data)} ));
			//inst_v_buffer.update<debug_info::inst_data>(0, data, cmd_buffer);

			dcall.num_instances = hlp::arr_count(inst_data);
			dcall.RegisterBuffer(DbgBufferType::ePerInst, dbg_vert_layout::instance_binding, buffer_info{ inst_v_buffer.buffer(),0 });
			auto& pos_buffer = mesh.as<VulkanMesh>().Get(attrib_index::Position);
			auto& idx_buffer = *mesh.as<VulkanMesh>().GetIndexBuffer();
			dcall.RegisterBuffer(DbgBufferType::ePerVtx, dbg_vert_layout::vertex_binding, buffer_info{ *pos_buffer.buffer(),s_cast<uint32_t>(pos_buffer.offset)});
			dcall.index_buffer = buffer_info{*idx_buffer.buffer(),s_cast<uint32_t>(idx_buffer.offset)};
			dcall.num_indices = mesh.as<VulkanMesh>().IndexCount();
/*

			size_t num_inst_chunk = impl->inst_buffer.chunk_size()/sizeof(buffer[0]);
			size_t num_elems= buffer.size();
			uint32_t inst_binding = dbg_vert_layout::instance_binding;
			uint32_t vert_binding = dbg_vert_layout::vertex_binding;
	        
			size_t num_iterations = num_elems / num_inst_chunk + ((num_elems%num_inst_chunk)?1:0);
			auto& vbuffer = impl->vert_buffer;
			auto [vertex_buffer, vb_offset] = vbuffer.new_chunk(s_cast<const void*>(std::data(shape_buffer_proxy)), hlp::buffer_size<uint32_t>(shape_buffer_proxy));
			auto& inst_buffer = impl->inst_buffer;
			for (auto i = num_iterations; i-- > 0;)
			{
				
				auto& dcall = impl->render_buffer.emplace_back();
				dcall.num_instances = static_cast<uint32_t>(std::min(num_inst_chunk,hlp::buffer_size(buffer) - (num_inst_chunk * i)));
				auto [instance_buffer, ib_offset]
					= inst_buffer.new_chunk(
						std::data(buffer)+num_inst_chunk*i,
						dcall.num_instances
					);
				dcall.num_vertices = hlp::arr_count(shape_buffer);
				dcall.RegisterBuffer(DbgBufferType::ePerInst, inst_binding, buffer_info{ instance_buffer,ib_offset });
				dcall.RegisterBuffer(DbgBufferType::ePerVtx, vert_binding, buffer_info{ vertex_buffer,vb_offset });
			}			*/
		}
		//hlp::EndSingleTimeCbufferCmd(cmd_buffer, impl->detail.GraphicsQueue(), false, {}, {}, *impl->buffer_ready);
		//impl->inst_buffer.update_buffers();
		//impl->vert_buffer.update_buffers();
	}

	VulkanDebugRenderer::~VulkanDebugRenderer() = default;

	void VulkanDebugRenderer::DrawShape(DbgShape shape, const mat4& tfm, const color& color)
	{
		this->info->render_info[shape].emplace_back(debug_info::inst_data{ color, tfm });
	}
	void VulkanDebugRenderer::DrawShape(MeshType shape, const mat4& tfm, const color& color)
	{
		
		this->info->render_info2[Mesh::defaults[shape]].emplace_back(debug_info::inst_data{ color, tfm });
	}
	void VulkanDebugRenderer::DrawShape(RscHandle<Mesh> shape, const mat4& tfm, const color& color)
	{
		this->info->render_info2[shape].emplace_back(debug_info::inst_data{ color, tfm });

	}
	void VulkanDebugRenderer::Draw(const ray& ray, const color& color)
	{
		const auto line_tfm = look_at(ray.origin + ray.velocity / 2, ray.origin + ray.velocity, vec3{ 0,1,0 }) * mat4 { scale(vec3{ ray.velocity.length() / 2 }) };
		DrawShape(MeshType::Line, line_tfm, color);

		const auto orient_tfm = orient(ray.velocity.get_normalized());
		const auto arrow_tfm = translate(ray.origin + ray.velocity) * mat4 { orient_tfm * scale(vec3{ 0.025f }) };
		DrawShape(MeshType::DbgArrow, arrow_tfm, color);
	}
	void VulkanDebugRenderer::Draw(const sphere& sphere, const color& color)
	{
		const mat4 tfm = translate(sphere.center) * mat4 { scale(vec3{ sphere.radius * 2 }) };
		DrawShape(MeshType::Sphere, tfm, color);
	}
	void VulkanDebugRenderer::Draw(const box& box, const color& color)
	{
		const mat4 tfm = translate(box.center) * mat4 { box.axes() } *scale(box.extents);
		DrawShape(MeshType::Box, tfm, color);
	}
	void VulkanDebugRenderer::Draw(const aabb& box, const color& color)
	{
		const mat4 tfm = translate(box.center()) * mat4 { scale(box.extents()) };
		DrawShape(MeshType::Box, tfm, color);
	}
	const std::vector<vec3>& GetSquareFace(bool is_line_list)
	{
		static constexpr float a = 1.0f;
		static std::vector<vec3> triangle_list{
			vec3{ -a,-a,0.0f },
			vec3{ -a, a,0.0f },
			vec3{  a, a,0.0f },
			vec3{  a, a,0.0f },
			vec3{  a,-a,0.0f },
			vec3{ -a,-a,0.0f },
		};

		static std::vector<vec3> line_list{
			//line list

				vec3{ -a,-a,0.0f },
				vec3{ -a, a,0.0f },
				vec3{ -a, a,0.0f },
				vec3{  a, a,0.0f },

				vec3{  a, a,0.0f },
				vec3{  a,-a,0.0f },
				vec3{  a,-a,0.0f },
				vec3{ -a,-a,0.0f },
				//*/
		};
		return (is_line_list) ? line_list : triangle_list;
	}
	template<typename V, typename F>
	V Transform(const V& v, F func)
	{
		V result{ v };
		for (auto& r : result)
		{
			r = func(r);
		}
		return result;
	}
	template<typename U, typename V, typename F>
	U Transform(const V& v, F func)
	{
		U result{};
		std::transform(std::begin(v), std::end(v), std::inserter(result, std::end(result)), func);
		return result;
	}

	namespace detail
	{
		//Base case 
		template<typename T, typename ... Args>
		void concat(T&, const Args& ...) {}
		//General case
		template<typename T, typename U, typename ... Args>
		void concat(T& result, const U& t, const Args& ... args)
		{
			std::copy(t.begin(), t.end(), std::back_inserter(result));
			concat(result, args...);
		}
	}

	template<typename T, typename ... Args>
	T concat(const T& t, const Args& ... args)
	{
		T result;
		detail::concat(result, t, args...);
		return result;
	}


	const vector<vec3>& GetUnitCube()
	{
		static vector<vec3> vertices
		{
			concat(
			Transform(GetSquareFace(),[](const vec3& v) {
					mat4 r{ rotate(vec3{0,1.0f,0},rad{pi * 0.0f   }) };
					return r * (translate(vec3{0,0,0.5f}) * vec4 { v,1.0f }); }),
			Transform(GetSquareFace(),[](const vec3& v) { return mat4{rotate(vec3{0,1.0f,0},rad{pi * 0.5f})} *(translate(vec3{0,0,0.5f}) * vec4 { v,1.0f }); }),
			Transform(GetSquareFace(),[](const vec3& v) { return mat4{rotate(vec3{0,1.0f,0},rad{pi * 1.0f})} *(translate(vec3{0,0,0.5f}) * vec4 { v,1.0f }); }),
			Transform(GetSquareFace(),[](const vec3& v) { return mat4{rotate(vec3{0,1.0f,0},rad{pi * 1.5f})} *(translate(vec3{0,0,0.5f}) * vec4 { v,1.0f }); }),
			Transform(GetSquareFace(),[](const vec3& v) { return mat4{rotate(vec3{1.0f,0,0},rad{pi * 0.5f})} *(translate(vec3{0,0,0.5f}) * vec4 { v,1.0f }); }),
			Transform(GetSquareFace(),[](const vec3& v) { return mat4{rotate(vec3{1.0f,0,0},rad{pi * 1.5f})} *(translate(vec3{0,0,0.5f}) * vec4 { v,1.0f }); })
			)
		};
		return vertices;
	}


	static vector<debug_vertex> ConvertVecToVert(const vector<vec3>& vec)
	{
		vector<debug_vertex> result;
		for (auto& v : vec)
		{
			result.emplace_back(v);
		}
		return result;
	}
	const vector<debug_vertex>& GetDebugCube()
	{
		static const auto data = ConvertVecToVert(GetUnitCube());
		return data;
	}
}