#include "pch.h"
#include "Vulkan_ImGui_Interface.h"

//#include <vulkan/VulkanWin32GraphicsSystem.h>

//Imgui
#include <vkn/VulkanState.h>
#include <vkn/VulkanView.h>
#include <vkn/BufferHelpers.h>

#include <imgui/imgui.h>

//Helper header from imgui for vulkan implementation
#include <editorstatic/imgui/Imgui_impl_vulkan.h>
#include <editorstatic/imgui/ImGUIImpl_Win32.h>

namespace idk
{
	static void check_vk_result(VkResult err)
	{
		if (err == 0) return;
		printf("VkResult %d\n", err);
		if (err < 0)
			abort();
	}

	static void check_vk_result(vk::Result err)
	{
		if (err == vk::Result::eSuccess) return;
		printf("VkResult %d\n", err);
		if (err != vk::Result::eSuccess)
			abort();
	}

	namespace edt {
		VI_Interface::VI_Interface(vkn::VulkanState* v)
			:vkObj{v}
		{}

		void VI_Interface::Init()
		{

			//Creation
			vkn::VulkanView& vknViews = vkObj->View();

			editorInit.edt_imageCount = vkn::hlp::arr_count(vknViews.Swapchain().image_views);
			editorControls.edt_imageCount = editorInit.edt_imageCount;
			editorControls.edt_frameIndex;
			editorControls.edt_semaphoreIndex;
			editorInit.edt_min_imageCount = 2;
			editorInit.edt_pipeCache;

			//Setup vulkan window for imgui
			ImGuiRecreateSwapChain();
			ImGuiRecreateCommandBuffer();

			ImGui_ImplVulkan_InitInfo info{};
			info.Instance = *vknViews.Instance();
			info.PhysicalDevice = vknViews.PDevice();
			info.Device = *vknViews.Device();

			info.QueueFamily = *vknViews.QueueFamily().graphics_family;
			info.Queue = vknViews.GraphicsQueue();

			info.PipelineCache = VK_NULL_HANDLE;

			vk::DescriptorPoolSize pSizes[] =
			{
				{ vk::DescriptorType::eSampler, 1000 },
				{ vk::DescriptorType::eCombinedImageSampler, 1000 },
				{ vk::DescriptorType::eSampledImage, 1000 },
				{ vk::DescriptorType::eStorageImage, 1000 },
				{ vk::DescriptorType::eUniformTexelBuffer, 1000 },
				{ vk::DescriptorType::eStorageTexelBuffer, 1000 },
				{ vk::DescriptorType::eUniformBuffer, 1000 },
				{ vk::DescriptorType::eStorageBuffer, 1000 },
				{ vk::DescriptorType::eUniformBufferDynamic, 1000 },
				{ vk::DescriptorType::eStorageBufferDynamic, 1000 },
				{ vk::DescriptorType::eInputAttachment, 1000 }
			};
			vk::DescriptorPoolCreateInfo pInfo{
				vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
				1000 * vkn::hlp::arr_count(pSizes),
				vkn::hlp::arr_count(pSizes),
				pSizes
			};
			//info.DescriptorPool = *(vknViews.Device()->createDescriptorPoolUnique(pInfo, nullptr, vknViews.Dispatcher()));
			VkResult res = vkCreateDescriptorPool(*vknViews.Device(),&s_cast<VkDescriptorPoolCreateInfo&>(pInfo),nullptr,&info.DescriptorPool);
			res;
			info.Allocator = nullptr;
			info.MinImageCount = editorInit.edt_min_imageCount;
			info.ImageCount = editorInit.edt_imageCount;
			info.CheckVkResultFn = check_vk_result;

			//IMGUI setup
			IMGUI_CHECKVERSION();

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2(vknViews.Swapchain().extent.width, vknViews.Swapchain().extent.height);

			//Imgui Style
			ImGui::StyleColorsClassic();

			//Platform/renderer bindings
			ImGui_ImplWin32_Init(vknViews.GetWindowsInfo().wnd);
			ImGui_ImplVulkan_Init(&info, *(vknViews.Renderpass()));

			//Upload fonts leave it to later
			// Upload Fonts
			{
				// Use any command queue
				vk::CommandBuffer& command_buffer = *editorControls.edt_frames[editorControls.edt_frameIndex].edt_cBuffer;

				//err = vkResetCommandPool(g_Device, command_pool, 0);
				//check_vk_result(err);
				vknViews.Device()->resetCommandPool(*editorControls.edt_frames[editorControls.edt_frameIndex].edt_cPool,vk::CommandPoolResetFlags::Flags(),vknViews.Dispatcher());
				vk::CommandBufferBeginInfo begin_info = {};
				begin_info.flags |= vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
				//err = vkBeginCommandBuffer(command_buffer, &begin_info);
				//check_vk_result(err);
				//editorControls.edt_frames[editorControls.edt_frameIndex].edt_cBuffer->begin(begin_info,vknViews.Dispatcher());
				command_buffer.begin(begin_info, vknViews.Dispatcher());

				ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

				vk::SubmitInfo end_info = {};
				end_info.commandBufferCount = 1;
				end_info.pCommandBuffers = &command_buffer;
				//err = vkEndCommandBuffer(command_buffer);
				//check_vk_result(err);
				command_buffer.end(vknViews.Dispatcher());

				//err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
				//check_vk_result(err);
				vknViews.GraphicsQueue().submit(1, &end_info, vk::Fence{}, vknViews.Dispatcher());

				//err = vkDeviceWaitIdle(g_Device);
				//check_vk_result(err);
				vknViews.Device()->waitIdle(vknViews.Dispatcher());
				ImGui_ImplVulkan_DestroyFontUploadObjects();
			}
		}

