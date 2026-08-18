#pragma once
#include "raylib.h"

class Base {
private:
    float x, y;
    int hp;
    int maxHp;
    Texture2D texture;
    bool isPlayer;

public:
    Base();
    Base(float startX, float startY, Texture2D tex, bool isPlayerSide);

    void Draw() const;
    void Update();
    void TakeDamage(float amount); 

    int GetHP() const { return hp; }
    int GetMaxHP() const { return maxHp; }
    float GetX() const { return x; }
};