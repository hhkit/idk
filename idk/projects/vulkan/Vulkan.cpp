#include "pch.h"
#include "Vulkan.h"
#include <math/matrix_transforms.h>
#include <idk.h>
#include <VulkanDetail.h>
#include <VulkanHelpers.h>
#include <gfx/buffer_desc.h>
using namespace vhlp;
//Uncomment this when the temporary glm namespace glm{...} below has been removed.
//namespace glm = idk;
//Temporary, should move elsewhere
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

namespace idk
{
	enum   BufferType
	{
		eVertexBuffer
		,eIndexBuffer
	};
	struct buffer_data
	{
		using num_obj_t = size_t;
		using num_bytes_t = size_t;
		const void*    buffer = nullptr;
		num_bytes_t    len    = 0      ;
		template<typename T>
		buffer_data(const T& container)
			: buffer{ s_cast<const void*>(ArrData(container)) }, len{ buffer_size(container) }{}
		template<typename T>
		buffer_data(const T& container, num_obj_t offset, num_obj_t length)
			: buffer{ s_cast<const void*>(ArrData(container) + offset) }, len{ buffer_size(ArrData(container)+offset,ArrData(container) + length) }{}

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
		ShaderStage stage    {eNone};
	};

	struct IPipeline;

	enum RenderType
	{
		 eDraw
		,eIndexed
	};
	struct render_info
	{
		vector<buffer_data>* vertex_buffers   = nullptr;
		RenderType           render_type      = eDraw;
		uint32_t             inst_count       = 1;
	};


	class GfxInterface
	{
	public:
		using pipeline_handle = std::weak_ptr<IPipeline>;
		//Basic version, probably change this later on to use a single config struct instead.
		pipeline_handle RegisterPipeline(const std::vector<buffer_desc>& descriptors,std::vector<shader_info> shaders);
		void QueueForRendering(pipeline_handle pipeline, const render_info& info);

	};




	struct IPipeline
	{
		virtual void Create() = 0;
		virtual void RegisterCommands() = 0;
		void Draw() {}
	};
}

//template<typename RT = size_t, typename T = int>
//RT buffer_size(std::vector<T> const& vertices)
//{
//	return static_cast<RT>(sizeof(vertices[0]) * vertices.size());
//}
class DbgVertexBuffer
{
public:
	DbgVertexBuffer(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes);
	void Resize(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes,bool force_downsize=false);
	template<typename T>
	void Update(vk::DeviceSize offset, vk::ArrayProxy<T> arr, vk::CommandBuffer& buffer)
	{
		Update(offset, buffer_size(arr), buffer, reinterpret_cast<unsigned char const*>(arr.data()));
	}
	void Update(vk::DeviceSize offset, vk::DeviceSize len, vk::CommandBuffer& buffer, unsigned char const* data);
	vk::Buffer       Buffer() const { return *buffer; }
private:
	vk::UniqueDeviceMemory memory;
	vk::UniqueBuffer       buffer;

	size_t capacity=0;

};



std::unique_ptr<DbgVertexBuffer> dbg_vertex_buffer;


struct UniformBufferObject
{
	mat4 model, view, projection;
};

