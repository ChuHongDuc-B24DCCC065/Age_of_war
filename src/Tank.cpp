#include "Tank.h"

Tank::Tank(float startX, float startY, Faction faction) 
    : Unit(startX, startY, faction) {
    
    // Chỉ số của Tank (Rất trâu, đi chậm)
    hp = maxHp = 250;
    damage = 30;
    attackRange = 50.0f;
    attackCooldown = 2.0f;
    speed = 20.0f;
    cost = 150;
    
    // Kích thước to béo
    width = 40.0f;
    height = 65.0f;
    // Màu đậm hơn (Phe ta màu xanh đậm, phe địch màu đỏ sẫm)
    color = (faction == Faction::PLAYER) ? DARKBLUE : MAROON;
}