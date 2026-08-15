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
    // 1. Lấy vị trí chuột hiện tại
    Vector2 mousePoint = GetMousePosition();
    
    // 2. Kiểm tra xem chuột có nằm trong khu vực của Nút không
    bool isHovered = CheckCollisionPointRec(mousePoint, bounds);

    Color bgColor;
    Color textColor;

    // 3. Xử lý màu sắc dựa trên việc có đủ tiền (affordable) hay không
    if (!affordable) {
        // Trạng thái KHÔNG đủ tiền: Nền tối, chữ đỏ cảnh báo, không có hiệu ứng hover
        bgColor = DARKGRAY;
        textColor = RED;
    } else {
        // Trạng thái ĐỦ tiền: Nếu di chuột vào thì sáng lên (LIGHTGRAY), không thì dùng màu gốc (baseColor)
        bgColor = isHovered ? LIGHTGRAY : baseColor;
        textColor = isHovered ? BLACK : WHITE;
    }

    // 4. Vẽ nút bo góc
    DrawRectangleRounded(bounds, 0.2f, 10, bgColor);
    
    // 5. Vẽ viền cho nút (Đã thêm số 2 vào trước BLACK, đây là độ dày của viền)
    DrawRectangleRoundedLines(bounds, 0.2f, 10, BLACK);

    // 6. Tính toán kích thước chữ để Căn giữa
    int fontSize = 20;
    int textWidth = MeasureText(text.c_str(), fontSize);
    
    int textX = bounds.x + (bounds.width / 2) - (textWidth / 2);
    int textY = bounds.y + (bounds.height / 2) - (fontSize / 2);

    // 7. In chữ lên nút
    DrawText(text.c_str(), textX, textY, fontSize, textColor);
}