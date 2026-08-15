#include "Base.h"

// 1. Định nghĩa hàm khởi tạo rỗng
Base::Base() {} 

// 2. Hàm khởi tạo chính
Base::Base(float startX, float startY, Texture2D tex, bool isPlayerSide) {
    x = startX;
    y = startY;
    texture = tex;
    isPlayer = isPlayerSide;
    
    hp = maxHp = 1000; // Máu nhà chính trâu hơn lính
}

// 3. Hàm vẽ nhà chính
void Base::Draw() const {
    // Vẽ ảnh nhà (Lật ngược nếu là nhà địch)
    float flip = isPlayer ? 1.0f : -1.0f;
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width * flip, (float)texture.height };
    
    // Nếu ảnh nhà bị lơ lửng trên không, bạn có thể trừ đi texture.height ở tọa độ y
    Vector2 position = { x, y - (texture.height / 2) }; // Chỉnh số này cho vừa chạm đất
    
    DrawTextureRec(texture, sourceRec, position, WHITE);

    // Vẽ thanh máu nhà chính (To và dài hơn thanh máu lính)
    float healthPercentage = hp / maxHp;
    int barWidth = 100;  
    int barHeight = 10;  
    int barX = (int)x + (texture.width / 2) - (barWidth / 2);
    int barY = (int)position.y - 20; // Nổi lên trên nóc nhà
    
    DrawRectangle(barX - 2, barY - 2, barWidth + 4, barHeight + 4, BLACK);
    DrawRectangle(barX, barY, barWidth, barHeight, RED);
    DrawRectangle(barX, barY, barWidth * healthPercentage, barHeight, GREEN);
}
void Base::Update() {
    // Tạm thời để trống. 
    // Sau này bạn có thể thêm logic như: Nếu máu dưới 50% thì bốc khói, cháy nổ...
}
void Base::TakeDamage(float amount) {
    hp -= amount;
    if (hp < 0) hp = 0; // Đảm bảo máu không bị âm
}