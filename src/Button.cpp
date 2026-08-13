#include "Button.h"

Button::Button(float x, float y, float width, float height, std::string text, Color color) {
    bounds = {x, y, width, height};
    this->text = text;
    baseColor = color;
}

bool Button::IsClicked() const {
    Vector2 mousePoint = GetMousePosition(); // Lấy tọa độ chuột
    
    // Thuật toán kiểm tra: Chuột có nằm trong Hình chữ nhật không?
    if (CheckCollisionPointRec(mousePoint, bounds)) {
        // Có đang bấm chuột trái không?
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return true;
        }
    }
    return false;
}

void Button::Draw(bool affordable) const {
    Vector2 mousePoint = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePoint, bounds);

    Color bgColor = baseColor;
    
    if (!affordable) {
        bgColor = GRAY; // Không đủ tiền -> Màu xám
    } 
    else if (isHovered) {
        bgColor = LIGHTGRAY; // Đủ tiền + Chuột lướt qua -> Sáng lên
    }

    // Vẽ hình chữ nhật nền
    DrawRectangleRec(bounds, bgColor);
    // Vẽ viền đen cho đẹp
    DrawRectangleLinesEx(bounds, 2.0f, BLACK);
    
    // Đoạn code căn giữa chữ vào giữa nút bấm
    int textWidth = MeasureText(text.c_str(), 20); // Đo xem chuỗi chữ dài bao nhiêu pixel
    DrawText(text.c_str(), bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - 10, 20, BLACK);
}