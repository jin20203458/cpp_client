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

		if (playerName == myPlayerName || playerName=="UninitPlayer") continue; // 자신,초기화 되지 않은 플레이어 제외
        std::print("Player {} : move x{},y{}\n", playerName, posX, posY);

        auto it = remotePlayers.find(playerName);
        if (it != remotePlayers.end()) // 기존 목록 최신화
        {
            it->second->updatePosition(posX, posY);
        }
        else // 신규 플레이어 추가
        {
            RemotePlayer* newPlayer = new RemotePlayer(playerName);
            newPlayer->updatePosition(posX, posY);
            remotePlayers[playerName] = newPlayer;
        }
        updatedPlayers[playerName] = true;
    }

    // 기존 플레이어 중에서 업데이트되지 않은 플레이어 삭제
    for (auto it = remotePlayers.begin(); it != remotePlayers.end(); ) 
    {
        if (updatedPlayers.find(it->first) == updatedPlayers.end()) 
        {
            delete it->second;
            it = remotePlayers.erase(it); // erase()는 삭제된 원소의 다음 원소 이터레이터를 반환
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
