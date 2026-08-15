#pragma once
#include "raylib.h"

class Base {
private:
    float x, y;
    float hp, maxHp;
    Texture2D texture; 
    bool isPlayer;     

public:
    Base(); 
    Base(float startX, float startY, Texture2D tex, bool isPlayerSide);
    
    void Update();
    void Draw() const;
    float GetHP() const { return hp; }

    float GetX() const { return x; }
    
    float GetWidth() const { return (float)texture.width; } 
    
    void TakeDamage(float amount); 
};