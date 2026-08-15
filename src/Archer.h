#pragma once
#include "Unit.h"
#include "raylib.h"

class Archer : public Unit {
private:
    Texture2D texture;
public:
    Archer(float startX, float startY, Faction faction, Texture2D tex); 
    void Draw() override; 
};