#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <string>
#include <print>
#include "ByteStream.h"
#include "PacketType.h"
#include "Player.h"
#include "GameWorld.h"
using namespace std;

SOCKET connectToServer(const std::string& ip, int port);
void SendData(GameWorld* gameWorld);
void ReceiveData(GameWorld* gameWorld);


void main()
{
    SOCKET sock = connectToServer("127.0.0.1", 5000);
    if (sock == INVALID_SOCKET) return;
    

    Player* playerPtr = new Player(sock, "P1");
    std::print("Decide your name (only in English)");
    std::string name;
    cin >> name;
    playerPtr->setName(name);
	GameWorld gameWorld;
	gameWorld.setLocalPlayer(playerPtr);

    // 송수신을 처리하는 각각의 스레드를 생성
    thread sendThread(SendData,&gameWorld);  // 송신 스레드
    thread recvThread(ReceiveData,&gameWorld);  // 수신 스레드

    // 스레드가 종료될 때까지 기다림
    sendThread.join();
    recvThread.join();

    // 연결 종료 후 클린업
    delete playerPtr;
    closesocket(sock);
    WSACleanup();
}

void ReceiveData( GameWorld*gameWorld )
{
    Player* localPlayer = gameWorld->getLocalPlayer();
    ByteStream& stream = localPlayer->getSession().getReceiveStream();

    while (true)
    {
        if (!localPlayer->receivePlayerData())
        {
            std::cerr << "Error receiving data from server";
            break;
        }
        else
        {
            //std::print("Received data Size {}\n", stream.GetSize());
            PacketType dataType = stream.ReadPacketType();

            if (dataType == PacketType::WorldUpdate) gameWorld->syncWorldData();
            else  std::cerr << "Invalid packet type received";
        }
    }
}

void SendData( GameWorld* gameWorld )
{
    // start
	float x = 0, y = 0;
    Player * player = gameWorld->getLocalPlayer();
	player->Init();

	// update
    while (true)
    {	
		player->sendPlayerData();
        Sleep(1000);  // 데이터 송신 간격을 1초로 설정
		x++,y++;
		player->updatePosition(x, y);
    }
}

SOCKET connectToServer(const std::string& ip, int port) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
        cerr << "WSAStartup failed!\n";
        return INVALID_SOCKET;
    }

    // 1. 소켓 생성
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        cerr << "Socket creation failed!\n";
        WSACleanup();
        return INVALID_SOCKET;
    }

    // 2. 서버 주소 설정
    SOCKADDR_IN saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    InetPtonA(AF_INET, ip.c_str(), &(saddr.sin_addr));

    // 3. 서버 연결 요청
    if (connect(sock, (SOCKADDR*)&saddr, sizeof(saddr)) == SOCKET_ERROR) {
        cerr << "Connect error\n";
        closesocket(sock);
        WSACleanup();
        return INVALID_SOCKET;
    }

    print("Connected to server: {}:{}\n", ip, port);
    return sock;
}
