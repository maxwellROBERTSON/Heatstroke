#pragma once

#include <vector>

#include "../../vulkan/objects/Texture.hpp"
#include "../../vulkan/objects/VkObjects.hpp"

#include <vulkan/vulkan.h>
#include <stb_image.h>

namespace Engine {

	class Skybox {
	public:

		Skybox(VulkanContext* context, std::vector<const char*> filenames);
		~Skybox() {
			std::fprintf(stderr, "Skybox destroyed\n");
		}

		void loadFiles(std::vector<const char*> filenames);
		void bind(VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout);

	private:
		VulkanContext* context;

		int width;
		int height;
		int channels;

		stbi_uc* skyboxImageData[6];

		vk::Texture image;
		vk::ImageView imageView;
		vk::Sampler sampler;

		VkDescriptorSet descriptor = VK_NULL_HANDLE;
		
		void uploadToGPU();
		void makeDescriptor();
	};

}