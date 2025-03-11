//#include "GameLoop.hpp"

#include <yojimbo.h>

#include "../Engine/Network/Client/GameClient.hpp"
#include "../Engine/Network/Helpers/GameAdapter.hpp"
#include "../Engine/Network/Helpers/GameConfig.hpp"
#include "../Engine/Network/Helpers/GetLocalIPAddress.hpp"
#include "../Engine/Network/Server/GameServer.hpp"

#include "../Engine/Physics/PhysicsWorld.hpp"
#include "DemoGame.hpp"

//int main() try {
//    // This manual scope is very important, it ensures the objects in models have their associated
//    // objects' destructors called BEFORE we destroy the vulkan device. Do not move the models vector
//    // out of this scope unless you also handle the object lifetimes from them.
//    {
//        /*PhysicsWorld physicsWorld;
//		physicsWorld.init();*/
//
//        std::vector<Engine::vk::Model> models;
//		ComponentTypeRegistry registry = ComponentTypeRegistry::Get();
//		EntityManager entityManager = EntityManager(&registry);
//
//		initialiseGame(registry, entityManager);
//        initialiseModels(models);
//		runGameLoop(models, registry, entityManager);



int main() try
{
	std::unique_ptr<FPSTest> game = std::make_unique<FPSTest>();
	game->Run();
	game.get()->GetContext().allocator.reset();
	game.get()->GetContext().window.reset();
	return 0;
}
catch (const std::exception& error) {
	std::fprintf(stderr, "\n");
	std::fprintf(stderr, "Error thrown: %s\n", error.what());
	return 1;
}



//int main() try {
//	// This manual scope is very important, it ensures the objects in models have their associated
//	// objects' destructors called BEFORE we destroy the vulkan device. Do not move the models vector
//	// out of this scope unless you also handle the object lifetimes from them.
//	{
//		std::vector<Engine::vk::Model> models;
//		initialiseGame();
//		initialiseModels(models);
//		runGameLoop(models);
//
//		for (Engine::vk::Model& model : models)
//			model.destroy();
//	}
//
//	// vkContext would get destroyed after main exits
//	// and Vulkan doesnt like objects being destroyed
//	// past main, so we manually call those object 
//	// destructors ourselves.
//	vkContext.allocator.reset();
//	vkContext.window.reset();
//
//	return 0;
//}
//catch (const std::exception& error) {
//	std::fprintf(stderr, "\n");
//	std::fprintf(stderr, "Error thrown: %s\n", error.what());
//	return 1;
//}

//int main()
//{
//	if (!InitializeYojimbo())
//	{
//		std::cerr << "Failed to initialize Yojimbo\n";
//		exit(1);
//	}
//	else
//	{
//		std::cout << "Yojimbo initialized\n";
//	}
//
//#ifdef DEBUG
//	yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
//#else
//	yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
//#endif
//
//	// Make adapter
//	GameAdapter* adapter = YOJIMBO_NEW(yojimbo::GetDefaultAllocator(), GameAdapter);
//
//	// Make config
//	yojimbo::ClientServerConfig config;
//
//#ifdef CLIENT
//	// Make and start client
//	GameClient client(&config, adapter, GetLocalIPAddress(3000));
//#else
//	// Make and start server
//	GameServer server(&config, adapter, GetLocalIPAddress(3000), 2);
//	server.Stop();
//#endif
//	return 0;
//}
