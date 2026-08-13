#pragma once
#include "raylib.h"
class Base {
private:
    float x, y;
    float width, height;
    int hp;
    int maxHp;
    Color color; // Bây giờ C++ mới biết Color là gì

public:
    Base(float startX, float startY, Color teamColor);
    void Update();
    void Draw();
    int GetHP() const;
    float GetX() const;
    float GetWidth() const;
    void TakeDamage(int damageAmount);
}; // Nhớ CÓ dấu chấm phẩy ở đây