#include "pch.h"

#include <meta/tag.inl>
#include <math/linear.inl>
#include <meta/tuple.inl>
#include <vkn/VulkanState.inl>

#include <set>
#include <map>

#include <idk.h>
#include <math/matrix_transforms.inl>
#include <gfx/buffer_desc.h>

#include <vkn/VulkanView.h>
#include <vkn/BufferHelpers.h>
#include <vkn/RenderState.h>
#include <vkn/utils/utils.h>
#include <vkn/UboManager.h>
#include <res/ResourceHandle.inl>

#include <vkn/VknRenderTarget.h>

#include <editor/IEditor.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
size_t Track(size_t s);


namespace idk
{
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
	size_t ToClearColorBit(size_t i)
	{
		return (1&i) << 1;
	}
	size_t ToClearDepthBit(size_t i)
	{
		return (1 & i) << 0;
	}
	size_t GetRpClearIndex( bool clear_col, bool clear_depth)
	{
		return ToClearColorBit(clear_col)|ToClearDepthBit(clear_depth);
	}
	bool ClearColorBit(size_t i)
	{
		return i & ToClearColorBit(1);
	}
	bool ClearDepthBit(size_t i)
	{
		return i & ToClearDepthBit(1);
	}

	std::unordered_set<uint32_t> VulkanState::QueueFamilyIndices::unique_queues()const
	{
		return
		{
			*graphics_family,
			*present_family ,
			//		*transfer_family
		};
	}


	constexpr auto vkeFormatVec2 = vk::Format::eR32G32Sfloat;
	constexpr auto vkeFormatVec3 = vk::Format::eR32G32B32Sfloat;
	constexpr auto vkeFormatVec4 = vk::Format::eR32G32B32A32Sfloat;