std::unordered_set<uint32_t> Vulkan::QueueFamilyIndices::unique_queues()const
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

		attribute_descriptions[0].binding  = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format   = vkeFormatVec2;
		attribute_descriptions[0].offset   = static_cast<uint32_t>(offsetof(vertex, pos));

		attribute_descriptions[1].binding  = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format   = vkeFormatVec3;
		attribute_descriptions[1].offset   = static_cast<uint32_t>(offsetof(vertex, col));

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
	static std::pair<std::array<vk::VertexInputAttributeDescription, 4>,uint32_t> getAttributeDescriptions(uint32_t loc=0)
	{
		std::array<vk::VertexInputAttributeDescription, 4> attribute_descriptions{};

		for (int i = 0; i < 4; ++i)
		{
			attribute_descriptions[i].binding = 1;
			attribute_descriptions[i].location = loc;
			attribute_descriptions[i].format = vkeFormatVec4;
			attribute_descriptions[i].offset = static_cast<uint32_t>(offsetof(vertex_instanced, model)+i*sizeof(vec4));
			loc += calc_attr_offset(sizeof(vec4));
		}
		

	   return std::make_pair(attribute_descriptions,loc);
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


bool Vulkan::QueueFamilyIndices::isComplete() const
{
	return graphics_family && present_family ;
}

float Vulkan::deviceSuitability(vk::PhysicalDevice const& pd)
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

vk::PhysicalDevice Vulkan::SelectDevice(std::vector<vk::PhysicalDevice> const& devices)
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

std::vector<const char*> Vulkan::GetValidationLayers()
{
	std::vector<const char*> layers
	{
		"VK_LAYER_KHRONOS_validation"
	};
	return layers;
}

std::vector<const char*> Vulkan::GetExtensions(vk::Optional<const std::string>)
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

std::vector<const char*> Vulkan::GetDeviceExtensions()
{
	std::vector<const char*> ext
	{
		//VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};
	return ext;
}

std::vector<vk::LayerProperties> Vulkan::GetAllValidationLayers()
{
	auto result = vk::enumerateInstanceLayerProperties(dispatcher);
	return result;
}

std::vector<vk::ExtensionProperties> Vulkan::GetAllExtensions(vk::Optional<const std::string> layer)
{
	auto result = vk::enumerateInstanceExtensionProperties(layer, dispatcher);
	return result;
}

vk::SurfaceFormatKHR Vulkan::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
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

vk::PresentModeKHR Vulkan::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
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

vk::Extent2D Vulkan::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
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

Vulkan::QueueFamilyIndices Vulkan::findQueueFamilies(vk::PhysicalDevice const& device)
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

Vulkan::SwapChainSupportDetails Vulkan::querySwapChainSupport(const vk::PhysicalDevice& device) {
	SwapChainSupportDetails details;
	details.capabilities = device.getSurfaceCapabilitiesKHR(*m_surface, dispatcher);
	details.formats = device.getSurfaceFormatsKHR(*m_surface, dispatcher);
	details.presentModes = device.getSurfacePresentModesKHR(*m_surface, dispatcher);
	return details;
}

void Vulkan::createInstance()
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
		ArrCount(layers),
		layers.data(),
		ArrCount(extensions),
		extensions.data()
	);
	auto debugInfo = populateDebugMessengerCreateInfo();
	instInfo.pNext = &debugInfo;
	try
	{
		*instance = vk::createInstance(instInfo, nullptr, dispatcher);
		dyn_dispatcher.init(*instance, vkGetInstanceProcAddr);
		m_debug_messenger = instance->createDebugUtilsMessengerEXTUnique(debugInfo, nullptr, dyn_dispatcher);
		//if (result != vk::Result::eSuccess)
		//{
		//	std::cout << "FAILED TO CREATE" << std::endl;
		//}
	}
	catch (vk::Error& err)
	{
		std::string msg = err.what();
		msg.append("");
		utl::cerr() << msg << std::endl;
	}
}

void Vulkan::createSurface(HINSTANCE winstance, HWND wnd)
{
	vk::Win32SurfaceCreateInfoKHR w32createInfo{ vk::Win32SurfaceCreateFlagsKHR{},winstance,wnd };
	m_surface = instance->createWin32SurfaceKHRUnique(w32createInfo, nullptr, dispatcher);
}

void Vulkan::pickPhysicalDevice()
{
	auto physical_devices = instance->enumeratePhysicalDevices(dispatcher);
	pdevice = SelectDevice(physical_devices);
}

void Vulkan::createLogicalDevice()
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

	auto valLayers = GetValidationLayers();

	vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags{},
		ArrCount(info), info.data(),
		ArrCount(valLayers), valLayers.data(),
		ArrCount(extensions), extensions.data());
	//m_device.~UniqueHandle();
	m_device = vk::UniqueDevice{ pdevice.createDevice(createInfo, nullptr, dispatcher) };
	m_graphics_queue = m_device->getQueue(*m_queue_family.graphics_family, 0, dispatcher);
	m_present_queue  = m_device->getQueue(*m_queue_family.present_family , 0, dispatcher);
	//m_transfer_queue = m_device->getQueue(*m_queue_family.transfer_family, 0, dispatcher);
}

void Vulkan::createSwapChain() {
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
		queueFamilyIndexCount = ArrCount(queueFamilyIndices);
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
	m_swapchain.format = surfaceFormat.format;

}

void Vulkan::createImageViews()
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
			m_swapchain.format,
			vk::ComponentMapping{},
			vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor,0,1,0,1 }
		};
		image_views.emplace_back(m_device->createImageViewUnique(createInfo, nullptr, dispatcher));
	}
}

vk::UniqueShaderModule Vulkan::createShaderModule(const std::string& code)
{
	vk::ShaderModuleCreateInfo mod{
		vk::ShaderModuleCreateFlags{},
		code.length(),reinterpret_cast<uint32_t const*>(code.data())
	};
	return m_device->createShaderModuleUnique(mod);
}

