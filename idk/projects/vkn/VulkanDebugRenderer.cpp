#include "pch.h"
#include <vulkan/vulkan.hpp>

#include <math/matrix_transforms.h>

#include <vkn/VulkanState.h>
#include <vkn/VulkanPipeline.h>
#include <vkn/VulkanView.h>
#include <vkn/BufferHelpers.h>
#include <vkn/RenderState.h>

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
		uniform_info uniforms{};
		hash_table<DbgShape, vbo> shape_buffers{};
		hash_table<DbgShape, vector<debug_instance>> instance_buffers{};

		pimpl(VulkanView& deets) :detail{ deets } {};
	};

	
	VulkanDebugRenderer::VulkanDebugRenderer(VulkanState& vulkan):vulkan_{&vulkan}
	{
	}



	void VulkanDebugRenderer::Init(const idk::pipeline_config& config, const idk::uniform_info& uniform_info)
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
					vec3{ 0.0f, 0.5f-(sqrt(3.0f/2.0f)),0.0f},
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

	void VulkanDebugRenderer::Render()
	{
		auto& cmd_buffer = *impl->detail.CurrCommandbuffer();
		auto& detail   = impl->detail  ;
		auto& pipeline = impl->pipeline;
		auto& uniforms = impl->uniforms;
		draw_call dc;
		dc.pipeline = &pipeline;
		dc.uniform_info = uniforms;
		mat4 view = glm::lookAt(vec3{ 0,2,2 }, vec3{ 0,0,0 }, vec3{ 0,1,0 });
		auto extent = detail.Swapchain().extent;
		mat4 proj = perspective(idk::rad(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
		dc.uniforms.emplace_back(0, 0, view);
		dc.uniforms.emplace_back(0, 1, proj);
		for (auto& [shape, buffer] : info->render_info)
		{
			auto&& shape_buffer = impl->shape_buffers.find(shape)->second.vertices;
			auto&& shape_buffer_proxy = impl->shape_buffers.find(shape)->second.ToProxy();

			//Bind vtx buffers
			auto instance_buffer = detail.AddToMasterBuffer(std::data(buffer), hlp::buffer_size<uint32_t>(buffer));
			auto vertex_buffer   = detail.AddToMasterBuffer(idk::s_cast<const void*>(std::data(shape_buffer_proxy)), hlp::buffer_size<uint32_t>(shape_buffer_proxy));
			dc.instance_count = hlp::arr_count(buffer);
			dc.vertex_count   = hlp::arr_count(shape_buffer);
			dc.vtx_binding.emplace_back(dbg_vert_layout::vertex_binding, vertex_buffer);
			dc.vtx_binding.emplace_back(dbg_vert_layout::instance_binding, instance_buffer);

			//Bind idx buffers
			//auto  index_buffer = detail.AddToMasterBuffer();
			//cmd_buffer.bindIndexBuffer(detail.CurrMasterVtxBuffer(),)

			//Bind uniform buffer

			//Draw
			//cmd_buffer.draw(ArrCount(shape_buffer), ArrCount(buffer), 0, 0, detail.Dispatcher());
			detail.CurrRenderState().AddDrawCall(dc);
		}	
		info->render_info.clear();
	}

	VulkanDebugRenderer::~VulkanDebugRenderer() = default;




	const std::vector<vec3>& GetSquareFace(bool is_line_list )
	{
		//vec3{-0.5f,-0.5f,0.0f},
		//vec3{-0.5f, 0.5f,0.0f},
		//vec3{ 0.5f, 0.5f,0.0f},
		//
		//vec3{ 0.5f, 0.5f,0.0f},
		//vec3{ 0.5f,-0.5f,0.0f},
		//vec3{-0.5f,-0.5f,0.0f},
		static std::vector<vec3> triangle_list{
			vec3{ -0.5f,-0.5f,0.0f },
			vec3{ -0.5f, 0.5f,0.0f },
			vec3{  0.5f, 0.5f,0.0f },
			vec3{  0.5f, 0.5f,0.0f },
			vec3{  0.5f,-0.5f,0.0f },
			vec3{ -0.5f,-0.5f,0.0f },
		};
		static std::vector<vec3> line_list{
			//line list

				vec3{ -0.5f,-0.5f,0.0f },
				vec3{ -0.5f, 0.5f,0.0f },
				vec3{ -0.5f, 0.5f,0.0f },
				vec3{  0.5f, 0.5f,0.0f },

				vec3{  0.5f, 0.5f,0.0f },
				vec3{  0.5f,-0.5f,0.0f },
				vec3{  0.5f,-0.5f,0.0f },
				vec3{ -0.5f,-0.5f,0.0f },
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