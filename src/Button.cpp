#include "Button.h"

Button::Button() 
    : rect(Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f }), text(""), baseColor(GRAY), isEnabled(true) {}

Button::Button(float x, float y, float width, float height, const char* label, Color color)
    : rect(Rectangle{ x, y, width, height }), text(label), baseColor(color), isEnabled(true) {}

void Button::SetText(const char* newText) {
    text = newText;
}

void Button::SetRect(Rectangle newRect) {
    rect = newRect;
}

bool Button::IsHovered() const {
    if (!isEnabled) return false;
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, rect);
}

bool Button::IsClicked() const {
    if (!isEnabled) return false;
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void Button::Draw(bool enabled) {
    isEnabled = enabled;
    Vector2 mousePos = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePos, rect) && isEnabled;

    // 1. Màu sắc theo trạng thái
    Color bodyColor = isEnabled ? (isHovered ? ColorAlpha(baseColor, 0.9f) : baseColor) : Color{ 30, 41, 59, 180 };
    Color borderColor = isEnabled ? (isHovered ? RAYWHITE : ColorAlpha(WHITE, 0.3f)) : Color{ 71, 85, 105, 100 };
    Color textColor = isEnabled ? RAYWHITE : Color{ 148, 163, 184, 150 };

    // 2. Vẽ nền nút và viền
    DrawRectangleRounded(rect, 0.2f, 4, bodyColor);
    DrawRectangleRoundedLines(rect, 0.2f, 4, borderColor);

    // 3. Canh giữa text
    const char* str = text.c_str();
    int fontSize = 12;
    int textWidth = MeasureText(str, fontSize);
    int textX = (int)(rect.x + (rect.width - textWidth) / 2.0f);
    int textY = (int)(rect.y + (rect.height - fontSize) / 2.0f);

    // 4. Đổ bóng chữ
    if (isEnabled) {
        DrawText(str, textX + 1, textY + 1, fontSize, ColorAlpha(BLACK, 0.6f));
    }
    DrawText(str, textX, textY, fontSize, textColor);
}