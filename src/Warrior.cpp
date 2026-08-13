#include "Warrior.h"

Warrior::Warrior(float startX, float startY, Faction faction) 
    : Unit(startX, startY, faction) { // Gọi constructor của lớp cha
    
    // Ghi đè các chỉ số thiết kế cho Warrior
    hp = maxHp = 100;
    damage = 15;
    attackRange = 40.0f;
    attackCooldown = 1.0f;
    speed = 50.0f;
    cost = 50;
    
    // Đồ họa tạm thời (kích thước hình chữ nhật)
    width = 25.0f;
    height = 50.0f;
}