	uint32_t calc_attr_offset(uint32_t data_size)
	{
		return (data_size - 1) / sizeof(vec4) + 1;
	}



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
			if (!features.geometryShader)
				throw "Unable to support geometry shaders";
			return (features.geometryShader) ? 1.0f : 0.0f;
		},
			[](vk::PhysicalDeviceProperties const& , vk::PhysicalDeviceFeatures const& features)->float
		{
			//throw if this is a deal breaker.
			if (!features.fillModeNonSolid) throw "unable to support fillmode non-solid";
			return (features.fillModeNonSolid) ? 1.0f : 0.0f;
		},
			[](vk::PhysicalDeviceProperties const& , vk::PhysicalDeviceFeatures const& features)->float
		{
			//throw if this is a deal breaker.
			if (!features.samplerAnisotropy) throw "unable to support samplerAnisotropy";
			return (features.samplerAnisotropy) ? 1.0f : 0.0f;
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

			auto rextensions = pd.enumerateDeviceExtensionProperties(nullptr, dyn_dispatcher);
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
// #pragma optimize("",off)
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
			"VK_LAYER_KHRONOS_validation"
		};
		
		return (enable_validation) ? layers : decltype(layers){};
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
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
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
		try
		{
			auto debugInfo = populateDebugMessengerCreateInfo();
			instInfo.pNext = (enable_validation)?&debugInfo:nullptr;
			*instance = vk::createInstance(instInfo, nullptr, dispatcher);
			dyn_dispatcher.init(*instance, vkGetInstanceProcAddr);
			if(enable_validation)
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
		pdevFeatures.setFillModeNonSolid(VK_TRUE);
		pdevFeatures.setSamplerAnisotropy(VK_TRUE);
		pdevFeatures.setGeometryShader(VK_TRUE);


		auto valLayers = GetValidationLayers();
		vk::PhysicalDeviceDescriptorIndexingFeaturesEXT aaaaaaaa{};
		aaaaaaaa.runtimeDescriptorArray = true;

		vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags{},
			hlp::arr_count(info), info.data(),
			hlp::arr_count(valLayers), valLayers.data(),
			hlp::arr_count(extensions), extensions.data(), &pdevFeatures
		);
		//createInfo.setPNext(&aaaaaaaa);
		//m_device.~UniqueHandle();
		m_device = vk::UniqueDevice{ pdevice.createDevice(createInfo, nullptr, dispatcher) };
		m_graphics_queue = m_device->getQueue(*m_queue_family.graphics_family, 0, dispatcher);
		m_present_queue = m_device->getQueue(*m_queue_family.present_family, 0, dispatcher);
		//m_transfer_queue = m_device->getQueue(*m_queue_family.transfer_family, 0, dispatcher);
	}

	void VulkanState::createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(pdevice);

		surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		extent = chooseSwapExtent(swapChainSupport.capabilities);
		format = surfaceFormat.format;

		//Ask for minimum + 1 so that we have 1 extra image to work with
		imageCount = swapChainSupport.capabilities.minImageCount + 1;
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
				   vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc,
				   imageSharingMode,
				   queueFamilyIndexCount,
				   pQueueFamilyIndices,
				   swapChainSupport.capabilities.currentTransform,
				   //Composite Alpha is for alpha in respect to other windows
				   vk::CompositeAlphaFlagBitsKHR::eOpaque,
				   presentMode,
				   VK_TRUE
		};

		m_swapchain = std::make_unique<SwapChainInfo>(*view_,std::move(m_device->createSwapchainKHRUnique(createInfo, nullptr, dispatcher)),presentMode,surfaceFormat,extent);
		
		m_swapchain->m_swapchainGraphics.images = m_device->getSwapchainImagesKHR(*m_swapchain->swap_chain, dispatcher);
		m_swapchain->m_swapchainGraphics.CreateImageViewWithCurrImgs(*view_);
		//m_swapchain->m_graphics.images = m_swapchain->m_swapchainGraphics.images;

		//for (auto& elem : m_swapchain->m_inBetweens)
			//elem->images = m_swapchain->m_swapchainGraphics.images;
		//m_swapchain->edt_images = m_swapchain->swapchain_images;
		m_swapchain->extent = extent;
		//m_swapchain->format = surfaceFormat.format;
		m_swapchain->surface_format = surfaceFormat;
		m_swapchain->present_mode = presentMode;

	}

	void VulkanState::createFrameObjects()
	{
		for ([[maybe_unused]]auto& image : m_swapchain->m_swapchainGraphics.images)
		{
			FrameObjects fo{ *view_,*view_ };
			m_swapchain->frame_objects.emplace_back(std::move(fo));
		}
	}

	void VulkanState::createImageViews()
	{

		//m_swapchain->m_graphics.CreateImageViewWithCurrImgs(*view_);
		//m_swapchain->m_swapchainGraphics.CreateImageViewWithCurrImgs(*view_);

		//for (auto& elem : m_swapchain->m_inBetweens)
			//elem->CreateImageViewWithCurrImgs(*view_);

		m_swapchain->m_graphics.CreateImagePool(*view_);

		for (auto& elem : m_swapchain->m_inBetweens)
		{
			elem->CreateImagePool(*view_);
		}
	}

	vk::UniqueShaderModule VulkanState::createShaderModule(const string_view& code)
	{
		vk::ShaderModuleCreateInfo mod{
			vk::ShaderModuleCreateFlags{},
			code.length(),reinterpret_cast<uint32_t const*>(code.data())
		};
		return m_device->createShaderModuleUnique(mod);
	}
	enum AttachmentType
	{
		eDepthStencil,
		eColor,
		eSize
	};
	void VulkanState::createRenderPass()
	{
		vk::AttachmentDescription colorAttachment
		{
			vk::AttachmentDescriptionFlags{}
			,m_swapchain->surface_format.format
			,vk::SampleCountFlagBits::e1
			,vk::AttachmentLoadOp::eClear
			,vk::AttachmentStoreOp::eStore
			,vk::AttachmentLoadOp::eClear
			,vk::AttachmentStoreOp::eDontCare
			,vk::ImageLayout::eUndefined
			,vk::ImageLayout::eGeneral
		};
		vk::AttachmentDescription colorAttachment2
		{
			vk::AttachmentDescriptionFlags{}
			,m_swapchain->surface_format.format
			,vk::SampleCountFlagBits::e1
			,vk::AttachmentLoadOp::eDontCare
			,vk::AttachmentStoreOp::eStore
			,vk::AttachmentLoadOp::eDontCare
			,vk::AttachmentStoreOp::eDontCare
			,vk::ImageLayout::eColorAttachmentOptimal
			,vk::ImageLayout::eGeneral
		};
		vk::AttachmentDescription depthAttachment
		{
			vk::AttachmentDescriptionFlags{}
			,vk::Format::eD32Sfloat
			,vk::SampleCountFlagBits::e1
			,vk::AttachmentLoadOp::eClear
			,vk::AttachmentStoreOp::eStore
			,vk::AttachmentLoadOp::eClear
			,vk::AttachmentStoreOp::eStore
			,vk::ImageLayout::eDepthStencilAttachmentOptimal
			,vk::ImageLayout::eGeneral
		};

		vk::AttachmentReference colorAttachmentRef
		{
			0
			,vk::ImageLayout::eColorAttachmentOptimal
		};
		vector< vk::AttachmentDescription> colorAttachments2[1<<3][BasicRenderPasses::eSizeBrp];
		vector< vk::AttachmentReference> colorAttachmentRef2[BasicRenderPasses::eSizeBrp][AttachmentType::eSize];
		
		colorAttachmentRef2[BasicRenderPasses::eRgbaColorOnly][AttachmentType::eColor] = {
			{
				0
				,vk::ImageLayout::eColorAttachmentOptimal
			}
		};
		colorAttachmentRef2[BasicRenderPasses::eRgbaColorDepth][AttachmentType::eColor] = {
			vk::AttachmentReference
			{
				0
				,vk::ImageLayout::eColorAttachmentOptimal
			}
		};
		colorAttachmentRef2[BasicRenderPasses::eRgbaColorDepth][AttachmentType::eDepthStencil] = {
			vk::AttachmentReference{
				1
				,vk::ImageLayout::eDepthStencilAttachmentOptimal
			}
		};
		colorAttachmentRef2[BasicRenderPasses::eDepthOnly][AttachmentType::eDepthStencil] = {
			{
				0
				,vk::ImageLayout::eDepthStencilAttachmentOptimal
			}
		};

		for (uint8_t i = 0; i < 1 << 3; ++i)
		{
			auto& att = colorAttachments2[i][BasicRenderPasses::eRgbaColorOnly] = {
				colorAttachment
			};
			att[0].loadOp = (ClearColorBit(i))? vk::AttachmentLoadOp::eClear:vk::AttachmentLoadOp::eLoad;
		}

		for (uint8_t i = 0; i < 1 << 3; ++i)
		{
			auto& atts =colorAttachments2[i][BasicRenderPasses::eRgbaColorDepth] = {
				colorAttachment,
				depthAttachment
			};
			atts[0].loadOp = (ClearColorBit(i)) ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
			atts[1].loadOp = (ClearDepthBit(i)) ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
		}
		for (uint8_t i = 0; i < 1 << 3; ++i)
		{
			auto& att= colorAttachments2[i][BasicRenderPasses::eDepthOnly] = {
				depthAttachment
			};
			att[0].loadOp = (ClearDepthBit(i)) ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
		}

		vk::SubpassDescription subpass
		{
			vk::SubpassDescriptionFlags{}
			,vk::PipelineBindPoint::eGraphics
			,0,nullptr
			,1,&colorAttachmentRef
		};


		vk::SubpassDescription subpass2[BasicRenderPasses::eSizeBrp];
		BasicRenderPasses rp_type{};
		for([[maybe_unused]]auto& sp : subpass2)
		{
			//auto rp_type = BasicRenderPasses::eDepthOnly;
			auto& attachmentRef = colorAttachmentRef2[rp_type];

			subpass2[rp_type] = vk::SubpassDescription{
				vk::SubpassDescriptionFlags{}
				,vk::PipelineBindPoint::eGraphics
				,0,nullptr
				,hlp::arr_count(attachmentRef[AttachmentType::eColor])
				,std::data(attachmentRef[AttachmentType::eColor])
				,nullptr
				,(std::size(attachmentRef[AttachmentType::eDepthStencil]))?std::data(attachmentRef[AttachmentType::eDepthStencil]):nullptr
			};
			rp_type++;
		}
		vk::SubpassDependency dependency2[BasicRenderPasses::eSizeBrp];
		{
			rp_type = BasicRenderPasses::eRgbaColorOnly;
			dependency2[rp_type] =
				vk::SubpassDependency{
					VK_SUBPASS_EXTERNAL//src
					,0U				   //dest
					,vk::PipelineStageFlagBits::eColorAttachmentOutput
					,vk::PipelineStageFlagBits::eColorAttachmentOutput
					,vk::AccessFlags{}
					,vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
			};
		}
		{
			rp_type = BasicRenderPasses::eRgbaColorDepth;
			dependency2[rp_type] =
				vk::SubpassDependency{
					VK_SUBPASS_EXTERNAL//src
					,0U				   //dest
					,vk::PipelineStageFlagBits::eColorAttachmentOutput
					,vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests
					,vk::AccessFlags{}
					, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
					| vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite
			};
		}
		{
			rp_type = BasicRenderPasses::eDepthOnly;
			dependency2[rp_type] =
				vk::SubpassDependency{
					VK_SUBPASS_EXTERNAL//src
					,0U				   //dest
					,vk::PipelineStageFlagBits::eColorAttachmentOutput
					,vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests
					,vk::AccessFlags{}
					,vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite|vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite
			};
		}
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
		vk::RenderPassCreateInfo renderPassInfo2[BasicRenderPasses::eSizeBrp];
		rp_type = {};
		for (auto& rpi : renderPassInfo2)
		{
			
			for(uint8_t i = 0;i<1<<3;++i)
			{
				rpi=
				vk::RenderPassCreateInfo{
					vk::RenderPassCreateFlags{}
					,hlp::arr_count(colorAttachments2[i][rp_type]),std::data(colorAttachments2[i][rp_type])
					,1,&subpass2[rp_type]
					,1,&dependency2[rp_type]
				};
				m_basic_renderpasses[i][rp_type] = m_device->createRenderPassUnique(rpi, nullptr, dispatcher);
			}
			++rp_type;
		}
		m_renderpass = m_device->createRenderPassUnique(renderPassInfo, nullptr, dispatcher);
		renderPassInfo.pAttachments = &colorAttachment2;
		m_crenderpass = m_device->createRenderPassUnique(renderPassInfo, nullptr, dispatcher);

		//Temporary For RenderState
		auto& rss = view_->RenderStates();
		for (auto& rs : rss)
			rs.RenderPass() = *m_crenderpass;
	}

	void VulkanState::createImage(uint32_t width, uint32_t height, vk::Format fmt, vk::ImageTiling tiling, vk::ImageUsageFlags usage, [[maybe_unused]] vk::MemoryPropertyFlags ppts, vk::Image& image, hlp::MemoryAllocator& allocator)
	{
		if (!imageFence)
			imageFence = m_device->createFenceUnique(vk::FenceCreateInfo{ vk::FenceCreateFlags{} });

		auto ucmd_buffer = hlp::BeginSingleTimeCBufferCmd(*m_device, *m_commandpool);
		auto cmd_buffer = *ucmd_buffer;


		vk::ImageCreateInfo imageInfo = {};
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = fmt;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = usage;
		imageInfo.samples = vk::SampleCountFlagBits::e1;
		imageInfo.sharingMode = vk::SharingMode::eExclusive;
		/*
		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}
		*/
		image = m_device->createImage(imageInfo, nullptr, dispatcher);
		auto& device = m_device;

		//hlp::MemoryAllocator allocator = {*m_device,pdevice};

		//vkGetImageMemoryRequirements(device, image, &memRequirements);
		auto alloc = allocator.Allocate(image, vk::MemoryPropertyFlagBits::eDeviceLocal);
		vk::DeviceSize num_bytes = (4 * sizeof(uint32_t));

		device->bindImageMemory(image, alloc->Memory(), alloc->Offset(), vk::DispatchLoaderDefault{});

		auto&& [stagingBuffer, stagingMemory] = hlp::CreateAllocBindBuffer(pdevice, *device, num_bytes, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible, vk::DispatchLoaderDefault{});
		const void* data = nullptr;
		if (data)
			hlp::MapMemory(*device, *stagingMemory, 0, data, num_bytes, vk::DispatchLoaderDefault{});
		vk::AccessFlags src_flags = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eShaderRead;
		vk::AccessFlags dst_flags = vk::AccessFlagBits::eTransferWrite;
		vk::PipelineStageFlags shader_flags = vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader;// | vk::PipelineStageFlagBits::eTessellationControlShader | vk::PipelineStageFlagBits::eTessellationEvaluationShader;
		vk::PipelineStageFlags src_stages = shader_flags;
		vk::PipelineStageFlags dst_stages = vk::PipelineStageFlagBits::eTransfer;
		[[maybe_unused]] vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal;
		vk::ImageLayout next_layout = vk::ImageLayout::eTransferDstOptimal;

		hlp::TransitionImageLayout(cmd_buffer, m_graphics_queue, image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, next_layout);

		device->resetFences(*imageFence);
		hlp::EndSingleTimeCbufferCmd(cmd_buffer, view_->GraphicsQueue(), false, *imageFence);
		uint64_t wait_for_milli_seconds = 1;
		uint64_t wait_for_micro_seconds = wait_for_milli_seconds * 1000;
		[[maybe_unused]] uint64_t wait_for_nano_seconds = wait_for_micro_seconds * 1000;
		while (device->waitForFences(*imageFence, VK_TRUE, wait_for_milli_seconds) == vk::Result::eTimeout);

	}

	vk::UniqueImageView VulkanState::createImageView(vk::UniqueImage& img, vk::Format fmt)
	{
		vk::ImageViewCreateInfo viewInfo;
		viewInfo.image = *img;
		viewInfo.viewType = vk::ImageViewType::e2D;
		viewInfo.format = fmt;
		viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		return m_device->createImageViewUnique(viewInfo, nullptr, dispatcher);
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

	void VulkanState::createCommandBuffers()
	{
	
		vk::CommandBufferAllocateInfo allocPriInfo
		{
			*m_commandpool
			,vk::CommandBufferLevel::ePrimary
			,static_cast<uint32_t>(MaxFramesInFlight())
		};
		m_pri_commandbuffers = m_device->allocateCommandBuffersUnique(allocPriInfo, dispatcher);
		m_present_trf_commandbuffers = m_device->allocateCommandBuffersUnique(allocPriInfo, dispatcher);
		vk::CommandBufferAllocateInfo allocBlitzInfo
		{
			*m_commandpool
			,vk::CommandBufferLevel::ePrimary
			,static_cast<uint32_t>(MaxFramesInFlight())
		};
		m_blitz_commandbuffers = m_device->allocateCommandBuffersUnique(allocBlitzInfo, dispatcher);
		
	}

	void VulkanState::createSemaphores()
	{
		m_swapchain->m_graphics.CreatePresentationSignals(*view_);
		for (auto& elem : m_swapchain->m_inBetweens)
			elem->CreatePresentationSignals(*view_);
		m_swapchain->m_swapchainGraphics.CreatePresentationSignals(*view_);
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

	const RenderPassObj& VulkanState::BasicRenderPass(BasicRenderPasses type, bool clear_col , bool clear_depth ) const
	{
		return m_basic_renderpasses[GetRpClearIndex(clear_col,clear_depth)][type];
	}

	vk::UniqueCommandPool VulkanState::CreateGfxCommandPool()
	{
		vk::CommandPoolCreateInfo info
		{
			vk::CommandPoolCreateFlags{vk::CommandPoolCreateFlagBits::eResetCommandBuffer}
			,*this->m_queue_family.graphics_family
		};
		return m_device->createCommandPoolUnique(info, nullptr, dispatcher);
	}

	void VulkanState::CleanupSwapChain() {

		m_renderpass.reset();
		m_swapchain->swap_chain.reset();
		m_descriptorpool.reset();
	}

	void VulkanState::RecreateSwapChain()
	{
		m_device->waitIdle(dispatcher);
		CleanupSwapChain();


		createSwapChain();
		createImageViews();
		createRenderPass();
		//createGraphicsPipeline();
		//createFramebuffers();
		//createUniformBuffers();
		//createDescriptorPool();
		//createDescriptorSet();
		createCommandBuffers();
		createSemaphores();
	}
	void PrintFormatBlitCompatibility();
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
		createRenderPass();
		createCommandPool();

		createImageViews();
		//createDescriptorSetLayout();
		//createGraphicsPipeline();
		//createFramebuffers();

		createCommandBuffers();
		createSemaphores();
		PrintFormatBlitCompatibility();
	}

	void VulkanState::NextFrame()
	{
		current_frame = (current_frame + 1) % max_frames_in_flight;
	}

	VulkanView& VulkanState::View()
	{
		return *view_;
	}

	void VulkanState::AcquireFrame(vk::Semaphore signal)
	{
		auto cf = current_frame;
		auto& current_signal = m_swapchain->m_graphics.pSignals[current_frame];
		m_device->waitForFences(1, &*current_signal.inflight_fence(), VK_TRUE, std::numeric_limits<uint64_t>::max(), dispatcher);
		m_device->resetFences(*current_signal.inflight_fence());
		auto status = m_device->getFenceStatus(*current_signal.inflight_fence());
		auto res = m_device->acquireNextImageKHR(*m_swapchain->swap_chain, std::numeric_limits<uint32_t>::max(), signal, {}, dispatcher);
		rv = res.value;
		rvRes = res.result;
		if (res.result != vk::Result::eSuccess)
		{
			if (res.result == vk::Result::eErrorOutOfDateKHR)
			{
				RecreateSwapChain();
				return;
			}
			throw std::runtime_error("Failed to acquire next image.");
		}
		imageIndex = res.value;
		m_swapchain->curr_index = res.value;
	}
	void VulkanState::DrawFrame(vk::Semaphore wait, vk::Semaphore signal, span<RscHandle<RenderTarget>> to_transition)
	{
		//AcquireFrame();
		auto& current_signal = m_swapchain->m_graphics.pSignals[current_frame];

		waitSemaphores = wait;//*current_signal.image_available;
		readySemaphores = signal;//*current_signal.render_finished;
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

		//updateUniformBuffer(imageIndex);

	//	m_device->resetFences(1, &*current_signal.inflight_fence(), dispatcher);

		{
			[[maybe_unused]] auto& render_state = view_->CurrRenderState();
			auto& command_buffer = *m_pri_commandbuffers[current_frame];

			vk::ClearValue clearcolor[] = {
				vk::ClearValue {vk::ClearColorValue{ std::array<float,4>{0.0f,0.0f,0.0f,0.0f} } },
				vk::ClearValue { vk::ClearColorValue{ std::array<float,4>{1.0f,1.0f,1.0f,1.0f} }}
			};
			;
			if (RscHandle<RenderTarget>{}->NeedsFinalizing())
				RscHandle<RenderTarget>{}->Finalize();
			//Get default framebuffer
			auto& vkn_fb = RscHandle<RenderTarget>{}.as<VknRenderTarget>();
			auto frame_buffer = RscHandle<RenderTarget>{}.as<VknRenderTarget>().Buffer();
			//This is the part where framebuffer can be swapped (one framebuffer per renderpass)
			vk::RenderPassBeginInfo renderPassInfo
			{
				*vkn_fb.GetRenderPass(false,false)
				,frame_buffer
				,vk::Rect2D{ vk::Offset2D{}, vk::Extent2D(s_cast<uint32_t>(vkn_fb.Size().x),s_cast<uint32_t>(vkn_fb.Size().y)) }
				,s_cast<uint32_t>(std::size(clearcolor))
				,std::data(clearcolor)
			};
			vk::CommandBufferBeginInfo begin_info
			{
				vk::CommandBufferUsageFlags{}
				,nullptr//&inherit_info
			};
			command_buffer.reset(vk::CommandBufferResetFlags{},dispatcher);
			command_buffer.begin(begin_info);
			auto& tex = vkn_fb.GetColorBuffer().as<VknTexture>();
			//hlp::TransitionImageLayout(command_buffer, {}, tex.Image(),
			//	tex.format, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eGeneral
			//);
			//command_buffer.executeCommands(render_state.TransferBuffer(), dispatcher);
			//command_buffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers, dispatcher);


			for (auto& rt : to_transition)
			{
				if (rt == RscHandle<RenderTarget>{}) continue;
				auto& c_tex = rt->GetColorBuffer().as<VknTexture>();
				auto& d_tex = rt->GetDepthBuffer().as<VknTexture>();
				hlp::TransitionImageLayout(command_buffer, vk::Queue{}, c_tex.Image(), c_tex.format, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eGeneral);
				hlp::TransitionImageLayout(command_buffer, vk::Queue{}, d_tex.Image(), d_tex.format, vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::ImageLayout::eGeneral);
			}
			//////////////////////////THIS IS WHERE UBO UPDATES MVP (VIEW TRANSFORM SHOULD BE DONE HERE)/////////////////////////

			//command_buffer.executeCommands(*m_commandbuffers[m_swapchain->curr_index], dispatcher);

			//command_buffer.executeCommands(render_state.CommandBuffer(), dispatcher);
			//command_buffer.endRenderPass(dispatcher);
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
				,&waitSemaphores
				,waitStages,
				0,nullptr
				//hlp::arr_count(cmds),std::data(cmds)
				,1,&readySemaphores
			};
			vk::SubmitInfo frame_submit[] = { render_state_submit_info };

			m_graphics_queue.submit(render_state_submit_info, {}, dispatcher);
			//if (!= vk::Result::eSuccess)
			//	throw std::runtime_error("failed to submit draw command buffer!");
		}
		
		//m_present_queue.waitIdle(dispatcher);
		//PresentFrame();
	}
	vk::Semaphore CreateVkSemaphore(vk::Device device)
	{
		return device.createSemaphore(vk::SemaphoreCreateInfo{});
	}
	VulkanView& View();
	auto CreateSemaphores(size_t N)
	{
		auto semaphores_ptr = std::make_unique<vk::Semaphore[]>(N);
		span<vk::Semaphore> semaphores{ &semaphores_ptr[0],&semaphores_ptr[0]+N };
		for (auto& semaphore : semaphores)
			semaphore = CreateVkSemaphore(*View().Device());
		return std::make_pair(std::move(semaphores_ptr), semaphores);
	}
	void VulkanState::PresentFrame(vk::Semaphore wait)
	{
		auto& current_sc_signal = m_swapchain->m_graphics.pSignals[current_frame];
		auto fence = *current_sc_signal.inflight_fence();
		auto sz = m_present_trf_commandbuffers.size();
		auto cf = current_frame;
		auto& command_buffer = *m_present_trf_commandbuffers[current_frame];
		static auto tmp = CreateSemaphores(max_frames_in_flight);
		auto& [blargptr,blaargh_span] = tmp;
		auto blaargh = //CreateVkSemaphore(*View().Device());/* 
		blaargh_span[current_frame];//*/
		m_device->resetFences(1, &fence, dispatcher);
		hlp::TransitionImageLayout(command_buffer, m_graphics_queue, m_swapchain->m_swapchainGraphics.images[rv], vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eGeneral, vk::ImageLayout::ePresentSrcKHR, hlp::BeginInfo{}, hlp::SubmissionInfo{ wait, vk::PipelineStageFlagBits::eBottomOfPipe, blaargh,fence});

		vk::SwapchainKHR swapchains[] = { *m_swapchain->swap_chain };

		vk::PresentInfoKHR presentInfo
		{
			1,&blaargh
			,1,swapchains
			,&imageIndex
			,nullptr
		};
		try
		{

			try
			{
				//hlp::TransitionImageLayout(command_buffer, m_graphics_queue, m_swapchain->swapchain_images[rv], vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::ePresentSrcKHR);
				rvRes = m_present_queue.presentKHR(presentInfo, dispatcher);
				if (
					rvRes
					!= vk::Result::eSuccess || m_ScreenResized)
				{
					if (m_ScreenResized)
						rvRes = vk::Result::eSuboptimalKHR;
				}
			}
			catch (const vk::OutOfDateKHRError&)
			{
				rvRes = vk::Result::eErrorOutOfDateKHR;
			}
			switch (rvRes)
			{
			case vk::Result::eErrorOutOfDateKHR:
			case vk::Result::eSuboptimalKHR:
				m_ScreenResized = false;
				m_ScreenResizedForImGui = true;
				RecreateSwapChain();
				break;
			case vk::Result::eSuccess:
				break;
			default:
				throw std::runtime_error("Failed to present");
				break;
			}
		}
		catch (const vk::Error& err)
		{
			hlp::cerr() << "Error presenting: " << err.what() << "\n";
			return;
		}

		;


		NextFrame();
	}

	void VulkanState::PresentFrame2()
	{
		auto cf = current_frame;
		auto& current_sc_signal = m_swapchain->m_swapchainGraphics.pSignals[current_frame];

		//Already done by acquire frame
		//m_device->waitForFences(1, &*current_sc_signal.inflight_fence, VK_TRUE, std::numeric_limits<uint64_t>::max(), dispatcher); 
		auto status = m_device->getFenceStatus(*current_sc_signal.inflight_fence());
		auto& command_buffer = *m_blitz_commandbuffers[current_frame];
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eBottomOfPipe// vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eTransfer
	};

		vk::CommandBufferBeginInfo begin_info
		{
			vk::CommandBufferUsageFlags{}
			,nullptr//&inherit_info
		};
		command_buffer.reset(vk::CommandBufferResetFlags{}, dispatcher);
		command_buffer.begin(begin_info);

		vk::Fence* prevFence = nullptr;
		bool was_blit = false;
		for (unsigned i = 0; i < m_swapchain->m_inBetweens.size(); ++i)
		{
			auto& elem = m_swapchain->m_inBetweens[i];


			if (elem->enabled)
			{
				auto& current_signal = elem->pSignals[current_frame];

				waitSemaphores = *elem->pSignals[current_frame].render_finished;
				readySemaphores = 
					((s_cast<size_t>(i) + 1) == m_swapchain->m_inBetweens.size()) 
					? *current_signal.render_finished 
					: *m_swapchain->m_inBetweens[s_cast<size_t>(i) + 1]->pSignals[current_frame].render_finished;

				//updateUniformBuffer(imageIndex);

				//m_device->resetFences(1, &*prevFence, dispatcher);
				auto sc_image = m_swapchain->m_swapchainGraphics.images[rv];
				auto src_image = elem->images[rv];
				if (sc_image != src_image)
				{
					if(prevFence)
					m_device->resetFences(1, &*prevFence, dispatcher);

					
					//hlp::TransitionImageLayout(command_buffer, m_graphics_queue, src_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::ePresentSrcKHR, vk::ImageLayout::eTransferSrcOptimal);
					//hlp::TransitionImageLayout(false, command_buffer, m_graphics_queue, src_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal);

					//hlp::TransitionImageLayout(command_buffer, m_graphics_queue, m_swapchain->swapchain_images[rv], vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal);

					hlp::TransitionImageLayout(command_buffer, m_graphics_queue, src_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal );
					hlp::TransitionImageLayout(command_buffer, m_graphics_queue, sc_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

					vk::ImageBlit imgBlit{};

					// Source
					imgBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imgBlit.srcSubresource.layerCount = 1;
					imgBlit.srcOffsets[0] = { 0,0,0 };
					imgBlit.srcOffsets[1].x = m_swapchain->extent.width;
					imgBlit.srcOffsets[1].y = m_swapchain->extent.height;
					imgBlit.srcOffsets[1].z = 1;

					// Destination
					imgBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imgBlit.dstSubresource.layerCount = 1;
					imgBlit.dstOffsets[0] = { 0,0,0 };
					imgBlit.dstOffsets[1].x = m_swapchain->extent.width;
					imgBlit.dstOffsets[1].y = m_swapchain->extent.height;
					imgBlit.dstOffsets[1].z = 1;

					//Idk why, but elem->images[rv]'s layout is currently vk::ImageLayout::eTransferDstOptimal, not src. Might need to transition first.
					command_buffer.blitImage(src_image, vk::ImageLayout::eTransferSrcOptimal, m_swapchain->m_swapchainGraphics.images[rv], vk::ImageLayout::eTransferDstOptimal, imgBlit, vk::Filter::eLinear, dispatcher);
					hlp::TransitionImageLayout( command_buffer, m_graphics_queue, src_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eGeneral);

					was_blit = true;
					//hlp::TransitionImageLayout(command_buffer, m_graphics_queue, m_swapchain->swapchain_images[rv], vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::ePresentSrcKHR, false);
					//hlp::TransitionImageLayout(true, command_buffer, m_graphics_queue, m_swapchain->m_swapchainGraphics.images[rv], vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
					//BeginFrame();
					prevFence = &*(elem->pSignals[current_frame].inflight_fence());

				}
			}
		}
		vk::Semaphore next = readySemaphores;
		if (m_swapchain->m_inBetweens.empty())
		{

			next = waitSemaphores = *m_swapchain->m_graphics.pSignals[current_frame].render_finished;
			readySemaphores = *m_swapchain->m_swapchainGraphics.pSignals[current_frame].render_finished;

			if (m_swapchain->m_swapchainGraphics.images[rv] != m_swapchain->m_graphics.images[rv])
			{
				if(prevFence)
					m_device->resetFences(1, prevFence, dispatcher);
				//->resetFences(1, &*current_signal.master_fence, dispatcher);

				hlp::TransitionImageLayout(command_buffer, m_graphics_queue, m_swapchain->m_swapchainGraphics.images[rv], vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
				hlp::TransitionImageLayout(command_buffer, m_graphics_queue, m_swapchain->m_graphics.images[rv], vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal           );


				vk::CommandBufferBeginInfo beg_info
				{
					vk::CommandBufferUsageFlags{}
					,nullptr//&inherit_info
				};

				//hlp::TransitionImageLayout(command_buffer, m_graphics_queue, m_swapchain->swapchain_images[rv], vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal);

				//command_buffer.reset(vk::CommandBufferResetFlags{}, dispatcher);
				//command_buffer.begin(begin_info);

				vk::ImageBlit imgBlit{};

				// Source
				imgBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
				imgBlit.srcSubresource.layerCount = 1;
				imgBlit.srcOffsets[0] = { 0,0,0 };
				imgBlit.srcOffsets[1].x = m_swapchain->extent.width;
				imgBlit.srcOffsets[1].y = m_swapchain->extent.height;
				imgBlit.srcOffsets[1].z = 1;

				// Destination
				imgBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
				imgBlit.dstSubresource.layerCount = 1;
				if (&Core::GetSystem<IEditor>())
				{
					imgBlit.dstOffsets[0] = { 0,0,0 };
					imgBlit.dstOffsets[1].y = m_swapchain->extent.height;
				}
				else
				{
					
					imgBlit.dstOffsets[0] = { 0,s_cast<int32_t>(m_swapchain->extent.height),0 };					
					imgBlit.dstOffsets[1].y = 0;
				}
				imgBlit.dstOffsets[1].x = m_swapchain->extent.width;
				imgBlit.dstOffsets[1].z = 1;

				command_buffer.blitImage(m_swapchain->m_graphics.images[rv], vk::ImageLayout::eTransferSrcOptimal, m_swapchain->m_swapchainGraphics.images[rv], vk::ImageLayout::eTransferDstOptimal, imgBlit, vk::Filter::eLinear, dispatcher);
				hlp::TransitionImageLayout(command_buffer, m_graphics_queue, m_swapchain->m_graphics.images[rv], vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eGeneral);
				was_blit = true;

				next = readySemaphores;

				
			}
		}

		//hlp::TransitionImageLayout(command_buffer, m_graphics_queue, m_swapchain->swapchain_images[rv], vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::ePresentSrcKHR);
		if (was_blit)
		{
			hlp::TransitionImageLayout(command_buffer, m_graphics_queue, m_swapchain->m_swapchainGraphics.images[rv], vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eGeneral);
			command_buffer.end();
			vk::CommandBuffer cmds[] =
			{
				command_buffer,
			};
			vk::SubmitInfo render_state_submit_info
			{
				1
				,&waitSemaphores
				,waitStages
				,hlp::arr_count(cmds),std::data(cmds)
				,1,&readySemaphores
			};


			vk::SubmitInfo frame_submit[] = { render_state_submit_info };
			if (m_graphics_queue.submit(hlp::arr_count(frame_submit), std::data(frame_submit), vk::Fence{}, dispatcher) != vk::Result::eSuccess)
				throw std::runtime_error("failed to submit draw command buffer!");
		}
		else {
		
			//Dis be hack
			//hlp::TransitionImageLayout(command_buffer, m_graphics_queue, m_swapchain->m_swapchainGraphics.images[rv], vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::ePresentSrcKHR, vk::ImageLayout::eTransferDstOptimal, next, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eTransfer, readySemaphores);
			next = waitSemaphores;
		
		}

		PresentFrame(next);
		return;
	}

	void VulkanState::OnResize()
	{
		m_ScreenResized = true;
		m_ScreenResizedForImGui = true;
	}

	void VulkanState::Cleanup()
	{
		m_device->waitIdle();
		m_swapchain.reset();
		//instance.reset();
	}

	VulkanState::VulkanState() : view_{ std::make_unique<VulkanView>(*this) }
	{
	}

	VulkanState::~VulkanState()
	{
		m_graphics_queue.waitIdle();
		m_present_queue.waitIdle();
		m_device->waitIdle();

		for (auto& ucmd : m_pri_commandbuffers)
			ucmd->reset(vk::CommandBufferResetFlagBits::eReleaseResources);
		for (auto& ucmd : m_present_trf_commandbuffers)
			ucmd->reset(vk::CommandBufferResetFlagBits::eReleaseResources);
		for( auto& ucmd : m_blitz_commandbuffers)
			ucmd->reset(vk::CommandBufferResetFlagBits::eReleaseResources);
	}

	VulkanResourceManager& VulkanState::ResourceManager()
	{
		return rsc_manager;
	}

	void FrameObjects::FrameReset()
	{
		ubo_manager.Clear(); //Clear the previous frame's UBOs
		pools.Reset(); //Reset the previous frame's descriptors
	}
	
}