#include "Archer.h"

Archer::Archer(float startX, float startY, Faction faction) 
    : Unit(startX, startY, faction) {
    
    // Chỉ số của Archer (Máu giấy, đam to, tầm xa)
    hp = maxHp = 60;
    damage = 20;
    attackRange = 200.0f;
    attackCooldown = 1.5f;
    speed = 35.0f;
    cost = 75;
    
    // Kích thước nhỏ gọn hơn Warrior
    width = 20.0f;
    height = 45.0f;
    // Đổi màu cho dễ nhận diện (Cung thủ Phe ta màu xanh dương nhạt, phe địch màu Cam)
    color = (faction == Faction::PLAYER) ? SKYBLUE : ORANGE;
}