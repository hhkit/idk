#include "pch.h"

#include <set>
#include <map>

#include <idk.h>
#include <math/matrix_transforms.h>
#include <gfx/buffer_desc.h>

#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
#include <vkn/BufferHelpers.h>
#include <vkn/RenderState.h>
#include <vkn/utils/utils.h>
#include <vkn/UboManager.h>

namespace idk
{
	template<typename T>
	using set = std::set<T>;
	template<typename Key,typename Val>
	using map = std::map<Key,Val>;
}

//Uncomment to disable validation
#define WX_VAL_VULK

//Uncomment this when the temporary glm namespace glm{...} below has been removed.
//namespace glm = idk;
//Temporary, should move elsewhere
#include <cstdint>
#include <map>



namespace idk::vkn
{
	struct ProcessedDrawCalls
	{
		struct BindingInfo
		{
			uint32_t binding;
			vk::Buffer ubuffer;
			uint32_t buffer_offset;
			uint32_t arr_index;
			size_t size;
			BindingInfo(
				uint32_t binding_,
				vk::Buffer& ubuffer_,
				uint32_t buffer_offset_,
				uint32_t arr_index_,
				size_t size_
			) :
				binding{ binding_ },
				ubuffer{ ubuffer_ },
				buffer_offset{ buffer_offset_ },
				arr_index{ arr_index_ },
				size{ size_ }
			{
			}
		};
		struct DrawCall
		{
			const draw_call* p_dc;
			//set, update_instr
			hash_table<uint32_t, vector<BindingInfo>> bindings;
		};
		vector<DrawCall> draw_calls;
		void Add(DrawCall&& pdc)
		{
			draw_calls.emplace_back(std::move(pdc));
		}
		decltype(draw_calls)::const_iterator begin()const
		{
			return draw_calls.cbegin();
		}
		decltype(draw_calls)::const_iterator end()const
		{
			return draw_calls.cend();
		}
	};

	namespace glm
	{
		using namespace idk;
		mat4 lookAt(vec3 const& eye, vec3 const& center, vec3 const& up)
		{

			vec3 const f((center - eye).get_normalized());
			vec3 const s((f.cross(up)).get_normalized());
			vec3 const u(s.cross(f));

			mat4 Result{};
			Result[0][0] = s.x;
			Result[1][0] = s.y;
			Result[2][0] = s.z;
			Result[0][1] = u.x;
			Result[1][1] = u.y;
			Result[2][1] = u.z;
			Result[0][2] = -f.x;
			Result[1][2] = -f.y;
			Result[2][2] = -f.z;
			Result[3][0] = -dot(s, eye);
			Result[3][1] = -dot(u, eye);
			Result[3][2] = dot(f, eye);
			return Result;
		}
	}


	enum   BufferType
	{
		eVertexBuffer
		, eIndexBuffer
	};
	struct buffer_data
	{
		using num_obj_t = size_t;
		using num_bytes_t = size_t;
		const void* buffer = nullptr;
		num_bytes_t    len = 0;
		template<typename T>
		buffer_data(const T& container)
			: buffer{ s_cast<const void*>(std::data(container)) }, len{ buffer_size(container) }{}
		template<typename T>
		buffer_data(const T& container, num_obj_t offset, num_obj_t length)
			: buffer{ s_cast<const void*>(std::data(container) + offset) }, len{ vhlp::buffer_size(std::data(container) + offset,std::data(container) + length) }{}

	};
	enum ShaderStage
	{
		eVert,
		eFrag,
		eGeom,//unused for now
		eTess,//unused for now
		eComp,//unused for now
		eNone
	};
	struct shader_info
	{
		string_view byte_code{     };
		ShaderStage stage{ eNone };
	};

	struct IPipeline;

	enum RenderType
	{
		eDraw
		, eIndexed
	};
	struct render_info
	{
		vector<buffer_data>* vertex_buffers = nullptr;
		RenderType           render_type = eDraw;
		uint32_t             inst_count = 1;
	};


	class GfxInterface
	{
	public:
		using pipeline_handle = std::weak_ptr<IPipeline>;
		//Basic version, probably change this later on to use a single config struct instead.
		pipeline_handle RegisterPipeline(const std::vector<buffer_desc>& descriptors, std::vector<shader_info> shaders);
		void QueueForRendering(pipeline_handle pipeline, const render_info& info);

	};




	struct IPipeline
	{
		virtual void Create() = 0;
		virtual void RegisterCommands() = 0;
		void Draw() {}
	};


	//template<typename RT = size_t, typename T = int>
	//RT buffer_size(std::vector<T> const& vertices)
	//{
	//	return static_cast<RT>(sizeof(vertices[0]) * vertices.size());
	//}
	class DbgVertexBuffer
	{
	public:
		DbgVertexBuffer(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes);
		void Resize(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes, bool force_downsize = false);
		template<typename T>
		void Update(vk::DeviceSize offset, vk::ArrayProxy<T> arr, vk::CommandBuffer& buffer)
		{
			Update(offset, hlp::buffer_size(arr), buffer, reinterpret_cast<unsigned char const*>(arr.data()));
		}
		void Update(vk::DeviceSize offset, vk::DeviceSize len, vk::CommandBuffer& buffer, unsigned char const* data);
		vk::Buffer       Buffer() const { return *buffer; }
	private:
		vk::UniqueDeviceMemory memory;
		vk::UniqueBuffer       buffer;

		size_t capacity = 0;

	};



	std::unique_ptr<DbgVertexBuffer> dbg_vertex_buffer;


	struct UniformBufferObject
	{
		mat4 model, view, projection;
	};

	std::unordered_set<uint32_t> VulkanState::QueueFamilyIndices::unique_queues()const
	{
		return
		{
			*graphics_family,
			*present_family ,
			//		*transfer_family
		};
	}


	//using vec2 = glm::vec2;
	//using vec3 = glm::vec3;
	//using vec4 = glm::vec4;

	constexpr auto vkeFormatVec2 = vk::Format::eR32G32Sfloat;
	constexpr auto vkeFormatVec3 = vk::Format::eR32G32B32Sfloat;
	constexpr auto vkeFormatVec4 = vk::Format::eR32G32B32A32Sfloat;

	uint32_t calc_attr_offset(uint32_t data_size)
	{
		return (data_size - 1) / sizeof(vec4) + 1;
	}


	struct dbg_vertex
	{
		using vert_t = dbg_vertex;
		vec2 pos;

		static vk::VertexInputBindingDescription getBindingDescription()
		{
			vk::VertexInputBindingDescription binding_description{};
			binding_description.binding = 0;
			binding_description.stride = sizeof(vert_t);
			binding_description.inputRate = vk::VertexInputRate::eVertex;
			return binding_description;
		}
		static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
		{
			std::vector<vk::VertexInputAttributeDescription> attribute_descriptions(1);

			attribute_descriptions[0].binding = 0;
			attribute_descriptions[0].location = 0;
			attribute_descriptions[0].format = vkeFormatVec2;
			attribute_descriptions[0].offset = static_cast<uint32_t>(offsetof(vert_t, pos));

			return attribute_descriptions;
		}
	};
	struct dbg_instance
	{
		using vert_t = dbg_instance;
		alignas(16) mat4 model;
		vec4 col;
		static vk::VertexInputBindingDescription getBindingDescription()
		{
			return vk::VertexInputBindingDescription
			{
				1,							   //binding   
				sizeof(vert_t),      //stride    
				vk::VertexInputRate::eInstance,//inputRate 
			};
		}
		static std::pair < std::vector<vk::VertexInputAttributeDescription>, uint32_t > getAttributeDescriptions(uint32_t loc = 0)
		{
			constexpr auto sz = 5;
			std::vector<vk::VertexInputAttributeDescription> attribute_descriptions{};

			for (int i = 0; i < 4; ++i)
			{
				attribute_descriptions.emplace_back(
					loc,															   //location 
					1,																   //binding  
					vkeFormatVec4,													   //format   
					static_cast<uint32_t>(offsetof(vert_t, model) + i * sizeof(vec4))//offset   
				);
				loc += calc_attr_offset(sizeof(vec4));
			}
			attribute_descriptions.emplace_back(
				loc,															   //location 
				1,																   //binding  
				vkeFormatVec4,													   //format   
				static_cast<uint32_t>(offsetof(vert_t, col))                       //offset   
			);
			loc += calc_attr_offset(sizeof(vec4));


			return std::make_pair(attribute_descriptions, loc);
		}
	};


	struct vertex
	{
		vec2 pos;
		vec3 col;

		static vk::VertexInputBindingDescription getBindingDescription()
		{
			vk::VertexInputBindingDescription binding_description{};
			binding_description.binding = 0;
			binding_description.stride = sizeof(vertex);
			binding_description.inputRate = vk::VertexInputRate::eVertex;
			return binding_description;
		}
		static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
		{
			std::vector<vk::VertexInputAttributeDescription> attribute_descriptions(2);

			attribute_descriptions[0].binding = 0;
			attribute_descriptions[0].location = 0;
			attribute_descriptions[0].format = vkeFormatVec2;
			attribute_descriptions[0].offset = static_cast<uint32_t>(offsetof(vertex, pos));

			attribute_descriptions[1].binding = 0;
			attribute_descriptions[1].location = 1;
			attribute_descriptions[1].format = vkeFormatVec3;
			attribute_descriptions[1].offset = static_cast<uint32_t>(offsetof(vertex, col));

			return attribute_descriptions;
		}
	};