		void VI_Interface::Shutdown()
		{
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}

		void VI_Interface::ImGuiFrameBegin()
		{
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}

		void VI_Interface::ImGuiFrameUpdate()
		{
			vkn::VulkanView& vknViews = vkObj->View();

			bool& resize = vkObj->View().ImguiResize();
			if (resize)
			{
				resize = false;
				ImGui_ImplVulkan_SetMinImageCount(editorInit.edt_min_imageCount);
				ImGuiRecreateSwapChain();
				ImGuiRecreateCommandBuffer();

				ImGuiIO& io = ImGui::GetIO();
				io.DisplaySize = ImVec2(vknViews.Swapchain().extent.width, vknViews.Swapchain().extent.height);
			}

			ImGuiFrameBegin();

			if (editorControls.demoWindow)
				ImGui::ShowDemoWindow(&editorControls.demoWindow);

			//////////////////////////IMGUI DATA HANDLING//////////////////////
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &editorControls.demoWindow);      // Edit bools storing our window open/close state

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			//ImGui::ColorEdit3("clear color", (float*)& clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
			///////////////////////////////END///////////////////////////////

			ImGuiFrameEnd();
		}

		void VI_Interface::ImGuiFrameEnd()
		{
			ImGui::Render();
			ImGuiFrameRender();
			ImGuiFramePresent();
		}

