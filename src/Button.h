#pragma once
#include "raylib.h"
#include <string>

class Button {
private:
    Rectangle bounds; // Chứa x, y, width, height
    std::string text;
    Color baseColor;

public:
    Button(float x, float y, float width, float height, std::string text, Color color);
    
    bool IsClicked() const;
    void Draw(bool affordable) const; // Truyền vào true/false xem có đủ tiền mua không
};