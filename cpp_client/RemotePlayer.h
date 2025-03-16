#pragma once
#include <string>


class RemotePlayer 
{
public:
    RemotePlayer(const std::string& name) : name(name), posX(0), posY(0) {}

    void updatePosition(float x, float y) { posX = x; posY = y; }
    void setName(const std::string& name) { this->name = name; }
    
    float getPosX() const { return posX; }
    float getPosY() const { return posY; }
    const std::string& getName() const { return name; }


private:
    std::string name;
    float posX, posY;
};
