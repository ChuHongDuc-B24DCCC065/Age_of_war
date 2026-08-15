#pragma once
#include "Unit.h"
#include "raylib.h"

class Tank : public Unit {
private:
    Texture2D texture;
public:
    Tank(float startX, float startY, Faction faction, Texture2D tex); 
    void Draw() override; 
};