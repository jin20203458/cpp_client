#pragma once
#include <unordered_map>
#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include <print>
#include "Player.h"
#include "PacketType.h"
#include "RemotePlayer.h"

class GameWorld {
public:
    GameWorld();
    ~GameWorld();

	void setLocalPlayer(Player* player) { localPlayer = player; }
	Player* getLocalPlayer() { return localPlayer; }

    void syncWorldData();
private:
    void cleanup();

    Player* localPlayer;
    std::unordered_map<std::string, RemotePlayer*> remotePlayers;
    std::mutex worldMutex;

};
