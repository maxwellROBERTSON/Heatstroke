#include "Debug.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <iostream>

namespace Engine {
	namespace Debug
	{
		bool DebugManager::initImGui(VulkanWindow aWindow)
		{

			GLFWwindow* window = aWindow.window;
			// start dearImGui context
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui_ImplGlfw_InitForVulkan(window, true);
			std::cout << "CHECKPOINT 1" << std::endl;

			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsLight();

			// Setup Platform/Renderer backends
			ImGui_ImplGlfw_InitForVulkan(window, true);
			ImGui_ImplVulkan_InitInfo init_info = {};
			//init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
			init_info.Instance = aWindow.instance;
			init_info.PhysicalDevice = aWindow.physicalDevice;
			init_info.Device = aWindow.device.get()->device;
			init_info.QueueFamily = aWindow.graphicsFamilyIndex;
			init_info.Queue = aWindow.graphicsQueue;
			//init_info.PipelineCache = g_PipelineCache;
			init_info.DescriptorPoolSize = 1; // replaces need to provide descriptor pool, imgui creats one for us
			//init_info.RenderPass = wd->RenderPass;
			init_info.Subpass = 0;
			init_info.MinImageCount = 0;
			init_info.ImageCount = aWindow.swapImages.size();
			init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			//init_info.Allocator = g_Allocator;
			//init_info.CheckVkResultFn = check_vk_result;
			ImGui_ImplVulkan_Init(&init_info);

			// DescriptorPool, Renderpass, CommandBuffer, Framebuffer
			// DescriptorPool can be made by imgui

			return true;
		}
	}
}

