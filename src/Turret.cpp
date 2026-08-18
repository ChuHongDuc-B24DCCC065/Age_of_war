#include "Turret.h"
#include <cmath>

Turret::Turret() 
    : x(0), y(0), faction(Faction::PLAYER), tier(TurretTier::SLINGSHOT), 
      attackRange(340.0f), damage(25), attackCooldown(1.5f), 
      currentCooldown(0.0f), justShot(false), targetPos({0, 0}) {}

Turret::Turret(float posX, float posY, Faction f, TurretTier t) 
    : x(posX), y(posY), faction(f), tier(t), currentCooldown(0.0f), 
      justShot(false), targetPos({0, 0}) 
{
    if (tier == TurretTier::SLINGSHOT) {
        attackRange = 340.0f;
        damage = 25;
        attackCooldown = 1.6f;
    } else if (tier == TurretTier::BALLISTA) {
        attackRange = 400.0f;
        damage = 45;
        attackCooldown = 1.3f;
    } else {
        attackRange = 480.0f;
        damage = 35;
        attackCooldown = 0.5f;
    }
}

void Turret::Update(float deltaTime, const std::vector<std::shared_ptr<Unit>>& units) {
    if (currentCooldown > 0.0f) {
        currentCooldown -= deltaTime;
    }

    std::shared_ptr<Unit> targetUnit = nullptr;
    float minDistance = 99999.0f;

    for (const auto& unit : units) {
        if (unit->GetFaction() != faction && !unit->IsDead()) {
            float dist = (faction == Faction::PLAYER) ? (unit->GetX() - x) : (x - unit->GetX());
            if (dist > 0 && dist <= attackRange && dist < minDistance) {
                minDistance = dist;
                targetUnit = unit;
            }
        }
    }

    if (targetUnit != nullptr && currentCooldown <= 0.0f) {
        targetUnit->TakeDamage(damage);
        currentCooldown = attackCooldown;
        justShot = true;
        targetPos = { targetUnit->GetX(), 650.0f };
    }
}

void Turret::Draw() const {
    // Ụ súng
    DrawRectangle((int)(x - 12), (int)(y - 4), 24, 8, DARKGRAY);
    DrawCircle((int)x, (int)y - 4, 10.0f, (faction == Faction::PLAYER ? BLUE : RED));

    // Nòng súng hướng ra chiến trường
    float barrelLength = (tier == TurretTier::GATLING_GUN) ? 20.0f : 15.0f;
    float barrelEndX = (faction == Faction::PLAYER) ? (x + barrelLength) : (x - barrelLength);
    DrawLineEx({ x, y - 4 }, { barrelEndX, y - 6 }, 3.5f, BLACK);
}