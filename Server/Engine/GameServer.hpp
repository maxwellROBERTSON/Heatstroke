#include "../../third_party/yojimbo/include/yojimbo.h"
#include "GameAdapter.hpp"

class GameServer
{
public:
	GameServer(yojimbo::Address, int);
	void Start();
	void Run();
	void Update(float);
	void ProcessMessages();
	void ProcessMessage(int, GameMessage*);
	void Stop();

private:
	yojimbo::ClientServerConfig config;
	yojimbo::Server* server;
	GameAdapter* adapter;
	int maxClients;
};
