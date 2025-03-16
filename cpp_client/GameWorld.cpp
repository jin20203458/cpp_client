#include "GameWorld.h"

GameWorld::GameWorld() 
{
    
}


void GameWorld::syncWorldData()
{
	std::lock_guard<std::mutex> lock(worldMutex);
	ByteStream& stream = localPlayer->getSession().getReceiveStream();
	int playerCount = stream.ReadInt();
	std::print("Player count : {}\n", playerCount);
	std::unordered_map<std::string, bool> updatedPlayers;
	std::string myPlayerName = localPlayer->getName();

    for (int i = 0; i < playerCount; ++i)
    {
        std::string playerName = stream.ReadString();
        float posX = stream.ReadFloat();
        float posY = stream.ReadFloat();

		if (playerName == myPlayerName || playerName=="UninitPlayer") continue; // �ڽ�,�ʱ�ȭ ���� ���� �÷��̾� ����
        std::print("Player {} : move x{},y{}\n", playerName, posX, posY);

        auto it = remotePlayers.find(playerName);
        if (it != remotePlayers.end()) // ���� ��� �ֽ�ȭ
        {
            it->second->updatePosition(posX, posY);
        }
        else // �ű� �÷��̾� �߰�
        {
            RemotePlayer* newPlayer = new RemotePlayer(playerName);
            newPlayer->updatePosition(posX, posY);
            remotePlayers[playerName] = newPlayer;
        }
        updatedPlayers[playerName] = true;
    }

    // ���� �÷��̾� �߿��� ������Ʈ���� ���� �÷��̾� ����
    for (auto it = remotePlayers.begin(); it != remotePlayers.end(); ) 
    {
        if (updatedPlayers.find(it->first) == updatedPlayers.end()) 
        {
            delete it->second;
            it = remotePlayers.erase(it); // erase()�� ������ ������ ���� ���� ���ͷ����͸� ��ȯ
        }
        else
        {
            ++it;
        }
    }
}

void GameWorld::cleanup()
{
	if (localPlayer) delete localPlayer;
	for (auto& pair : remotePlayers) delete pair.second;
	remotePlayers.clear();
}

GameWorld::~GameWorld()
{
    cleanup();
}
