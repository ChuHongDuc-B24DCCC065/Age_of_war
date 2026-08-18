#include "Archer.h"

Archer::Archer(float startX, float startY, Faction faction, Texture2D tex) 
    : Unit(startX, startY, faction) {
        unitType = UnitType::ARCHER;
    
    this->texture = tex; 

    // Chỉ số của Archer (Máu giấy, tay dài)
    hp = maxHp = 70;
    damage = 25;
    attackRange = 250.0f; // Tầm đánh xa
    attackCooldown = 1.0f;
    speed = 30.0f;
    cost = 75;
    
    width = (float)texture.width;
    height = (float)texture.height;
}

void Archer::Draw() {
    
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width * (faction == Faction::PLAYER ? 1.0f : -1.0f), (float)texture.height };
    Rectangle destRec   = { x, y, 90.0f, 90.0f };
    Vector2 origin      = { 45.0f, 90.0f }; // Neo vào chân lính
    DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);

    
    DrawHealthBar();
}