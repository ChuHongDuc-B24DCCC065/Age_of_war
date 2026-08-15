#include "Warrior.h"

// 1. Nhận thêm tham số Texture2D tex
Warrior::Warrior(float startX, float startY, Faction faction, Texture2D tex) 
    : Unit(startX, startY, faction) { // Gọi constructor của lớp cha
    
    // Lưu ảnh vào biến của lính
    this->texture = tex; 

    // Ghi đè các chỉ số thiết kế cho Warrior
    hp = maxHp = 100;
    damage = 15;
    attackRange = 40.0f;
    attackCooldown = 1.0f;
    speed = 50.0f;
    cost = 50;
    
    // 2. Tự động lấy kích thước từ bức ảnh, không cần gán cứng số 25 và 50 nữa
    width = (float)texture.width;
    height = (float)texture.height;
}

// 3. Viết thêm hàm Draw để vẽ ảnh ra màn hình
void Warrior::Draw() {
    // ==========================================
    // 1. VẼ HÌNH ẢNH LÍNH
    // ==========================================
    float flip = (faction == Faction::PLAYER) ? 1.0f : -1.0f;
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width * flip, (float)texture.height };
    Vector2 position = { x, y };
    DrawTextureRec(texture, sourceRec, position, WHITE);

    // ==========================================
    // 2. VẼ THANH MÁU TRÊN ĐẦU NHÂN VẬT
    // ==========================================
    float healthPercentage = (float)hp / maxHp;
    int barWidth = 40;  
    int barHeight = 6;  
    
    int barX = (int)x + (texture.width / 2) - (barWidth / 2);
    int barY = (int)y - 10;
    
    DrawRectangle(barX - 1, barY - 1, barWidth + 2, barHeight + 2, BLACK);
    DrawRectangle(barX, barY, barWidth, barHeight, RED);
    DrawRectangle(barX, barY, barWidth * healthPercentage, barHeight, GREEN);
}