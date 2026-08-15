#include "Tank.h"

Tank::Tank(float startX, float startY, Faction faction, Texture2D tex) 
    : Unit(startX, startY, faction) {
    
    this->texture = tex; 

    // Chỉ số của Tank (Trâu bò, tay ngắn, đi chậm)
    hp = maxHp = 300;
    damage = 40;
    attackRange = 200.0f;
    attackCooldown = 2.0f;
    speed = 25.0f;
    cost = 150;
    
    width = (float)texture.width;
    height = (float)texture.height;
}

void Tank::Draw() {
    // 1. Vẽ ảnh lật chiều
    float flip = (faction == Faction::PLAYER) ? 1.0f : -1.0f;
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width * flip, (float)texture.height };
    Vector2 position = { x, y };
    DrawTextureRec(texture, sourceRec, position, WHITE);

    // 2. Vẽ thanh máu
    float healthPercentage = (float)hp / maxHp;
    int barWidth = 40;  
    int barHeight = 6;  
    int barX = (int)x + (texture.width / 2) - (barWidth / 2);
    int barY = (int)y - 10;
    
    DrawRectangle(barX - 1, barY - 1, barWidth + 2, barHeight + 2, BLACK);
    DrawRectangle(barX, barY, barWidth, barHeight, RED);
    DrawRectangle(barX, barY, barWidth * healthPercentage, barHeight, GREEN);
}