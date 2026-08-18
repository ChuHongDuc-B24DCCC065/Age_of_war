#include "Tank.h"

Tank::Tank(float startX, float startY, Faction faction, Texture2D tex) 
    : Unit(startX, startY, faction) {
        unitType = UnitType::TANK;
    
    this->texture = tex; 

    // Chỉ số của Tank (Trâu bò, tay ngắn, đi chậm)
    hp = maxHp = 300;
    damage = 40;
    attackRange = 200.0f;
    attackCooldown = 1.5f;
    speed = 25.0f;
    cost = 150;
    
    width = (float)texture.width;
    height = (float)texture.height;
}

void Tank::Draw() {
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width * (faction == Faction::PLAYER ? 1.0f : -1.0f), (float)texture.height };
    Rectangle destRec   = { x, y, 90.0f, 90.0f };
    Vector2 origin      = { 45.0f, 90.0f }; 
    DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);

    DrawHealthBar();
}