	struct vertex_instanced
	{
		alignas(16) mat4 model;
		static vk::VertexInputBindingDescription getBindingDescription()
		{
			return vk::VertexInputBindingDescription
			{
				1,							   //binding   
				sizeof(vertex_instanced),      //stride    
				vk::VertexInputRate::eInstance,//inputRate 
			};
		}
		static std::pair<std::array<vk::VertexInputAttributeDescription, 4>, uint32_t> getAttributeDescriptions(uint32_t loc = 0)
		{
			std::array<vk::VertexInputAttributeDescription, 4> attribute_descriptions{};

			for (int i = 0; i < 4; ++i)
			{
				attribute_descriptions[i].binding = 1;
				attribute_descriptions[i].location = loc;
				attribute_descriptions[i].format = vkeFormatVec4;
				attribute_descriptions[i].offset = static_cast<uint32_t>(offsetof(vertex_instanced, model) + i * sizeof(vec4));
				loc += calc_attr_offset(sizeof(vec4));
			}


			return std::make_pair(attribute_descriptions, loc);
		}
	};


	const std::vector<vertex> g_vertices =
	{
		{{-0.5f,-0.5f},{ 1.0f, 0.0f, 0.0f}},
		{{ 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f}},
		{{ 0.5f,-0.5f},{ 1.0f, 1.0f, 1.0f}},
	};
	const std::vector<uint16_t> g_indices
	{
		0,1,2,
		1,0,3
	};