void Vulkan::createRenderPass()
{
	vk::AttachmentDescription colorAttachment
	{
		vk::AttachmentDescriptionFlags{}
		,m_swapchain.format
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

}

void Vulkan::createDescriptorSetLayout()
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
		,ArrCount(uboLayoutBinding)
		,ArrData(uboLayoutBinding)
	};
	m_descriptorsetlayout = m_device->createDescriptorSetLayoutUnique(uboLayoutCreateInfo, nullptr, dispatcher);
}

void Vulkan::createGraphicsPipeline()
{
	auto vert = GetBinaryFile("shaders/vertex.vert.spv");
	auto frag = GetBinaryFile("shaders/fragment.frag.spv");

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
		,ArrCount(binding_desc)                         //vertexBindingDescriptionCount   
		,ArrData(binding_desc)                          //pVertexBindingDescriptions      
		,ArrCount(attr_desc)                            //vertexAttributeDescriptionCount 
		,ArrData(attr_desc)                             //pVertexAttributeDescriptions
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
		,ArrCount(stageCreateInfo),stageCreateInfo
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

void Vulkan::createFramebuffers()
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

void Vulkan::createCommandPool()
{
	vk::CommandPoolCreateInfo info
	{
		vk::CommandPoolCreateFlags{vk::CommandPoolCreateFlagBits::eResetCommandBuffer}
		,*this->m_queue_family.graphics_family
	};
	m_commandpool = m_device->createCommandPoolUnique(info, nullptr, dispatcher);

}

struct Vulkan::vbo
{
	vk::UniqueBuffer gpu_buffer;
	vk::UniqueDeviceMemory gpu_memory;
	vbo(vk::UniqueBuffer&& gpu_buffer_,
		vk::UniqueDeviceMemory&& gpu_memory_)
		: gpu_buffer{ std::move( gpu_buffer_ )}
		, gpu_memory{ std::move( gpu_memory_ )}
	{

	}
};


void Vulkan::createVertexBuffers()
{
	vk::DeviceSize bufferSize = buffer_size(g_vertices);

	auto [stagingBuffer, stagingBufferMemory] = CreateAllocBindBuffer(
		pdevice, *m_device, bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		dispatcher);

	{
		MapMemory(*m_device,*stagingBufferMemory, 0, ArrData(g_vertices), bufferSize,dispatcher);
	}


	auto [vertex_buffer,device_memory] = CreateAllocBindBuffer(
		pdevice,*m_device, static_cast<vk::DeviceSize>(buffer_size(g_vertices)),
		vk::BufferUsageFlagBits::eTransferDst    | vk::BufferUsageFlagBits::eVertexBuffer   , 
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		dispatcher);

	dbg_vertex_buffer = std::make_unique<DbgVertexBuffer>(pdevice, *m_device, buffer_size(g_vinstanced));
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
	//memcpy_s(handle, mmr.size, ArrData(vertices), buffer_size(vertices));
	//std::vector<decltype(mmr)> memory_ranges
	//{
	//	mmr
	//};
	////Not necessary rn since we set the HostCoherent bit 
	////This command only guarantees that the memory(on gpu) will be updated by vkQueueSubmit
	//m_device->flushMappedMemoryRanges(memory_ranges, dispatcher);
	//m_device->unmapMemory(*m_device_memory);
}

void Vulkan::createIndexBuffers()
{
	vk::DeviceSize bufferSize = buffer_size(g_indices);
	
	auto [stagingBuffer, stagingMemory] = CreateAllocBindBuffer(
		pdevice, *m_device, bufferSize,
		vk::BufferUsageFlagBits::eTransferSrc|vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible,
		dispatcher
		);
	MapMemory(*m_device, *stagingMemory, 0, ArrData(g_indices), bufferSize, dispatcher);

	auto [index_buffer, ib_memory] = CreateAllocBindBuffer(
		pdevice, *m_device, bufferSize,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		dispatcher
	);
	m_index_buffer = std::move(index_buffer);
	m_ib_memory = std::move(ib_memory);
	copyBuffer(*stagingBuffer, *m_index_buffer, bufferSize);
}

void Vulkan::createUniformBuffers()
{

	vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
	auto size = m_swapchain.images.size();
	m_swapchain.uniform_buffers.resize(size);

	for(auto& uniform : m_swapchain.uniform_buffers)
	{
		auto pair = CreateAllocBindBuffer(
			pdevice, *m_device, bufferSize,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			dispatcher);
		uniform = std::move(pair);
	}

}

void Vulkan::createDescriptorPool()
{
	vk::DescriptorPoolSize pool_size[]
	{
		{
			vk::DescriptorType::eUniformBuffer,
			ArrCount(m_swapchain.images)
		} 
	};
	vk::DescriptorPoolCreateInfo create_info
	{
		 vk::DescriptorPoolCreateFlagBits{} //Flag if we'll be deleting or updating the descriptor sets afterwards
		,ArrCount(m_swapchain.images)
		,ArrCount(pool_size)
		,ArrData(pool_size)
	};
	m_descriptorpool = m_device->createDescriptorPoolUnique(create_info,nullptr,dispatcher);
}

void Vulkan::createDescriptorSet()
{
	std::vector<vk::DescriptorSetLayout> layouts{m_swapchain.images.size(), *m_descriptorsetlayout};
	vk::DescriptorSetAllocateInfo allocInfo
	{
		*m_descriptorpool
		,ArrCount(layouts)
		,ArrData(layouts)
	};
	m_swapchain.descriptor_sets =m_device->allocateDescriptorSets(allocInfo, dispatcher);
	int i = 0;
	for([[maybe_unused]]auto& [ubuffer,umemory]: m_swapchain.uniform_buffers)
	{
		auto& dset = m_swapchain.descriptor_sets[i++];
		vk::DescriptorBufferInfo bufferInfo[]=
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
			,ArrCount(bufferInfo)
			,vk::DescriptorType::eUniformBuffer
			,nullptr
			,ArrData(bufferInfo)
			,nullptr
		};
		m_device->updateDescriptorSets(descriptorWrite, nullptr, dispatcher);

	}

}