		void VI_Interface::ImGuiFrameRender()
		{
			vkn::VulkanView& vknViews = vkObj->View();

			vk::Semaphore image_acquired_semaphore = *editorControls.edt_frameSemophores[editorControls.edt_semaphoreIndex].edt_imageAvailable;
			vk::Semaphore render_complete_semaphore = *editorControls.edt_frameSemophores[editorControls.edt_semaphoreIndex].edt_renderFinished;
			
			auto result = vknViews.Device()->acquireNextImageKHR(*vknViews.Swapchain().swap_chain, std::numeric_limits<uint32_t>::max(), image_acquired_semaphore, {},vknViews.Dispatcher());
			editorControls.edt_frameIndex = result.value;
			//err = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
			//check_vk_result(err);

			EditorFrame* fd = &(editorControls.edt_frames[editorControls.edt_frameIndex]);
			{
				//err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
				//check_vk_result(err);
				vknViews.Device()->waitForFences(1, &*fd->edt_fence, VK_TRUE, std::numeric_limits<uint64_t>::max(), vknViews.Dispatcher());

				//err = vkResetFences(g_Device, 1, &fd->Fence);
				//check_vk_result(err);
				vknViews.Device()->resetFences(1, &*fd->edt_fence, vknViews.Dispatcher());

			}
			{
				//err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
				//check_vk_result(err);
				vknViews.Device()->resetCommandPool(*fd->edt_cPool,vk::CommandPoolResetFlags::Flags(),vknViews.Dispatcher());
				vk::CommandBufferBeginInfo info = {};
				//info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				info.flags |= vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
				//err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
				//check_vk_result(err);
				fd->edt_cBuffer->begin(info, vknViews.Dispatcher());
			}
			{
				vk::RenderPassBeginInfo info{};
				info.renderPass = *editorControls.edt_renderPass;
				info.framebuffer = *fd->edt_framebuffer;
				info.renderArea.extent.width = vknViews.Swapchain().extent.width;
				info.renderArea.extent.height = vknViews.Swapchain().extent.height;
				info.clearValueCount = 1;
				info.pClearValues = &editorControls.edt_clearValue;
				//vkCmdBeginRenderPass(*fd->edt_cBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
				fd->edt_cBuffer->beginRenderPass(info,vk::SubpassContents::eInline,vknViews.Dispatcher());
			}

			// Record Imgui Draw Data and draw funcs into command buffer
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *fd->edt_cBuffer);

