#include "../Network/Helpers/GameConfig.hpp"
#include "GUI.hpp"

#include "../../Game/DemoGame.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Getters

ImFont* GUI::GetFont(std::string s)
{
	auto it = fonts.find(s);
	if (it != fonts.end())
		return it->second;
	return nullptr;
}

std::tuple<Engine::vk::Sampler, Engine::vk::Texture, Engine::vk::ImageView, VkDescriptorSet>* GUI::GetImage(std::string s)
{
	auto it = images.find(s);
	if (it != images.end())
		return &it->second;
	return nullptr;
}

bool GUI::GetGUIMode(std::string s)
{
	for (int i = 0; i < guiModes.size(); i++)
	{
		if (guiModes[i] == s)
			return activeGUIModes[i];
	}
	return false;
}

// Setters

void GUI::initGUI()
{
	IMGUI_CHECKVERSION();
	ImGuiContext* ImGuiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext(ImGuiContext);
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForVulkan(this->game->GetContext().getGLFWWindow(), true);
	Engine::VulkanWindow* window = this->game->GetContext().window.get();
	ImGui_ImplVulkan_InitInfo init_info = {};

	//init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
	init_info.Instance = window->instance;
	init_info.PhysicalDevice = window->physicalDevice;
	init_info.Device = window->device->device;
	init_info.QueueFamily = window->graphicsFamilyIndex;
	init_info.Queue = window->graphicsQueue;
	init_info.DescriptorPool = window->device->dPool;
	init_info.RenderPass = this->game->getRenderer().getRenderPassHandle("gui");
	init_info.Subpass = 0;

	VkSurfaceCapabilitiesKHR caps;
	if (const auto res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(window->physicalDevice, window->surface, &caps); VK_SUCCESS != res)
		throw Utils::Error("Unable to get surface capabilities\n vkGetPhysicalDeviceSurfaceCapabilitiesKHR() returned %s", Utils::toString(res).c_str());

	std::uint32_t imageCount = 2;

	if (imageCount < caps.minImageCount + 1)
		imageCount = caps.minImageCount + 1;

	if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
		imageCount = caps.maxImageCount;

	init_info.MinImageCount = caps.minImageCount < 2 ? 2 : caps.minImageCount;
	init_info.ImageCount = imageCount;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui_ImplVulkan_Init(&init_info);
}

void GUI::makeGUI()
{
	int width;
	int height;
	glfwGetFramebufferSize(game->GetContext().getGLFWWindow(), &width, &height);

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	for (int i = 0; i < activeGUIModes.size(); i++)
	{
		if (activeGUIModes[i])
		{
			functions[guiModes[i]](&width, &height);
		}
	}

	ImGui::EndFrame();

	ImGui::Render();
}

void GUI::AddFunction(std::string s, std::function<void(int*, int*)> func)
{
	functions.emplace(s, func);
	guiModes.emplace_back(s);
	activeGUIModes.emplace_back(false);
}

void GUI::AddFont(std::string s, const char* filename, float size)
{
	fonts.emplace(s, ImGui::GetIO().Fonts->AddFontFromFileTTF(filename, size));
}

void GUI::AddTexture(std::string s, const char* filename)
{
	images.emplace(s, std::tuple<Engine::vk::Sampler, Engine::vk::Texture, Engine::vk::ImageView, VkDescriptorSet>{});
	Engine::vk::SamplerInfo samplerInfo;
	samplerInfo.magFilter = VK_FILTER_NEAREST;
	samplerInfo.minFilter = VK_FILTER_NEAREST;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	std::tuple<Engine::vk::Sampler, Engine::vk::Texture, Engine::vk::ImageView, VkDescriptorSet>* imageTuple = GetImage(s);
	std::get<0>(*imageTuple) = createTextureSampler(*game->GetContext().window, samplerInfo);

	std::get<1>(*imageTuple) = Engine::vk::createTexture(game->GetContext(), "ImGui Texture", loadPngAsTinyImage(filename), VK_FORMAT_R8G8B8A8_UNORM, std::get<0>(*imageTuple).handle);

	std::get<2>(*imageTuple) = Engine::vk::createImageView(*game->GetContext().window, std::get<1>(*imageTuple).image, VK_FORMAT_R8G8B8A8_UNORM);

	std::get<3>(*imageTuple) = ImGui_ImplVulkan_AddTexture(
		std::get<0>(*imageTuple).handle,
		std::get<2>(*imageTuple).handle,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);
}

void GUI::ToggleGUIMode(std::string s)
{
	for (int i = 0; i < guiModes.size(); i++)
	{
		if (guiModes[i] == s)
		{
			activeGUIModes[i] = !activeGUIModes[i];
			std::cout << "Toggled GUI Mode " << i << std::endl;
		}
	}
}

tinygltf::Image GUI::loadPngAsTinyImage(const char* filename)
{
	tinygltf::Image image;
	int width, height, channels;

	// Load image as 4-channel RGBA
	unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
	if (!data)
		throw std::runtime_error(std::string("Failed to load PNG: ") + filename);

	size_t dataSize = width * height * 4; // RGBA
	image.image.assign(data, data + dataSize);
	image.width = width;
	image.height = height;
	image.component = 4; // RGBA
	image.bits = 8;
	image.pixel_type = TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;
	image.name = filename;

	stbi_image_free(data);
	return image;
}

void GUI::ResetGUIModes()
{
	for (int i = 0; i < guiModes.size(); i++)
	{
		activeGUIModes[i] = false;
	}
}
