#pragma once
#include "raylib.h"
#include <string>

class Button {
private:
    Rectangle rect;
    std::string text;
    Color baseColor;
    bool isEnabled;

public:
    Button();
    Button(float x, float y, float width, float height, const char* label, Color color);

    void SetText(const char* newText);
    void SetRect(Rectangle newRect);
    bool IsClicked() const;
    void Draw(bool enabled);
};