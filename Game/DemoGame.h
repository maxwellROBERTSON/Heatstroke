#pragma once

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Engine/Core/Game.h"

#include "../Engine/ECS/ComponentTypeRegistry.hpp"
#include "../Engine/ECS/EntityManager.hpp"
#include "../Engine/ECS/RenderComponent.hpp"
#include "UserComponents/TestUserComponent.hpp"

#include "../Engine/vulkan/VulkanContext.hpp"

#include "Camera.hpp"
#include "Uniforms.hpp"

#include "../Engine/Events/Event.h"
#include "../Engine/Events/KeyEvent.h"
#include "../Engine/Events/MouseEvent.h"

class FPSTest : public Engine::Game
{
public:
	FPSTest() : Engine::Game("FPS Test Game")
	{
		this->Init();
	}
	virtual void Init() override;
	virtual void Update() override;
	virtual void OnEvent(Engine::Event& e) override;

	void registerComponents();
	void initialiseModels();
	void Render();

	~FPSTest() {
		for (Engine::vk::Model& model : models)
			model.destroy();

		this->GetContext().allocator.reset();
		this->GetContext().window.reset();
	};



public:
	Camera camera;
	std::vector<Engine::vk::Model> models;
	bool recreateSwapchain;
};

void updateSceneUniform(glsl::SceneUniform& aScene, Camera& camera, std::uint32_t aFramebufferWidth, std::uint32_t aFramebufferHeight);
void updateModelMatrices(const Engine::VulkanContext& aContext, glsl::ModelMatricesUniform& aModelMatrices, Engine::vk::Buffer& aBuffer, std::vector<Engine::vk::Model>& aModels, std::size_t dynamicAlignment);