	std::vector<vertex_instanced> g_vinstanced =
	{
		{
			{glm::translate(vec3{0,0,0}) * mat4 { glm::rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::rad(glm::deg(90.0f) * 0.0f)) }
	//{1.0f,0.0f,0.0f,0.0f},
	//{0.0f,1.0f,0.0f,0.0f},
	//{0.0f,0.0f,1.0f,1.0f},
	//{0.0f,0.0f,0.0f,1.0f},
	}
},
{
	{glm::translate(vec3{0,1,0}) * mat4 { glm::rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::rad(glm::deg(90.0f) * 0.0f)) }
	//{1.0f,0.0f,0.0f,0.0f},
	//{0.0f,1.0f,0.0f,0.0f},
	//{0.0f,0.0f,1.0f,1.0f},
	//{0.0f,0.0f,0.0f,1.0f},
	}
},
{
	{glm::translate(vec3{0,0,1}) * mat4 { glm::rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::rad(glm::deg(90.0f) * 0.0f)) }
	//{1.0f,0.0f,0.0f,0.0f},
	//{0.0f,1.0f,0.0f,0.0f},
	//{0.0f,0.0f,1.0f,1.0f},
	//{00.0f,00.0f,0.0f,1.0f},
	}
}
	};


	bool VulkanState::QueueFamilyIndices::isComplete() const
	{
		return graphics_family && present_family;
	}

	float VulkanState::deviceSuitability(vk::PhysicalDevice const& pd)
	{
		auto properties = pd.getProperties(dispatcher);
		auto features = pd.getFeatures(dispatcher);

		std::vector < std::function<float(vk::PhysicalDeviceProperties const&, vk::PhysicalDeviceFeatures const&)>> checks
		{
			[](vk::PhysicalDeviceProperties const& properties, vk::PhysicalDeviceFeatures const&)->float
		{
			float result = 0;
			switch (properties.deviceType)
			{
			case vk::PhysicalDeviceType::eDiscreteGpu: result = 1.0f; break;
			case vk::PhysicalDeviceType::eIntegratedGpu: result = 0.5f; break;
			default:throw "incorrect physical device type"; break;
			}
			return result;
		},
			[](vk::PhysicalDeviceProperties const& , vk::PhysicalDeviceFeatures const& features)->float
		{
			//throw if this is a deal breaker.
			return (features.geometryShader) ? 1.0f : 0.0f;
		},
			[](vk::PhysicalDeviceProperties const& , vk::PhysicalDeviceFeatures const& features)->float
		{
			//throw if this is a deal breaker.
			if (!features.fillModeNonSolid) throw "unable to support fillmode non-solid";
			return (features.fillModeNonSolid) ? 1.0f : 0.0f;
		},
		};
		float total = 0.0f;
		try
		{
			for (auto& check : checks)
			{
				total += check(properties, features);
			}
			if (!findQueueFamilies(pd).isComplete())
				throw "Unable to support desired queue family.";

			auto rextensions = pd.enumerateDeviceExtensionProperties(nullptr, dispatcher);
			std::unordered_set<std::string> extensions;
			std::transform(rextensions.begin(), rextensions.end(),
				std::inserter(extensions, extensions.begin()),
				[](decltype(rextensions)::value_type const& rext) {
					return rext.extensionName;
				}
			);
			auto required = GetDeviceExtensions();
			for (auto& ext : required)
			{
				if (extensions.find(ext) == extensions.end())
				{
					throw "Unsuitable device";
				}
			}
			auto swapChainSupportDetails = querySwapChainSupport(pd);
			if (swapChainSupportDetails.formats.empty() || swapChainSupportDetails.presentModes.empty())
				throw "Device doesn't support required swapchain extensions";
		}
		catch (...)
		{
			total = -1.0f;
		}
		return total;
	}

	vk::PhysicalDevice VulkanState::SelectDevice(std::vector<vk::PhysicalDevice> const& devices)
	{
		float most_suitable = 0;
		vk::PhysicalDevice pd = {};
		for (auto& device : devices)
		{
			float suitability = deviceSuitability(device);
			if (most_suitable < suitability)
			{
				pd = device;
				most_suitable = suitability;
			}
		}
		if (most_suitable <= 0)
			throw std::runtime_error("Failed to find suitable GPU.");
		auto dbg_prop = pd.getProperties();
		auto dbg_feat = pd.getFeatures();
		return pd;
	}

	std::vector<const char*> VulkanState::GetValidationLayers()
	{
		std::vector<const char*> layers
		{
#ifndef WX_VAL_VULK
			"VK_LAYER_KHRONOS_validation"
#endif
		};
		return layers;
	}

	std::vector<const char*> VulkanState::GetExtensions(vk::Optional<const std::string>)
	{
		std::vector<const char*> ext
		{
			//VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		};
		if (GetValidationLayers().size() > 0)
		{
			ext.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		return ext;
	}

	std::vector<const char*> VulkanState::GetDeviceExtensions()
	{
		std::vector<const char*> ext
		{
			//VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};
		return ext;
	}

	std::vector<vk::LayerProperties> VulkanState::GetAllValidationLayers()
	{
		auto result = vk::enumerateInstanceLayerProperties(dispatcher);
		return result;
	}

	std::vector<vk::ExtensionProperties> VulkanState::GetAllExtensions(vk::Optional<const std::string> layer)
	{
		auto result = vk::enumerateInstanceExtensionProperties(layer, dispatcher);
		return result;
	}

	vk::SurfaceFormatKHR VulkanState::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
	{
		std::optional<vk::SurfaceFormatKHR> result;
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				result = availableFormat;
				break;
			}
		}
		if (!result)
		{
			//Failed to find the ideal format.
			if (availableFormats.size() > 0)
				result = availableFormats[0];
			else
				throw std::runtime_error{ "Invalid list of available surface formats." };
		}
		return *result;
	}

	vk::PresentModeKHR VulkanState::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
	{
		vk::PresentModeKHR result = vk::PresentModeKHR::eFifo;
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				result = availablePresentMode;
			}
		}
		//This is guaranteed to be present
		return result;
	}

	vk::Extent2D VulkanState::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
	{
		vk::Extent2D result;

		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			//Match with the current window
			result = capabilities.currentExtent;
		}
		else {
			//Set our desired size
			auto size = m_window.size;
			WIDTH = static_cast<uint32_t>(size.x);
			HEIGHT = static_cast<uint32_t>(size.y);
			vk::Extent2D actualExtent = { WIDTH, HEIGHT };
			//Make sure it fits within the window contraint
			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			result = actualExtent;
		}
		return result;
	}

	VulkanState::QueueFamilyIndices VulkanState::findQueueFamilies(vk::PhysicalDevice const& device)
	{
		QueueFamilyIndices result;
		auto families = device.getQueueFamilyProperties();
		for (auto i = families.size(); i-- > 0;)
		{
			auto& family = families[i];
			;

			if (family.queueCount > 0)
			{
				if (device.getSurfaceSupportKHR(static_cast<uint32_t>(i), *m_surface, dispatcher))
				{
					result.present_family = static_cast<uint32_t>(i);
				}
				if (family.queueFlags & vk::QueueFlagBits::eGraphics)
				{
					result.graphics_family = static_cast<uint32_t>(i);
				}
				//if (family.queueFlags & vk::QueueFlagBits::eTransfer)
				//{
				//	result.transfer_family = static_cast<uint32_t>(i);
				//}
			}
			if (result.isComplete())
			{
				break;
			}
		}
		return result;
	}

	VulkanState::SwapChainSupportDetails VulkanState::querySwapChainSupport(const vk::PhysicalDevice& device) {
		SwapChainSupportDetails details;
		details.capabilities = device.getSurfaceCapabilitiesKHR(*m_surface, dispatcher);
		details.formats = device.getSurfaceFormatsKHR(*m_surface, dispatcher);
		details.presentModes = device.getSurfacePresentModesKHR(*m_surface, dispatcher);
		return details;
	}

	void VulkanState::createInstance()
	{

		vk::ApplicationInfo appInfo = {
			"Hello Triangle",         //pApplicationName
			VK_MAKE_VERSION(1, 0, 0), //applicationVersion
			"No Engine",			  //pEngineName 
			VK_MAKE_VERSION(1, 0, 0), //engineVersion
			VK_API_VERSION_1_0,		  //apiVersion   
									  //vk::StructureType::eApplicationInfo ,
		};
		std::vector<const char*> extensions = GetExtensions();
		auto vext = GetAllExtensions();

		std::vector<const char*> layers = GetValidationLayers();
		auto vlayers = GetAllValidationLayers();
		if (!CheckProperties(vlayers, layers, [](auto& layer) {return layer.layerName; }))
		{
			throw "Layer not found.";
		}
		if (!CheckProperties(vext, extensions, [](const vk::ExtensionProperties& ext) {return ext.extensionName; }))
		{
			throw "Extensions not found.";
		}

		vk::InstanceCreateInfo instInfo(
			vk::InstanceCreateFlags{},
			&appInfo,
			hlp::arr_count(layers),
			layers.data(),
			hlp::arr_count(extensions),
			extensions.data()
		);
#ifndef WX_VAL_VULK
		auto debugInfo = populateDebugMessengerCreateInfo();
		instInfo.pNext = &debugInfo;
#endif
		instInfo.pNext = nullptr;
		try
		{
			*instance = vk::createInstance(instInfo, nullptr, dispatcher);
			dyn_dispatcher.init(*instance, vkGetInstanceProcAddr);
#ifndef WX_VAL_VULK

			m_debug_messenger = instance->createDebugUtilsMessengerEXTUnique(debugInfo, nullptr, dyn_dispatcher);
#endif
			//if (result != vk::Result::eSuccess)
			//{
			//	std::cout << "FAILED TO CREATE" << std::endl;
			//}
		}
		catch (vk::Error& err)
		{
			std::string msg = err.what();
			msg.append("");
			hlp::cerr() << msg << std::endl;
		}
	}

	void VulkanState::createSurface(HINSTANCE winstance, HWND wnd)
	{
		vk::Win32SurfaceCreateInfoKHR w32createInfo{ vk::Win32SurfaceCreateFlagsKHR{},winstance,wnd };
		m_surface = instance->createWin32SurfaceKHRUnique(w32createInfo, nullptr, dispatcher);
	}

	void VulkanState::pickPhysicalDevice()
	{
		auto physical_devices = instance->enumeratePhysicalDevices(dispatcher);
		pdevice = SelectDevice(physical_devices);
		buffer_offset_alignment = s_cast<decltype(buffer_offset_alignment )>(pdevice.getProperties().limits.minUniformBufferOffsetAlignment);
		buffer_size_alignment = s_cast<decltype(buffer_size_alignment)>(pdevice.getProperties().limits.nonCoherentAtomSize);
	}

	void VulkanState::createLogicalDevice()
	{
		QueueFamilyIndices indices = findQueueFamilies(pdevice);
		m_queue_family = indices;
		float queuePriority = 1.0f;
		auto uniqueQueues = m_queue_family.unique_queues();

		std::vector<vk::DeviceQueueCreateInfo> info;
		for (auto& uniqueQueueFamily : uniqueQueues)
		{
			info.emplace_back(
				vk::DeviceQueueCreateFlags{}
				, uniqueQueueFamily
				, 1
				, &queuePriority);
		}
		auto extensions = GetDeviceExtensions();
		//vk::DeviceQueueCreateInfo queueCreateInfo{
		//	vk::DeviceQueueCreateFlags{}
		//	,indices.graphics_family.value()
		//	,1
		//	,&queuePriority
		//};
		vk::PhysicalDeviceFeatures pdevFeatures{};
		pdevFeatures.fillModeNonSolid = VK_TRUE;
		auto valLayers = GetValidationLayers();

		vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags{},
			hlp::arr_count(info), info.data(),
			hlp::arr_count(valLayers), valLayers.data(),
			hlp::arr_count(extensions), extensions.data(), &pdevFeatures
		);
		//m_device.~UniqueHandle();
		m_device = vk::UniqueDevice{ pdevice.createDevice(createInfo, nullptr, dispatcher) };
		m_graphics_queue = m_device->getQueue(*m_queue_family.graphics_family, 0, dispatcher);
		m_present_queue = m_device->getQueue(*m_queue_family.present_family, 0, dispatcher);
		//m_transfer_queue = m_device->getQueue(*m_queue_family.transfer_family, 0, dispatcher);
	}

	void VulkanState::createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(pdevice);

		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		//Ask for minimum + 1 so that we have 1 extra image to work with
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		QueueFamilyIndices indices = findQueueFamilies(pdevice);
		uint32_t queueFamilyIndices[] = { indices.graphics_family.value(), indices.present_family.value(), *indices.transfer_family };
		vk::SharingMode imageSharingMode = vk::SharingMode::eExclusive;
		uint32_t        queueFamilyIndexCount = 0;
		uint32_t* pQueueFamilyIndices = nullptr;

		//For ease of tutorial RN, exclusive is more efficient, but requires explicit transfers across queue families.
		if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
			imageSharingMode = vk::SharingMode::eConcurrent;
			queueFamilyIndexCount = hlp::arr_count(queueFamilyIndices);
			pQueueFamilyIndices = queueFamilyIndices;
		}
		vk::SwapchainCreateInfoKHR createInfo
		{
			vk::SwapchainCreateFlagsKHR{},
			*m_surface,
			imageCount,
			surfaceFormat.format,
			surfaceFormat.colorSpace,
			extent,/*
				   vk::Extent2D{0,0},//*/
				   //Always 1 unless developing stereoscopic 3d images
				   1,
				   vk::ImageUsageFlagBits::eColorAttachment,
				   imageSharingMode,
				   queueFamilyIndexCount,
				   pQueueFamilyIndices,
				   swapChainSupport.capabilities.currentTransform,
				   //Composite Alpha is for alpha in respect to other windows
				   vk::CompositeAlphaFlagBitsKHR::eOpaque,
				   presentMode,
				   VK_TRUE
		};
		m_swapchain.swap_chain = m_device->createSwapchainKHRUnique(createInfo, nullptr, dispatcher);


		m_swapchain.images = m_device->getSwapchainImagesKHR(*m_swapchain.swap_chain, dispatcher);
		m_swapchain.extent = extent;
		//m_swapchain.format = surfaceFormat.format;
		m_swapchain.surface_format = surfaceFormat;
		m_swapchain.present_mode = presentMode;

	}

	void VulkanState::createFrameObjects()
	{
		for ([[maybe_unused]]auto& image : m_swapchain.images)
		{
			FrameObjects fo { *view_,*view_ };
			//FrameObjects fo2 = std::move(fo);
			m_swapchain.frame_objects.emplace_back(std::move(fo));
		}
	}

	void VulkanState::createImageViews()
	{
		auto& images = m_swapchain.images;
		auto& image_views = m_swapchain.image_views;
		image_views.clear();
		//for (size_t i = 0; i < images.size(); ++i)
		for (auto& image : images)
		{
			vk::ImageViewCreateInfo createInfo{
				vk::ImageViewCreateFlags{},
				image,
				vk::ImageViewType::e2D,
				m_swapchain.surface_format.format,
				vk::ComponentMapping{},
				vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor,0,1,0,1 }
			};
			image_views.emplace_back(m_device->createImageViewUnique(createInfo, nullptr, dispatcher));
		}
	}

	vk::UniqueShaderModule VulkanState::createShaderModule(const std::string& code)
	{
		vk::ShaderModuleCreateInfo mod{
			vk::ShaderModuleCreateFlags{},
			code.length(),reinterpret_cast<uint32_t const*>(code.data())
		};
		return m_device->createShaderModuleUnique(mod);
	}

	void VulkanState::createRenderPass()
	{
		vk::AttachmentDescription colorAttachment
		{
			vk::AttachmentDescriptionFlags{}
			,m_swapchain.surface_format.format
			,vk::SampleCountFlagBits::e1
			,vk::AttachmentLoadOp::eClear
			,vk::AttachmentStoreOp::eStore
			,vk::AttachmentLoadOp::eDontCare
			,vk::AttachmentStoreOp::eDontCare
			,vk::ImageLayout::eUndefined
			,vk::ImageLayout::ePresentSrcKHR
		};

		vk::AttachmentReference colorAttachmentRef
		{
			0
			,vk::ImageLayout::eColorAttachmentOptimal
		};

		vk::SubpassDescription subpass
		{
			vk::SubpassDescriptionFlags{}
			,vk::PipelineBindPoint::eGraphics
			,0,nullptr
			,1,&colorAttachmentRef
		};

		vk::SubpassDependency dependency
		{
			VK_SUBPASS_EXTERNAL//src
			,0U				   //dest
			,vk::PipelineStageFlagBits::eColorAttachmentOutput
			,vk::PipelineStageFlagBits::eColorAttachmentOutput
			,vk::AccessFlags{}
			,vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
		};
		vk::RenderPassCreateInfo renderPassInfo
		{
			vk::RenderPassCreateFlags{}
			,1,&colorAttachment
			,1,&subpass
			,1,&dependency
		};

		m_renderpass = m_device->createRenderPassUnique(renderPassInfo, nullptr, dispatcher);

		//Temporary For RenderState
		auto& rss = view_->RenderStates();
		for (auto& rs : rss)
			rs.RenderPass() = *m_renderpass;
	}

	void VulkanState::createDescriptorSetLayout()
	{
		vk::DescriptorSetLayoutBinding uboLayoutBinding[]
		{
			{
			 0
			,vk::DescriptorType::eUniformBuffer
			,1
			,vk::ShaderStageFlagBits::eVertex
			, nullptr //pImmutableSamplers
			}
		};

		vk::DescriptorSetLayoutCreateInfo uboLayoutCreateInfo
		{
			 vk::DescriptorSetLayoutCreateFlags{}
			,hlp::arr_count(uboLayoutBinding)
			,std::data(uboLayoutBinding)
		};
		m_descriptorsetlayout = m_device->createDescriptorSetLayoutUnique(uboLayoutCreateInfo, nullptr, dispatcher);

		vk::DescriptorSetLayoutBinding uboLayoutBinding2[]
		{
			vk::DescriptorSetLayoutBinding{
			 0
			,vk::DescriptorType::eUniformBuffer
			,2
			,vk::ShaderStageFlagBits::eVertex
			, nullptr //pImmutableSamplers
			}
		};
		vk::DescriptorSetLayoutCreateInfo uboLayoutCreateInfo2
		{
			 vk::DescriptorSetLayoutCreateFlags{}
			,hlp::arr_count(uboLayoutBinding2)
			,std::data(uboLayoutBinding2)
		};
		;
		m_swapchain.uniforms2.layout = m_device->createDescriptorSetLayoutUnique(uboLayoutCreateInfo2, nullptr, dispatcher);

	}

	void VulkanState::createGraphicsPipeline()
	{
		auto vert = hlp::GetBinaryFile("shaders/vertex.vert.spv");
		auto frag = hlp::GetBinaryFile("shaders/fragment.frag.spv");

		auto fragModule = createShaderModule(frag);
		auto vertModule = createShaderModule(vert);

		const char* entryPoint = "main";

		vk::PipelineShaderStageCreateInfo fragShaderStageInfo
		{
			vk::PipelineShaderStageCreateFlags{},
			vk::ShaderStageFlagBits::eFragment,
			*fragModule,
			entryPoint,
			nullptr
		};

		vk::PipelineShaderStageCreateInfo vertShaderStageInfo
		{
			vk::PipelineShaderStageCreateFlags{},
			vk::ShaderStageFlagBits::eVertex,
			*vertModule,
			entryPoint,
			nullptr
		};
		vk::PipelineShaderStageCreateInfo stageCreateInfo[] = { vertShaderStageInfo,fragShaderStageInfo };

		vk::VertexInputBindingDescription binding_desc[] = {
			vertex::getBindingDescription()
			,vertex_instanced::getBindingDescription()
		};

		auto attr_desc = vertex::getAttributeDescriptions();
		auto attr_desc2 = vertex_instanced::getAttributeDescriptions(2);
		attr_desc.insert(attr_desc.end(), attr_desc2.first.begin(), attr_desc2.first.end());

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
			vk::PipelineVertexInputStateCreateFlags{}
			,hlp::arr_count(binding_desc)                         //vertexBindingDescriptionCount   
			,std::data(binding_desc)                          //pVertexBindingDescriptions      
			,hlp::arr_count(attr_desc)                            //vertexAttributeDescriptionCount 
			,std::data(attr_desc)                             //pVertexAttributeDescriptions
		};
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
			vk::PipelineInputAssemblyStateCreateFlags{}
			,vk::PrimitiveTopology::eTriangleList
			,VK_FALSE                              //Set to true to allow strips to be restarted with special indices 0xFFFF or 0xFFFFFFFF
		};

		vk::Viewport viewport
		{
			0.0f,0.0f, //x,y
			(float)m_swapchain.extent.width, (float)m_swapchain.extent.height,
			0.0f,1.0f // min/max depth
		};
		vk::Rect2D scissor{
			{ 0,0 },
			m_swapchain.extent
		};

		vk::PipelineViewportStateCreateInfo viewportState
		{
			vk::PipelineViewportStateCreateFlags{}
			,1,&viewport
			,1,&scissor
		};

		vk::PipelineRasterizationStateCreateInfo rasterizer
		{
			vk::PipelineRasterizationStateCreateFlags{}
			,VK_FALSE //depthClampEnable VK_FALSE discards fragments that are beyond the depth range, VK_TRUE clamps it instead.
			,VK_FALSE //If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage. This basically disables any output to the framebuffer.
			,vk::PolygonMode::eFill //Any other mode requires enabling a GPU feature
			,vk::CullModeFlagBits::eFront
			,vk::FrontFace::eClockwise
			,VK_FALSE
			,0.0f
			,0.0f
			,0.0f
			,1.0f //Any lines thicker than 1.0f requires wideLines gpu feature.
		};

		vk::PipelineMultisampleStateCreateInfo multisampling{
			vk::PipelineMultisampleStateCreateFlags{}
			,vk::SampleCountFlagBits::e1 //rasterizationSamples  
			,VK_FALSE					 //sampleShadingEnable   
			,1.0f						 //minSampleShading      // Optional
			,nullptr					 //pSampleMask           // Optional
			,VK_FALSE					 //alphaToCoverageEnable // Optional
			,VK_FALSE					 //alphaToOneEnable      // Optional
		};

		//VkPipelineDepthStencilStateCreateInfo //For depth/stencil buffers; ignored for now

		//Per frame buffer
		vk::PipelineColorBlendAttachmentState colorBlendAttachment
		{
			/*blendEnable         */VK_FALSE
			/*srcColorBlendFactor */,vk::BlendFactor::eOne	//optional
			/*dstColorBlendFactor */,vk::BlendFactor::eZero	//optional
			/*colorBlendOp        */,vk::BlendOp::eAdd		//optional
			/*srcAlphaBlendFactor */,vk::BlendFactor::eOne	//optional
			/*dstAlphaBlendFactor */,vk::BlendFactor::eZero	//optional
			/*alphaBlendOp        */,vk::BlendOp::eAdd      //optional
			/*colorWriteMask      */,vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
		};
		/*
		//How the values are used are like this:
		if (blendEnable) {
		finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
		finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
		} else {
		finalColor = newColor;
		}

		finalColor = finalColor & colorWriteMask;
		*/
		vk::PipelineColorBlendStateCreateInfo colorBlending
		{
			vk::PipelineColorBlendStateCreateFlags{}
			,VK_FALSE                //logicOpEnable   
			,vk::LogicOp::eCopy	     //logicOp         
			,1					     //attachmentCount 
			,&colorBlendAttachment   //pAttachments   
			,{ 0.0,0.0f,0.0f,0.0f }
		};


		vk::DynamicState dynamicStates[] = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eLineWidth
		};

		vk::PipelineDynamicStateCreateInfo dynamicState
		{
			vk::PipelineDynamicStateCreateFlags{}
			,2            //dynamicStateCount 
			,dynamicStates//pDynamicStates    
		};
		//For uniforms
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo
		{
			vk::PipelineLayoutCreateFlags{}
			, 1		  //setLayoutCount         
			, &*m_descriptorsetlayout //pSetLayouts            
			, 0		  //pushConstantRangeCount 
			, nullptr //pPushConstantRanges    
		};

		m_pipelinelayout = m_device->createPipelineLayoutUnique(pipelineLayoutInfo, nullptr, dispatcher);
		vk::GraphicsPipelineCreateInfo pipelineInfo
		{
			vk::PipelineCreateFlags{}
			,hlp::arr_count(stageCreateInfo),stageCreateInfo
			,&vertexInputInfo
			,&inputAssembly
			,nullptr
			,&viewportState
			,&rasterizer
			,&multisampling
			,nullptr
			,&colorBlending
			,nullptr
			,*m_pipelinelayout
			,*m_renderpass
			,0
		};
		m_pipeline = m_device->createGraphicsPipelineUnique({}, pipelineInfo, nullptr, dispatcher);
	}

	void VulkanState::createFramebuffers()
	{
		auto& image_views = m_swapchain.image_views;
		auto& extent = m_swapchain.extent;
		m_swapchain.frame_buffers.clear();
		for (auto& image_view : image_views) {
			vk::ImageView attachments[] = {
				*image_view
			};

			vk::FramebufferCreateInfo framebufferInfo
			{
				vk::FramebufferCreateFlags{}
				,*m_renderpass
				,1
				,attachments
				,extent.width
				,extent.height
				,1
			};
			m_swapchain.frame_buffers.emplace_back(m_device->createFramebufferUnique(framebufferInfo, nullptr, dispatcher));
		}
	}

	void VulkanState::createCommandPool()
	{
		vk::CommandPoolCreateInfo info
		{
			vk::CommandPoolCreateFlags{vk::CommandPoolCreateFlagBits::eResetCommandBuffer}
			,*this->m_queue_family.graphics_family
		};
		m_commandpool = m_device->createCommandPoolUnique(info, nullptr, dispatcher);

	}

	struct VulkanState::vbo
	{
		vk::UniqueBuffer gpu_buffer;
		vk::UniqueDeviceMemory gpu_memory;
		vbo(vk::UniqueBuffer&& gpu_buffer_,
			vk::UniqueDeviceMemory&& gpu_memory_)
			: gpu_buffer{ std::move(gpu_buffer_) }
			, gpu_memory{ std::move(gpu_memory_) }
		{

		}
	};

	void VulkanState::createVertexBuffers()
	{
		vk::DeviceSize bufferSize = hlp::buffer_size(g_vertices);

		auto [stagingBuffer, stagingBufferMemory] = hlp::CreateAllocBindBuffer(
			pdevice, *m_device, bufferSize,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			dispatcher);

		{
			hlp::MapMemory(*m_device, *stagingBufferMemory, 0, std::data(g_vertices), bufferSize, dispatcher);
		}


		auto [vertex_buffer, device_memory] = hlp::CreateAllocBindBuffer(
			pdevice, *m_device, static_cast<vk::DeviceSize>(hlp::buffer_size(g_vertices)),
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			dispatcher);

		dbg_vertex_buffer = std::make_unique<DbgVertexBuffer>(pdevice, *m_device, hlp::buffer_size(g_vinstanced));
		//auto [instance_buffer, instance_memory] = CreateAllocBindBuffer(
		//	pdevice, *m_device, static_cast<vk::DeviceSize>(buffer_size(g_vertices)),
		//	vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		//	vk::MemoryPropertyFlagBits::eDeviceLocal,
		//	dispatcher);
		m_vertex_buffers.emplace_back(std::move(vertex_buffer));
		m_vertex_memories.emplace_back(std::move(device_memory));

		//m_vertex_buffers.emplace_back( std::move(instance_buffer));
		//m_vertex_memories.emplace_back(std::move(instance_memory));

		copyBuffer(*stagingBuffer, *m_vertex_buffers.back(), bufferSize);
		//Invalid now cause vertex buffer is now on the device(gpu), not host.
		//vk::MappedMemoryRange mmr
		//{
		//	 *m_device_memory
		//	,0
		//	,buffer_size(vertices)
		//};
		//auto handle = m_device->mapMemory(*m_device_memory, mmr.offset, mmr.size, vk::MemoryMapFlags{}, dispatcher);
		//memcpy_s(handle, mmr.size, std::data(vertices), buffer_size(vertices));
		//std::vector<decltype(mmr)> memory_ranges
		//{
		//	mmr
		//};
		////Not necessary rn since we set the HostCoherent bit 
		////This command only guarantees that the memory(on gpu) will be updated by vkQueueSubmit
		//m_device->flushMappedMemoryRanges(memory_ranges, dispatcher);
		//m_device->unmapMemory(*m_device_memory);

	}

	void VulkanState::createIndexBuffers()
	{
		vk::DeviceSize bufferSize = hlp::buffer_size(g_indices);

		auto [stagingBuffer, stagingMemory] = hlp::CreateAllocBindBuffer(
			pdevice, *m_device, bufferSize,
			vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible,
			dispatcher
		);
		hlp::MapMemory(*m_device, *stagingMemory, 0, std::data(g_indices), bufferSize, dispatcher);

		auto [index_buffer, ib_memory] = hlp::CreateAllocBindBuffer(
			pdevice, *m_device, bufferSize,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			dispatcher
		);
		m_index_buffer = std::move(index_buffer);
		m_ib_memory = std::move(ib_memory);
		copyBuffer(*stagingBuffer, *m_index_buffer, bufferSize);
	}

	void VulkanState::createUniformBuffers()
	{

		vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
		auto size = m_swapchain.images.size();
		m_swapchain.uniform_buffers.resize(size);
		m_swapchain.uniform_buffers2.resize(size);
		for (auto& uniform : m_swapchain.uniform_buffers2)
		{
			auto pair = hlp::CreateAllocBindBuffer(
				pdevice, *m_device, bufferSize,
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				dispatcher);
			uniform = std::move(pair);
		}

		for (auto& uniform : m_swapchain.uniform_buffers)
		{
			auto pair = hlp::CreateAllocBindBuffer(
				pdevice, *m_device, bufferSize,
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				dispatcher);
			uniform = std::move(pair);
		}

		vk::DeviceSize bufferSize2 = sizeof(mat4)*2;
		m_swapchain.uniforms2.Init(size);

		for (auto& p : m_swapchain.uniforms2)
		{
			auto& uniform = p.uniform_buffer;
			auto pair = hlp::CreateAllocBindBuffer(
				pdevice, *m_device, bufferSize2,
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				dispatcher);
			uniform = std::move(pair);
		}
	}

	void VulkanState::createDescriptorPool()
	{
		//TODO establish a manager for the pools so that the descriptor sets manager can pull as necessary.
		vk::DescriptorPoolSize pool_size[]
		{
			vk::DescriptorPoolSize{
				vk::DescriptorType::eUniformBuffer,
				hlp::arr_count(m_swapchain.images)*1006
			}
		};
		vk::DescriptorPoolCreateInfo create_info
		{
			 vk::DescriptorPoolCreateFlagBits{} //Flag if we'll be deleting or updating the descriptor sets afterwards
			,hlp::arr_count(m_swapchain.images)*1006
			,hlp::arr_count(pool_size)
			,std::data(pool_size)
		};
		m_descriptorpool = m_device->createDescriptorPoolUnique(create_info, nullptr, dispatcher);
	}

	void VulkanState::createDescriptorSet()
	{
		std::vector<vk::DescriptorSetLayout> layouts{ m_swapchain.images.size(), *m_descriptorsetlayout };
		vk::DescriptorSetAllocateInfo allocInfo
		{
			*m_descriptorpool
			,hlp::arr_count(layouts)
			,std::data(layouts)
		};
		m_swapchain.descriptor_sets = m_device->allocateDescriptorSets(allocInfo, dispatcher);
		{
			int i = 0;
		for ([[maybe_unused]] auto& [ubuffer, umemory] : m_swapchain.uniform_buffers)
		{
			auto& dset = m_swapchain.descriptor_sets[i++];
			vk::DescriptorBufferInfo bufferInfo[] =
			{
				vk::DescriptorBufferInfo
				{
					*ubuffer
					, 0
					,sizeof(UniformBufferObject)
				}
			};
			;

			vk::WriteDescriptorSet descriptorWrite
			{
				dset
				,0
				,0
				,hlp::arr_count(bufferInfo)
				,vk::DescriptorType::eUniformBuffer
				,nullptr
				,std::data(bufferInfo)
				,nullptr
			};
			m_device->updateDescriptorSets(descriptorWrite, nullptr, dispatcher);

		}
		}
		//Dupe
		/*
		m_swapchain.descriptor_sets2 = m_device->allocateDescriptorSets(allocInfo, dispatcher);
		{
			int i = 0;
			for ([[maybe_unused]] auto& [ubuffer, umemory] : m_swapchain.uniform_buffers2)
			{
				auto& dset = m_swapchain.descriptor_sets2[i++];
				vk::DescriptorBufferInfo bufferInfo[] =
				{
					vk::DescriptorBufferInfo
					{
						*ubuffer
						, 0
						,sizeof(UniformBufferObject)
					}
				};
				;

				vk::WriteDescriptorSet descriptorWrite
				{
					dset
					,0
					,0
					,hlp::arr_count(bufferInfo)
					,vk::DescriptorType::eUniformBuffer
					,nullptr
					,std::data(bufferInfo)
					,nullptr
				};
				m_device->updateDescriptorSets(descriptorWrite, nullptr, dispatcher);

			}

		}//For new stuff

		std::vector<vk::DescriptorSetLayout> layouts2{ m_swapchain.uniforms2.size(), *m_swapchain.uniforms2.layout };
		vk::DescriptorSetAllocateInfo allocInfo2
		{
			*m_descriptorpool
			,hlp::arr_count(layouts2)
			,std::data(layouts2)
		};
		auto ds2 = m_device->allocateDescriptorSets(allocInfo2, dispatcher);
		for (uint32_t i = 0; i < m_swapchain.uniforms2.size(); ++i)
		{
			m_swapchain.uniforms2.descriptor_set(i) = ds2[i];
		}
		int i = 0;
		for ([[maybe_unused]] auto& [buffer,dset] : m_swapchain.uniforms2)
		{
			auto& [ubuffer, memory] = buffer;
			//auto& dset = ds2[i++];
			vk::DescriptorBufferInfo bufferInfo[] =
			{
				vk::DescriptorBufferInfo
				{
					*ubuffer
					, 0
					,sizeof(mat4)
				},
				vk::DescriptorBufferInfo
				{
					*ubuffer
					, sizeof(mat4)
					,sizeof(mat4)
				}
			};
			;

			vector<vk::WriteDescriptorSet> descriptorWrite
			{
				vk::WriteDescriptorSet{
					dset
					,0
					,0
					,hlp::arr_count(bufferInfo)
					,vk::DescriptorType::eUniformBuffer
					,nullptr
					,std::data(bufferInfo)
					,nullptr
				}
			};
			m_device->updateDescriptorSets(descriptorWrite, nullptr, dispatcher);

		}
		*/
	}

	void VulkanState::createCommandBuffers()
	{
		auto& rss = view_->RenderStates();
		{
			//For RenderState
			//rss.resize(max_frames_in_flight);
			for (auto& rs : rss)
			{
				vk::CommandBufferAllocateInfo rs_alloc_info
				{
					*m_commandpool
					,vk::CommandBufferLevel::eSecondary
					,2//static_cast<uint32_t>(m_swapchain.frame_buffers.size())
				};
				auto cmd_buffers = m_device->allocateCommandBuffersUnique(rs_alloc_info, dispatcher);
				rs.TransferBuffer(std::move(cmd_buffers[0]));
				rs.CommandBuffer(std::move(cmd_buffers[1]));
			}
		}
		vk::CommandBufferAllocateInfo allocInfo
		{
			*m_commandpool
			,vk::CommandBufferLevel::eSecondary
			,static_cast<uint32_t>(m_swapchain.frame_buffers.size())
		};
		m_commandbuffers = m_device->allocateCommandBuffersUnique(allocInfo, dispatcher);
		m_commandbuffers2 = m_device->allocateCommandBuffersUnique(allocInfo, dispatcher);

		vk::CommandBufferAllocateInfo allocPriInfo
		{
			*m_commandpool
			,vk::CommandBufferLevel::ePrimary
			,static_cast<uint32_t>(m_swapchain.frame_buffers.size())
		};
		m_pri_commandbuffers = m_device->allocateCommandBuffersUnique(allocPriInfo, dispatcher);
		{
			size_t i = 0;
			for (auto& commandBuffer : m_commandbuffers) {

				vk::CommandBufferInheritanceInfo inherit_info;
				inherit_info.renderPass = *m_renderpass;
				inherit_info.framebuffer = *m_swapchain.frame_buffers[i];

				vk::CommandBufferBeginInfo begin_info
				{
					vk::CommandBufferUsageFlagBits::eRenderPassContinue | vk::CommandBufferUsageFlagBits::eSimultaneousUse
					,&inherit_info
				};
				//vk::CommandBufferBeginInfo beginInfo
				//{
				//	vk::CommandBufferUsageFlags{}
				//	,nullptr
				//};
				commandBuffer->begin(begin_info);

				vk::ClearValue clearcolor{ vk::ClearColorValue{ std::array<float,4>{0.0f,0.0f,0.0f,1.0f} } };
				vk::RenderPassBeginInfo renderPassInfo
				{
					*m_renderpass
					,*m_swapchain.frame_buffers[i]
					,vk::Rect2D{ vk::Offset2D{}, m_swapchain.extent }
					,1
					,&clearcolor
				};
				//commandBuffer->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline, dispatcher);

				commandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline, dispatcher);
				std::vector<vk::Buffer> vertex_buffers
				{
					*m_vertex_buffers[0],
					dbg_vertex_buffer->Buffer()
				};
				std::vector<vk::DeviceSize> offsets
				{
					0,
					sizeof(vertex_instanced)
				};
				commandBuffer->bindVertexBuffers(0, vertex_buffers, offsets, dispatcher);
				commandBuffer->bindIndexBuffer(*m_index_buffer, 0, vk::IndexType::eUint16, dispatcher);
				commandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *m_pipelinelayout, 0, hlp::make_array_proxy(1, &m_swapchain.descriptor_sets[i]), nullptr, dispatcher);
				commandBuffer->drawIndexed(hlp::arr_count(g_indices), 2, 0, 0, 0, dispatcher);
				//commandBuffer->executeCommands(rss[(i+1)%rss.size()].CommandBuffer(), dispatcher);
				//commandBuffer->endRenderPass(dispatcher);
				commandBuffer->end(dispatcher);
				++i;
			}
		}

	}

	void VulkanState::createSemaphores()
	{
		vk::SemaphoreCreateInfo info{};
		vk::FenceCreateInfo     fenceInfo{ vk::FenceCreateFlagBits::eSignaled };
		m_pres_signals.resize(max_frames_in_flight);

		for (auto& signal : m_pres_signals)
		{
			signal.image_available = m_device->createSemaphoreUnique(info, nullptr, dispatcher);
			signal.render_finished = m_device->createSemaphoreUnique(info, nullptr, dispatcher);
			signal.inflight_fence = m_device->createFenceUnique(fenceInfo, nullptr, dispatcher);
		}
	}

	void VulkanState::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
	{
		//Setup copy command buffer/pool
		vk::CommandBufferAllocateInfo allocInfo
		{
			*m_commandpool
			,vk::CommandBufferLevel::ePrimary
			,1
		};
		auto commandBuffer = m_device->allocateCommandBuffersUnique(allocInfo, dispatcher);
		auto& cmd_buffer = *commandBuffer[0];
		hlp::CopyBuffer(cmd_buffer, m_graphics_queue, srcBuffer, dstBuffer, size);
	}
	void VulkanState::updateUniformBuffer(uint32_t image_index)
	{
		static int curr = 0;
		static float counter = 0;
		UniformBufferObject ubo = {};
		ubo.model = mat4{ idk::rotate(vec3(0.0f, 0.0f, 1.0f), idk::rad{ idk::deg(90.0f) } *counter) };
		ubo.view = glm::lookAt(vec3(2.0f+counter, 2.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
		ubo.projection = glm::perspective(idk::rad(45.0f), m_swapchain.extent.width / (float)m_swapchain.extent.height, 0.1f, 10.0f);
		//OpenGL's Y Clip coords is inverted compared to vulkan.
		ubo.projection[1][1] *= -1;
		auto& unbuffer = m_swapchain.uniform_buffers[image_index];// (curr++ % 2) ? m_swapchain.uniform_buffers[image_index] : m_swapchain.uniform_buffers2[image_index];
		auto& ubuffer_mem = *unbuffer.second;

		hlp::MapMemory(*m_device, ubuffer_mem, 0, &ubo, static_cast<vk::DeviceSize>(sizeof(ubo)), dispatcher);
		counter += 0.01f;
		mat4 vp[]
		{
			ubo.view,
			ubo.projection
		};
		hlp::MapMemory(*m_device, *m_swapchain.uniforms2.uniform_buffer(image_index).second, 0, vp, hlp::buffer_size(vp), dispatcher);
	}
	vk::DebugUtilsMessengerCreateInfoEXT VulkanState::populateDebugMessengerCreateInfo(ValHandler* userData) {
		vk::DebugUtilsMessageSeverityFlagsEXT severity_flags;

		//severity_flags |= ;// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;// | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		vk::DebugUtilsMessengerCreateInfoEXT createInfo(
			vk::DebugUtilsMessengerCreateFlagsEXT()
			, vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
			, vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
			, &debugCallback
			, (void*)userData
		);
		return createInfo;
	}

	VkBool32 VulkanState::ValHandler::processMsg(
		[[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
		[[maybe_unused]] const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
	{
		if (messageSeverity == VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			hlp::cerr() << "Err: ";
		hlp::cerr() << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	void VulkanState::UpdateWindowSize(vec2 size)
	{
		m_window.size = ivec2{ size };
		m_ScreenResized = true;
		m_ScreenResizedForImGui = true;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanState::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		static ValHandler def;
		return static_cast<ValHandler*>((pUserData) ? pUserData : &def)->processMsg(messageSeverity, messageType, pCallbackData);
	}

	void VulkanState::CleanupSwapChain() {
		m_swapchain.frame_buffers.clear();

		//m_commandbuffers.clear();
		//m_commandpool.release();
		//std::vector<vk::CommandBuffer> cb;
		//for (auto& ucb : m_commandbuffers)
		//{
		//	cb.emplace_back(*ucb);
		//}
		m_commandbuffers.clear();
		//m_device->freeCommandBuffers(*m_commandpool, cb,dispatcher);
		m_pipeline.reset();
		m_pipelinelayout.reset();
		m_renderpass.reset();
		m_swapchain.image_views.clear();
		m_swapchain.images.clear();
		m_swapchain.swap_chain.reset();
		m_swapchain.frame_buffers.clear();
		m_swapchain.uniform_buffers.clear();
		m_descriptorpool.reset();
		//m_device->destroyPipeline(*m_pipeline, nullptr,dispatcher);
		//vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		//vkDestroyRenderPass(device, renderPass, nullptr);

		//for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		//	vkDestroyImageView(device, swapChainImageViews[i], nullptr);
		//}
		//
		//vkDestroySwapchainKHR(device, swapChain, nullptr);
	}

	void VulkanState::RecreateSwapChain()
	{
		m_device->waitIdle(dispatcher);
		CleanupSwapChain();


		createSwapChain();
		createImageViews();
		createRenderPass();
		createGraphicsPipeline();
		createFramebuffers();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSet();
		createCommandBuffers();
		createSemaphores();
	}

	void VulkanState::InitVulkanEnvironment(window_info info)
	{
		m_window = info;
		view_->RenderStates().resize(3);
		createInstance();
		createSurface(info.winstance, info.wnd);
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createFrameObjects();
		createImageViews();
		createRenderPass();
		createDescriptorSetLayout();
		createGraphicsPipeline();
		createFramebuffers();
		createCommandPool();
		createVertexBuffers();
		createIndexBuffers();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSet();
		createCommandBuffers();
		createSemaphores();
	}

	void VulkanState::NextFrame()
	{
		current_frame = (current_frame + 1) % max_frames_in_flight;
	}

	VulkanView& VulkanState::View()
	{
		return *view_;
	}

	std::unique_ptr<VulkanState::vbo> VulkanState::CreateVbo(void const* buffer_start, void const* buffer_end)
	{
		uint8_t const* bstart = static_cast<uint8_t const*>(buffer_start);
		uint8_t const* bend = static_cast<uint8_t const*>(buffer_end);
		auto [vertex_buffer, device_memory] = hlp::CreateAllocBindVertexBuffer(pdevice, *m_device, bstart, bend, dispatcher);
		//m_vertex_buffer = std::move(vertex_buffer);
		//m_device_memory = std::move(device_memory);
		vk::MappedMemoryRange mmr
		{
			 *device_memory
			,0
			,hlp::buffer_size(g_vertices)
		};
		auto handle = m_device->mapMemory(*device_memory, mmr.offset, mmr.size, vk::MemoryMapFlags{}, dispatcher);
		memcpy_s(handle, mmr.size, std::data(g_vertices), hlp::buffer_size(g_vertices));
		std::vector<decltype(mmr)> memory_ranges
		{
			mmr
		};
		//Not necessary rn since we set the HostCoherent bit 
		//This command only guarantees that the memory(on gpu) will be updated by vkQueueSubmit
		//m_device->flushMappedMemoryRanges(memory_ranges, dispatcher);
		m_device->unmapMemory(*device_memory);
		return std::make_unique<vbo>(std::move(vertex_buffer), std::move(device_memory));
	}

	void VulkanState::Draw(unique_vbo const&, unique_ubo const&, unique_pipeline const&)
	{
	}

	void VulkanState::BeginFrame()
	{
		view_->SwapRenderState();
		{
			auto& rs2 = view_->CurrRenderState();
			auto& command_buffer2 = rs2.CommandBuffer();
			auto& trf_buffer2 = rs2.TransferBuffer();
			view_->ResetMasterBuffer();
			rs2.DrawCalls().resize(0);
			command_buffer2.reset(vk::CommandBufferResetFlags{}, view_->Dispatcher());
			trf_buffer2.reset(vk::CommandBufferResetFlags{}, view_->Dispatcher());
		}

		//vk::CommandBufferBeginInfo beginInfo
		//{
		//	vk::CommandBufferUsageFlags{}
		//	,nullptr
		//};
		//command_buffer.begin(beginInfo);

	}
	void UpdateUniformDS(
		vk::Device& device, 
		vk::DescriptorSet& dset, 
		uint32_t binding, 
		vk::Buffer& ubuffer, 
		uint32_t buffer_offset, 
		uint32_t arr_index, 
		size_t size)
	{
		//auto& dset = ds2[i++];
		vk::DescriptorBufferInfo bufferInfo[] =
		{
			vk::DescriptorBufferInfo
			{
				ubuffer
				, buffer_offset
				, size
			}
		};
		;

		vector<vk::WriteDescriptorSet> descriptorWrite
		{
			vk::WriteDescriptorSet{
				dset
				,binding
				,arr_index
				,hlp::arr_count(bufferInfo)
				,vk::DescriptorType::eUniformBuffer
				,nullptr
				,std::data(bufferInfo)
				,nullptr
			}
		};
		device.updateDescriptorSets(descriptorWrite, nullptr, vk::DispatchLoaderDefault{});
	}

	struct TmpBinding
	{
		uint32_t binding;
		vk::Buffer& ubuffer;
		uint32_t buffer_offset;
		uint32_t arr_index;
		size_t size;
	};

	void UpdateUniformDS(
		vk::Device& device,
		vk::DescriptorSet& dset,
		vector<TmpBinding> bindings
		)
	{
		for (auto& binding : bindings)
		{
		//auto& dset = ds2[i++];
		vk::DescriptorBufferInfo bufferInfo[]
		{
			vk::DescriptorBufferInfo{
			binding.ubuffer
			, binding.buffer_offset
			, binding.size
			}
		};
		;

		vector<vk::WriteDescriptorSet> descriptorWrite
		{
			vk::WriteDescriptorSet{
				dset
				,binding.binding
				,binding.arr_index
				,hlp::arr_count(bufferInfo)
				,vk::DescriptorType::eUniformBuffer
				,nullptr
				,std::data(bufferInfo)
				,nullptr
			}
		};
		device.updateDescriptorSets(descriptorWrite, nullptr, vk::DispatchLoaderDefault{});
		}
	}


	void UpdateUniformDS(
		vk::Device& device,
		vk::DescriptorSet& dset,
		vector<ProcessedDrawCalls::BindingInfo> bindings
	)
	{
		for (auto& binding : bindings)
		{
			//auto& dset = ds2[i++];
			vk::DescriptorBufferInfo bufferInfo[]
			{
				vk::DescriptorBufferInfo{
				binding.ubuffer
				, binding.buffer_offset
				, binding.size
				}
			};
			;

			vector<vk::WriteDescriptorSet> descriptorWrite
			{
				vk::WriteDescriptorSet{
					dset
					,binding.binding
					,binding.arr_index
					,hlp::arr_count(bufferInfo)
					,vk::DescriptorType::eUniformBuffer
					,nullptr
					,std::data(bufferInfo)
					,nullptr
				}
			};
			device.updateDescriptorSets(descriptorWrite, nullptr, vk::DispatchLoaderDefault{});
		}
	}

	vk::DescriptorSetLayout GetUniformLayout(VulkanPipeline& pipeline, uint32_t set)
	{
		auto itr = pipeline.uniform_layouts.find(set);
		assert(itr != pipeline.uniform_layouts.end());
			//throw std::runtime_error("Received incorre")
		return *itr->second;
	}
	intptr_t HashDsLayout(const vk::DescriptorSetLayout& layout)
	{
		return r_cast<intptr_t>(layout.operator VkDescriptorSetLayout());
	}
	;
	using DsBindingCount =hash_table<vk::DescriptorSetLayout, uint32_t>;
	std::pair<ProcessedDrawCalls,DsBindingCount> ProcessDcUniforms(vector<draw_call>& draw_calls, UboManager& ubo_manager)
	{
		std::pair<ProcessedDrawCalls, DsBindingCount> result{};
		DsBindingCount& collated_layouts = result.second;
		for (auto& dc : draw_calls)
		{
			auto& layouts = dc.pipeline->uniform_layouts;
			//set, bindings
			hash_table < uint32_t, vector<ProcessedDrawCalls::BindingInfo>> collated_bindings;
			for (auto& uniform : dc.uniforms)
			{
				auto itr = layouts.find(uniform.set);
				if (itr != layouts.end())
				{
					collated_layouts[(*itr->second)]++;
					auto&& [buffer,offset] = ubo_manager.Add(uniform.data);
					collated_bindings[uniform.set].emplace_back(
						ProcessedDrawCalls::BindingInfo
						{
							uniform.binding,
							buffer,
							offset,
							0,
							uniform.data.size()
						}
					);
				}
			}
			result.first.Add(ProcessedDrawCalls::DrawCall{&dc,std::move(collated_bindings)});
		}
		return result;
	}
	void PdcToCmdBuffer(const ProcessedDrawCalls& pdcs, vk::CommandBuffer& command_buffer, RenderState& rs,DescriptorSetLookup& alloced_dsets, VulkanView* view_)
	{
		auto& m_device = view_->Device();
		auto& dispatcher = view_->Dispatcher();
		VulkanPipeline* prev_pipeline = nullptr;
		//convert the processed draw calls into instructions
		for (auto& pdc : pdcs.draw_calls)
		{
			auto& dc = *pdc.p_dc;
			//Don't rebind the pipeline if it was already bound.
			if (dc.pipeline != prev_pipeline)
			{
				dc.pipeline->Bind(command_buffer, *view_);
				prev_pipeline = dc.pipeline;
			}
			//Obsolete for now, may be useful to move the uniform stuff into a clean line like this
			//dc.pipeline->BindUniformDescriptions(command_buffer, *view_, dc.uniform_info);
			auto& layouts = dc.pipeline->uniform_layouts;
			for (auto& binding : pdc.bindings)
			{
				auto set_index = binding.first;
				//Get the descriptor set layout for the current set
				auto layout_itr = layouts.find(set_index);
				if (layout_itr != layouts.end())
				{
					//Find the allocated pool of descriptor sets that matches the descriptor set layout
					auto ds_itr = alloced_dsets.find(*layout_itr->second);
					if (ds_itr != alloced_dsets.end())
					{
						//Get a descriptor set from the allocated pool
						auto ds = ds_itr->second.GetNext();
						//Update the descriptor set
						UpdateUniformDS(*m_device,ds,binding.second);
						command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *dc.pipeline->pipelinelayout, set_index, ds, nullptr, dispatcher);
					}
				}
			}
			for (auto& [first_binding, offset] : dc.vtx_binding)
			{
				command_buffer.bindVertexBuffers(first_binding, rs.Buffer(), offset, dispatcher);
			}
			command_buffer.draw(dc.vertex_count, dc.instance_count, 0, 0, dispatcher);
		}
	}

	void VulkanState::EndFrame()
	{
		auto& rs = view_->CurrRenderState();
		//auto& dispatcher = view_->Dispatcher();
		auto& command_buffer = rs.CommandBuffer();
		auto& fo = m_swapchain.frame_objects[m_swapchain.curr_index];
		vk::CommandBufferInheritanceInfo inherit_info{};
		inherit_info.renderPass = rs.RenderPass();
		inherit_info.framebuffer = *m_swapchain.frame_buffers[m_swapchain.curr_index];

		vk::CommandBufferBeginInfo begin_info
		{
			vk::CommandBufferUsageFlagBits::eRenderPassContinue
			,&inherit_info//&inherit_info
		};

		vk::CommandBufferInheritanceInfo trf_inherit{};
		vk::CommandBufferBeginInfo trf_begin_info
		{
			vk::CommandBufferUsageFlags{},
			&trf_inherit
		};


		rs.TransferBuffer().reset(vk::CommandBufferResetFlags{}, dispatcher);
		rs.TransferBuffer().begin(trf_begin_info, dispatcher);
		dbg_vertex_buffer->Update<decltype(g_vinstanced)::value_type>(0, g_vinstanced, rs.TransferBuffer());
		rs.UpdateMasterBuffer(*view_);
		rs.TransferBuffer().end();

		//rs.transfer_buffer->begin(beginInfo);
		command_buffer.begin(begin_info);
		//FrameObjects fo{ *view_ ,*view_ };
		fo.FrameReset();
		auto [pdcs,layout_count] = ProcessDcUniforms(rs.DrawCalls(), fo.ubo_manager);
		auto descriptor_sets = fo.pools.Allocate(layout_count);
		PdcToCmdBuffer(pdcs, command_buffer, rs, descriptor_sets, &*view_);

		command_buffer.end(dispatcher);

		fo.ubo_manager.UpdateAllBuffers();


	}

	void VulkanState::DrawFrame()
	{
		auto& current_signal = m_pres_signals[current_frame];
		m_device->waitForFences(1, &*current_signal.inflight_fence, VK_TRUE, std::numeric_limits<uint64_t>::max(), dispatcher);
		uint32_t imageIndex;
		auto rv = m_device->acquireNextImageKHR(*m_swapchain.swap_chain, std::numeric_limits<uint32_t>::max(), *current_signal.image_available, {}, dispatcher);
		if (rv.result != vk::Result::eSuccess)
		{
			if (rv.result == vk::Result::eErrorOutOfDateKHR)
			{
				RecreateSwapChain();
				return;
			}
			throw std::runtime_error("Failed to acquire next image.");
		}
		imageIndex = rv.value;
		m_swapchain.curr_index = rv.value;

		vk::Semaphore waitSemaphores[] = { *current_signal.image_available };
		vk::Semaphore readySemaphores[] = { *current_signal.render_finished };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

		updateUniformBuffer(imageIndex);

		vk::SubmitInfo submitInfo
		{
			1
			,waitSemaphores
			,waitStages
			,1,&*m_commandbuffers[imageIndex]
			,1,readySemaphores
		};
		m_device->resetFences(1, &*current_signal.inflight_fence, dispatcher);

		{
			auto& render_state = view_->CurrRenderState();
			auto& command_buffer = *m_pri_commandbuffers[m_swapchain.curr_index];
			EndFrame();

			vk::ClearValue clearcolor{ vk::ClearColorValue{ std::array<float,4>{0.0f,0.0f,0.0f,0.0f} } };
			vk::RenderPassBeginInfo renderPassInfo
			{
				render_state.RenderPass()
				,*m_swapchain.frame_buffers[m_swapchain.curr_index]
				,vk::Rect2D{ vk::Offset2D{}, m_swapchain.extent }
				,1
				,&clearcolor
			};
			vk::CommandBufferBeginInfo begin_info
			{
				vk::CommandBufferUsageFlags{}
				,nullptr//&inherit_info
			};
			command_buffer.reset(vk::CommandBufferResetFlags{},dispatcher);
			command_buffer.begin(begin_info);
			
			command_buffer.executeCommands(render_state.TransferBuffer(), dispatcher);
			command_buffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers, dispatcher);


			command_buffer.executeCommands(*m_commandbuffers[m_swapchain.curr_index], dispatcher);
			updateUniformBuffer(imageIndex);
			command_buffer.executeCommands(*m_commandbuffers[m_swapchain.curr_index], dispatcher);

			command_buffer.executeCommands(render_state.CommandBuffer(), dispatcher);
			command_buffer.endRenderPass(dispatcher);
			command_buffer.end();

			vk::CommandBuffer cmds[] =
			{
				//render_state.TransferBuffer(),
				//*m_commandbuffers[imageIndex],
				command_buffer,
			};
			vk::SubmitInfo render_state_submit_info
			{
				1
				,waitSemaphores
				,waitStages
				,hlp::arr_count(cmds),std::data(cmds)
				,1,readySemaphores
			};
			vk::SubmitInfo frame_submit[] = { render_state_submit_info };

			if (m_graphics_queue.submit(hlp::arr_count(frame_submit), std::data(frame_submit), *current_signal.inflight_fence, dispatcher) != vk::Result::eSuccess)
				throw std::runtime_error("failed to submit draw command buffer!");
			BeginFrame();
		}

		vk::SwapchainKHR swapchains[] = { *m_swapchain.swap_chain };

		vk::PresentInfoKHR presentInfo
		{
			1,readySemaphores
			,1,swapchains
			,&imageIndex
			,nullptr
		};
		try
		{
			rv.result = m_present_queue.presentKHR(presentInfo, dispatcher);
		if (
			rv.result
			!= vk::Result::eSuccess || m_ScreenResized)
		{
			if (m_ScreenResized)
				rv.result = vk::Result::eSuboptimalKHR;
		}
		}
		catch (const vk::OutOfDateKHRError& )
		{
			rv.result = vk::Result::eErrorOutOfDateKHR;
		}catch (const vk::Error& err)
		{
			hlp::cerr() << "Error presenting: " << err.what() << "\n";
			return;
		}

		switch (rv.result)
		{
		case vk::Result::eErrorOutOfDateKHR:
		case vk::Result::eSuboptimalKHR:
			m_ScreenResized = false;
			RecreateSwapChain();
			break;
		case vk::Result::eSuccess:
			break;
		default:
			throw std::runtime_error("Failed to present");
			break;
		}
		;
		NextFrame();
		//m_present_queue.waitIdle(dispatcher);

	}

	void VulkanState::OnResize()
	{
		m_ScreenResized = true;
		m_ScreenResizedForImGui = true;
	}

	void VulkanState::Cleanup()
	{
		m_device->waitIdle();
		dbg_vertex_buffer.reset();
		//instance.release();
	}

	VulkanState::VulkanState() : view_{ std::make_unique<VulkanView>(*this) }
	{
	}

	VulkanState::~VulkanState()
	{
		m_device->waitIdle();
		dbg_vertex_buffer.reset();
	}

	DbgVertexBuffer::DbgVertexBuffer(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes)
	{
		Resize(pdevice, device, num_bytes);
	}

	size_t grow_size(size_t sz)
	{
		size_t new_size = 1;
		while (new_size < sz)
		{
			new_size <<= 1;
		}
		return new_size;
	}

	void DbgVertexBuffer::Resize(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes, bool force_downsize)
	{
		if (num_bytes >= capacity || force_downsize)
		{
			capacity = grow_size(num_bytes);

			auto [buf, mem] = hlp::CreateAllocBindBuffer(
				pdevice, device, num_bytes,
				vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
				vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible,
				vk::DispatchLoaderDefault{}
			);
			memory = std::move(mem);
			buffer = std::move(buf);
		}
	}
	void DbgVertexBuffer::Update(vk::DeviceSize offset, vk::DeviceSize len, vk::CommandBuffer& cmd_buffer, unsigned char const* data)
	{
		cmd_buffer.updateBuffer(*buffer, offset, hlp::make_array_proxy(static_cast<uint32_t>(len), data), vk::DispatchLoaderDefault{});
	}
	void FrameObjects::FrameReset()
	{
		ubo_manager.Clear(); //Clear the previous frame's UBOs
		pools.Reset(); //Reset the previous frame's descriptors
	}
}