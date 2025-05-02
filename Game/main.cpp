#include <yojimbo.h>

#include "../Engine/Network/Client/GameClient.hpp"
#include "../Engine/Network/Helpers/GameAdapter.hpp"
#include "../Engine/Network/Helpers/GameConfig.hpp"
#include "../Engine/Network/Helpers/GetLocalIPAddress.hpp"
#include "../Engine/Network/Server/GameServer.hpp"

#include "../Engine/Physics/PhysicsWorld.hpp"
#include "DemoGame.hpp"

int main() try
{
	std::unique_ptr<FPSTest> game = std::make_unique<FPSTest>();
	game->Run();
	return 0;
}
catch (const std::exception& error) {
	std::fprintf(stderr, "\n");
	std::fprintf(stderr, "Error thrown: %s\n", error.what());
	return 1;
}
