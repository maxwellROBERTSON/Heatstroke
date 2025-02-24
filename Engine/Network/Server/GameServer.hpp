#pragma once

#include "../Helpers/GameAdapter.hpp"

class GameServer
{
public:
	GameServer(
		yojimbo::ClientServerConfig* config,
		GameAdapter* adapter,
		yojimbo::Address address,
		int maxClients
	);
	~GameServer() {}

	void Start();
	void Run();
	void Update(float);
	void ProcessMessages();
	void ProcessMessage(int, GameMessage*);
	void CleanUp();

	//EntityManager* GetEntityManager();
	//void AddEntity(Entity entity);

private:
	int maxClients;
	yojimbo::Server* server;

	yojimbo::ClientServerConfig* config;
	GameAdapter* adapter;
};
