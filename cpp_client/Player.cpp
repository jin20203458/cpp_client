#include "Player.h"

void Player::Init() 
{
    ByteStream& stream = session.getSendStream();
    stream.Clear();

    stream.WritePacketType(PacketType::PlayerInit);
    stream.WriteString(name);
    stream.WriteFloat(posX);
    stream.WriteFloat(posY);
    stream.HeaderInit();
	session.sendData();
}

bool Player::receivePlayerData()
{
    return session.receiveData();
}

void Player::sendPlayerData() 
{
    ByteStream& stream = session.getSendStream();
    stream.Clear();

    stream.WritePacketType(PacketType::PlayerUpdate);
    stream.WriteFloat(posX);
    stream.WriteFloat(posY);
    stream.HeaderInit();
    session.sendData();
}
