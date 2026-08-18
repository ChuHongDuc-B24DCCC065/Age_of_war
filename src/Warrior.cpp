#include "Warrior.h"

// 1. Nhận thêm tham số Texture2D tex
Warrior::Warrior(float startX, float startY, Faction faction, Texture2D tex) 
    : Unit(startX, startY, faction) { // Gọi constructor của lớp cha
    
    unitType = UnitType::WARRIOR;
    this->texture = tex; 

    // Ghi đè các chỉ số thiết kế cho Warrior
    hp = maxHp = 100;
    damage = 15;
    attackRange = 40.0f;
    attackCooldown = 1.2f;
    speed = 30.0f;
    cost = 50;
    
    // 2. Tự động lấy kích thước từ bức ảnh, không cần gán cứng số 25 và 50 nữa
    width = (float)texture.width;
    height = (float)texture.height;
    
}

// 3. Viết thêm hàm Draw để vẽ ảnh ra màn hình
void Warrior::Draw() {
    
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width * (faction == Faction::PLAYER ? 1.0f : -1.0f), (float)texture.height };
    Rectangle destRec   = { x, y, 90.0f, 90.0f };
    Vector2 origin      = { 45.0f, 90.0f };
    DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);

    DrawHealthBar();
}