void Vulkan::createCommandBuffers()
{
	vk::CommandBufferAllocateInfo allocInfo
	{
		*m_commandpool
		,vk::CommandBufferLevel::ePrimary
		,static_cast<uint32_t>(m_swapchain.frame_buffers.size())
	};
	m_commandbuffers = m_device->allocateCommandBuffersUnique(allocInfo, dispatcher);

	size_t i = 0;
	for (auto& commandBuffer : m_commandbuffers) {
		vk::CommandBufferBeginInfo beginInfo
		{
			vk::CommandBufferUsageFlags{}
			,nullptr
		};
		commandBuffer->begin(beginInfo);

		vk::ClearValue clearcolor{ vk::ClearColorValue{ std::array<float,4>{0.0f,0.0f,0.0f,1.0f} } };
		vk::RenderPassBeginInfo renderPassInfo
		{
			*m_renderpass
			,*m_swapchain.frame_buffers[i]
			,vk::Rect2D{ vk::Offset2D{}, m_swapchain.extent }
			,1
			,&clearcolor
		};
		dbg_vertex_buffer->Update<decltype(g_vinstanced)::value_type>(0, g_vinstanced, *commandBuffer);
		commandBuffer->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline, dispatcher);
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
		commandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *m_pipelinelayout, 0, make_array_proxy(1,&m_swapchain.descriptor_sets[i]),nullptr, dispatcher);
		commandBuffer->drawIndexed(ArrCount(g_indices), 2, 0, 0,0, dispatcher);
		commandBuffer->endRenderPass(dispatcher);
		commandBuffer->end(dispatcher);
		++i;
	}
}

void Vulkan::createSemaphores()
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

