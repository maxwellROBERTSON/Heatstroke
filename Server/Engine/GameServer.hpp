#include "GameAdapter.hpp"
#include "EntityManager.hpp"

class GameServer
{
public:
	GameServer(yojimbo::Address, int);
	~GameServer() { delete server; delete adapter; };

	void Start();
	void Run();
	void Update(float);
	void ProcessMessages();
	void ProcessMessage(int, GameMessage*);
	void Stop();

	EntityManager* GetEntityManager();
	//void AddEntity(Entity entity);

private:
	yojimbo::ClientServerConfig config;
	yojimbo::Server* server;
	GameAdapter* adapter;
	int maxClients;
	EntityManager entityManager;
};
