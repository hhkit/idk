#include "pch.h"
#include <core/Core.h>
#include <gfx/DebugRenderer.h>
#include <gfx/Mesh.h>

#include <vulkan/vulkan.hpp>

#include <math/matrix_transforms.h>

#include <vkn/VulkanState.h>
#include <vkn/VulkanPipeline.h>
#include <vkn/VulkanView.h>
#include <vkn/BufferHelpers.h>
#include <vkn/RenderState.h>
#include <vkn/VulkanWin32GraphicsSystem.h>

#include <util/ioutils.h>

#include <vkn/vector_buffer.h>

#include "VulkanDebugRenderer.h"
namespace idk::vkn
{
	namespace glm
	{
		mat4 lookAt(vec3 const& eye, vec3 const& center, vec3 const& up);
	}

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

	struct VulkanDebugRenderer::pimpl
	{
		VulkanView& detail;
		VulkanPipeline pipeline{};
		//uniform_info uniforms{};
		hash_table<DbgShape, vbo> shape_buffers{};
		hash_table<DbgShape, vector<debug_instance>> instance_buffers{};

		vector<DbgDrawCall> render_buffer;

		hlp::MemoryAllocator allocator;
		hlp::bbucket_list vert_buffer;
		hlp::bbucket_list inst_buffer;

		pimpl(VulkanView& deets) :detail{ deets }
		, allocator{ *deets.Device(),deets.PDevice() }
		, vert_buffer{ deets.PDevice(),*deets.Device(), allocator }
		, inst_buffer{ deets.PDevice(),*deets.Device(), allocator }

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
			return Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/dbgvertex.vert.spv").value();
		}();
		auto frag_data = []()
		{
			//auto stream = Core::GetSystem<FileSystem>().Open("/engine_data/shaders/dbgfragment.frag.spv", FS_PERMISSIONS::READ, true);
			//return stringify(stream);
			return Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/dbgfragment.frag.spv").value();
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
					 attribute_info{ idk::AttribFormat::eSVec3,0, offsetof(idk::debug_instance,color) }
					,attribute_info{ idk::AttribFormat::eMat4 ,1, offsetof(idk::debug_instance,model) }
				}
			});
		config.render_pass_type = BasicRenderPasses::eRgbaColorDepth;
		Init(config);
	}

	void VulkanDebugRenderer::Init(const idk::pipeline_config& config)//, const idk::uniform_info& uniform_info)
	{
		auto& system = *vulkan_;
		impl = std::make_unique<pimpl>(system.View());
		impl->pipeline.Create(config, impl->detail);

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

	void VulkanDebugRenderer::Render(const mat4& view, const mat4& proj, GraphicsState& )
	{
		auto& detail = impl->detail;
		auto& pipeline = impl->pipeline;
		draw_call dc;
		dc.pipeline = &pipeline;
		auto extent = detail.Swapchain().extent;
		dc.uniforms.emplace_back(0, 0, view);
		dc.uniforms.emplace_back(0, 1, proj);
		for (auto& [shape, buffer] : info->render_info)
		{
			auto&& shape_buffer = impl->shape_buffers.find(shape)->second.vertices;
			auto&& shape_buffer_proxy = impl->shape_buffers.find(shape)->second.ToProxy();

			//Bind vtx buffers
			auto instance_buffer = detail.AddToMasterBuffer(std::data(buffer), hlp::buffer_size<uint32_t>(buffer));
			auto vertex_buffer = detail.AddToMasterBuffer(s_cast<const void*>(std::data(shape_buffer_proxy)), hlp::buffer_size<uint32_t>(shape_buffer_proxy));
			dc.instance_count = hlp::arr_count(buffer);
			dc.vertex_count = hlp::arr_count(shape_buffer);
			dc.vtx_binding.emplace_back(dbg_vert_layout::vertex_binding, vertex_buffer);
			dc.vtx_binding.emplace_back(dbg_vert_layout::instance_binding, instance_buffer);

			detail.CurrRenderState().AddDrawCall(dc);
		}
		info->render_info.clear();
	}

	const vector<DbgDrawCall>& VulkanDebugRenderer::DbgDrawCalls() const
	{
		// TODO: insert return statement here
		return impl->render_buffer;
	}

	const VulkanPipeline& VulkanDebugRenderer::GetPipeline() const
	{
		// TODO: insert return statement here
		return impl->pipeline;
	}

	void VulkanDebugRenderer::GrabDebugBuffer()
	{
		info->render_info.clear();
		impl->render_buffer.clear();
		impl->vert_buffer.clear();
		impl->inst_buffer.clear();
		for (auto& elem : Core::GetSystem<DebugRenderer>().GetWorldDebugInfo())
		{
			//if (elem.mesh == Mesh::defaults[MeshType::Box])
			{
				DrawShape(DbgShape::eCube, elem.transform, elem.color);
			}
		}
		for (auto& [shape, buffer] : info->render_info)
		{
			auto&& shape_buffer = impl->shape_buffers.find(shape)->second.vertices;
			auto&& shape_buffer_proxy = impl->shape_buffers.find(shape)->second.ToProxy();

			//Bind vtx buffers
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
			}			
		}
		impl->inst_buffer.update_buffers();
		impl->vert_buffer.update_buffers();
	}

	VulkanDebugRenderer::~VulkanDebugRenderer() = default;

	void VulkanDebugRenderer::DrawShape(DbgShape shape, const mat4& tfm, const color& color)
	{
		this->info->render_info[shape].emplace_back(debug_info::inst_data{ color, tfm });
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
					mat3 r3 = rotate(vec3{0,1.0f,0},rad{pi * 0.0f   });
					mat4 r{r3};
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