			// Submit command buffer
			//vkCmdEndRenderPass(fd->CommandBuffer);
			fd->edt_cBuffer->endRenderPass(vknViews.Dispatcher());
			{
				vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
				vk::SubmitInfo info = {};
				info.waitSemaphoreCount = 1;
				info.pWaitSemaphores = &image_acquired_semaphore;
				info.pWaitDstStageMask = &wait_stage;
				info.commandBufferCount = 1;
				info.pCommandBuffers = &*fd->edt_cBuffer;
				info.signalSemaphoreCount = 1;
				info.pSignalSemaphores = &render_complete_semaphore;

				//err = vkEndCommandBuffer(fd->CommandBuffer);
				//check_vk_result(err);
				fd->edt_cBuffer->end(vknViews.Dispatcher());
				
				//err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
				vknViews.GraphicsQueue().submit(1,&info,*fd->edt_fence,vknViews.Dispatcher());
				//check_vk_result(err);
			}
		}

		void VI_Interface::ImGuiFramePresent()
		{
			vkn::VulkanView& vknViews = vkObj->View();

			vk::Semaphore render_complete_semaphore = *editorControls.edt_frameSemophores[editorControls.edt_semaphoreIndex].edt_renderFinished;
			vk::PresentInfoKHR info = {};
			info.waitSemaphoreCount = 1;
			info.pWaitSemaphores = &render_complete_semaphore;
			info.swapchainCount = 1;
			info.pSwapchains = &*vknViews.Swapchain().swap_chain;
			info.pImageIndices = &editorControls.edt_frameIndex;
			//VkResult err = vkQueuePresentKHR(vknViews.GraphicsQueue(), &info);
			vk::Result err = vknViews.GraphicsQueue().presentKHR(info, vknViews.Dispatcher());
			check_vk_result(err);
			editorControls.edt_semaphoreIndex = (editorControls.edt_semaphoreIndex + 1) % editorControls.edt_imageCount; // Now we can use the next set of semaphores

		}

		void VI_Interface::ImGuiRecreateSwapChain()
		{
			//Recreation of swapchain is already done in the vulkan state recreation

			//What is required to be recreated for imGui swapchain will be done here
			idk::vkn::VulkanView& vknViews = vkObj->View();

			if (editorInit.edt_min_imageCount == 0)
				editorInit.edt_min_imageCount = ImGui_ImplVulkanH_GetMinImageCountFromPresentMode(static_cast<VkPresentModeKHR>(vknViews.Swapchain().present_mode));

			for (uint32_t i = 0; i < editorInit.edt_imageCount; ++i)
			{
				editorControls.edt_frames.push_back(EditorFrame{});
				editorControls.edt_frameSemophores.push_back(EditorPresentationSignal{});
			}

			vk::AttachmentDescription attachment{};
			attachment.format = vknViews.Swapchain().surface_format.format;
			attachment.samples = vk::SampleCountFlagBits::e1;
			attachment.loadOp = static_cast<vk::AttachmentLoadOp>((editorControls.edt_clearEnable ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE));
			attachment.storeOp = vk::AttachmentStoreOp::eStore;
			attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
			attachment.initialLayout = vk::ImageLayout::eUndefined;
			attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
			vk::AttachmentReference color_attachment = {};
			color_attachment.attachment = 0;
			color_attachment.layout = vk::ImageLayout::eColorAttachmentOptimal;
			vk::SubpassDescription subpass = {};
			subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &color_attachment;
			vk::SubpassDependency dependency = {};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
			vk::RenderPassCreateInfo info = {};
			info.attachmentCount = 1;
			info.pAttachments = &attachment;
			info.subpassCount = 1;
			info.pSubpasses = &subpass;
			info.dependencyCount = 1;
			info.pDependencies = &dependency;

			editorControls.edt_renderPass = vknViews.Device()->createRenderPassUnique(info, nullptr, vknViews.Dispatcher());

			vk::ImageView att[1];
			for (uint32_t i = 0; i < editorControls.edt_imageCount; i++)
			{
				EditorFrame* fd = &editorControls.edt_frames[i];
				att[0] = fd->edt_backbufferView = *vknViews.Swapchain().image_views[i];
				vk::FramebufferCreateInfo fbInfo{
					vk::FramebufferCreateFlags{},
					*editorControls.edt_renderPass,
					1,
					att,
					vknViews.Swapchain().extent.width,
					vknViews.Swapchain().extent.height,
					1
				};
				fd->edt_framebuffer = vknViews.Device()->createFramebufferUnique(fbInfo, nullptr, vknViews.Dispatcher());
			}

		}
		void VI_Interface::ImGuiRecreateCommandBuffer()
		{
			vkn::VulkanView& vknViews = vkObj->View();

			for (uint32_t i = 0; i < editorControls.edt_imageCount; i++)
			{
				EditorFrame* fd = &editorControls.edt_frames[i];
				EditorPresentationSignal* fsd = &editorControls.edt_frameSemophores[i];
				{
					vk::CommandPoolCreateInfo info = {};
					info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
					info.queueFamilyIndex = vknViews.QueueFamily().graphics_family.value();
					fd->edt_cPool = vknViews.Device()->createCommandPoolUnique(info, nullptr, vknViews.Dispatcher());
				}
				{
					vk::CommandBufferAllocateInfo info = {};
					info.commandPool = *fd->edt_cPool;
					info.level = vk::CommandBufferLevel::ePrimary;
					info.commandBufferCount = 1;
					fd->edt_cBuffer = std::move(vknViews.Device()->allocateCommandBuffersUnique(info, vknViews.Dispatcher()).front());
					//err = vkAllocateCommandBuffers(device, &info, &fd->CommandBuffer);
					//check_vk_result(err);
				}
				{
					vk::FenceCreateInfo info = {};
					info.flags = vk::FenceCreateFlagBits::eSignaled;
					//err = vkCreateFence(device, &info, allocator, &fd->Fence);
					//check_vk_result(err);
					fd->edt_fence = vknViews.Device()->createFenceUnique(info, nullptr, vknViews.Dispatcher());
				}
				{
					vk::SemaphoreCreateInfo info = {};
					fsd->edt_imageAvailable = vknViews.Device()->createSemaphoreUnique(info, nullptr, vknViews.Dispatcher());
					fsd->edt_renderFinished = vknViews.Device()->createSemaphoreUnique(info, nullptr, vknViews.Dispatcher());

					//info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
					//err = vkCreateSemaphore(device, &info, allocator, &fsd->ImageAcquiredSemaphore);
					///check_vk_result(err);
					//err = vkCreateSemaphore(device, &info, allocator, &fsd->RenderCompleteSemaphore);
					//check_vk_result(err);
				}
			}
		}
	};
};