void Vulkan::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	//Setup copy command buffer/pool
	vk::CommandBufferAllocateInfo allocInfo
	{
		*m_commandpool
		,vk::CommandBufferLevel::ePrimary
		,1
	};
	auto commandBuffer =m_device->allocateCommandBuffersUnique(allocInfo, dispatcher);
	auto& cmd_buffer = *commandBuffer[0];
	CopyBuffer(cmd_buffer, m_graphics_queue, srcBuffer, dstBuffer, size);
}
void Vulkan::updateUniformBuffer(uint32_t image_index)
{
	UniformBufferObject ubo = {};
	ubo.model       = mat4{idk::rotate(vec3(0.0f, 0.0f, 1.0f), idk::rad{ idk::deg(90.0f) } * 0)};
	ubo.view        = glm::lookAt(vec3(2.0f, 2.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
	ubo.projection  = glm::perspective(idk::rad(45.0f), m_swapchain.extent.width / (float)m_swapchain.extent.height, 0.1f, 10.0f);
	//OpenGL's Y Clip coords is inverted compared to vulkan.
	ubo.projection[1][1] *= -1;
	MapMemory(*m_device, *m_swapchain.uniform_buffers[image_index].second, 0, &ubo, static_cast<vk::DeviceSize>(sizeof(ubo)), dispatcher);
}
vk::DebugUtilsMessengerCreateInfoEXT Vulkan::populateDebugMessengerCreateInfo(ValHandler* userData) {
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

VkBool32 Vulkan::ValHandler::processMsg(
	[[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
	[[maybe_unused]] const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
{
	utl::cerr() << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

void Vulkan::UpdateWindowSize(vec2 size)
{
	m_window.size = size;
	m_ScreenResized = true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	static ValHandler def;
	return static_cast<ValHandler*>((pUserData) ? pUserData : &def)->processMsg(messageSeverity, messageType, pCallbackData);
}

void Vulkan::CleanupSwapChain() {
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

void Vulkan::RecreateSwapChain()
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
}

void Vulkan::InitVulkanEnvironment(window_info info)
{
	m_window = info;
	createInstance();
	createSurface(info.winstance, info.wnd);
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
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

void Vulkan::NextFrame()
{
	current_frame = (current_frame + 1) % max_frames_in_flight;
}

vgfx::VulkanDetail&& Vulkan::GetDetail()
{
	return vgfx::VulkanDetail{*this};
}

std::unique_ptr<Vulkan::vbo> Vulkan::CreateVbo(void const* buffer_start, void const* buffer_end)
{
	uint8_t const* bstart = static_cast<uint8_t const*>(buffer_start);
	uint8_t const * bend = static_cast<uint8_t const*>(buffer_end);
	auto [vertex_buffer, device_memory] = CreateAllocBindVertexBuffer(pdevice, *m_device, bstart,bend, dispatcher);
	//m_vertex_buffer = std::move(vertex_buffer);
	//m_device_memory = std::move(device_memory);
	vk::MappedMemoryRange mmr
	{
		 *device_memory
		,0
		,buffer_size(g_vertices)
	};
	auto handle = m_device->mapMemory(*device_memory, mmr.offset, mmr.size, vk::MemoryMapFlags{}, dispatcher);
	memcpy_s(handle, mmr.size, ArrData(g_vertices), buffer_size(g_vertices));
	std::vector<decltype(mmr)> memory_ranges
	{
		mmr
	};
	//Not necessary rn since we set the HostCoherent bit 
	//This command only guarantees that the memory(on gpu) will be updated by vkQueueSubmit
	m_device->flushMappedMemoryRanges(memory_ranges, dispatcher);
	m_device->unmapMemory(*device_memory);
	return std::make_unique<vbo>(std::move(vertex_buffer), std::move(device_memory));
}

void Vulkan::Draw(unique_vbo const& , unique_ubo const& , unique_pipeline const& )
{
}

void Vulkan::DrawFrame()
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

	if (m_graphics_queue.submit(1, &submitInfo, *current_signal.inflight_fence, dispatcher) != vk::Result::eSuccess)
		throw std::runtime_error("failed to submit draw command buffer!");

	vk::SwapchainKHR swapchains[] = { *m_swapchain.swap_chain };

	vk::PresentInfoKHR presentInfo
	{
		1,readySemaphores
		,1,swapchains
		,&imageIndex
		,nullptr
	};
	if (
		m_present_queue.presentKHR(presentInfo, dispatcher)
		!= vk::Result::eSuccess || m_ScreenResized)
	{
		if (m_ScreenResized)
			rv.result = vk::Result::eSuboptimalKHR;
		switch (rv.result)
		{
		case vk::Result::eErrorOutOfDateKHR:
		case vk::Result::eSuboptimalKHR:
			m_ScreenResized = false;
			RecreateSwapChain();
			break;
		default:
			throw std::runtime_error("Failed to present");
			break;
		}
	}
	;
	NextFrame();
	//m_present_queue.waitIdle(dispatcher);

}

void Vulkan::OnResize()
{
	m_ScreenResized = true;
}

void Vulkan::Cleanup()
{
	m_device->waitIdle();
	//instance.release();
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

void DbgVertexBuffer::Resize(vk::PhysicalDevice& pdevice, vk::Device& device, size_t num_bytes,bool force_downsize)
{
	if (num_bytes >= capacity || force_downsize)
	{
		capacity = grow_size(num_bytes);

		auto [buf, mem] = CreateAllocBindBuffer(
			pdevice, device, num_bytes,
			vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst ,
			vk::MemoryPropertyFlagBits::eHostCoherent| vk::MemoryPropertyFlagBits::eHostVisible,
			vk::DispatchLoaderDefault{}
		);
		memory = std::move(mem);
		buffer = std::move(buf);		
	}
}
void DbgVertexBuffer::Update(vk::DeviceSize offset, vk::DeviceSize len, vk::CommandBuffer& cmd_buffer, unsigned char const* data)
{
	cmd_buffer.updateBuffer(*buffer, offset, make_array_proxy(static_cast<uint32_t>(len), data), vk::DispatchLoaderDefault{});
}