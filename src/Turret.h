#pragma once
#include "raylib.h"
#include <vector>
#include <memory>
#include "Unit.h"

enum class TurretTier { SLINGSHOT = 1, BALLISTA = 2, GATLING_GUN = 3 };

class Turret {
private:
    float x, y;
    Faction faction;
    TurretTier tier;
    float attackRange;
    int damage;
    float attackCooldown;
    float currentCooldown;
    bool justShot;
    Vector2 targetPos;

public:
    Turret();
    Turret(float posX, float posY, Faction f, TurretTier t = TurretTier::SLINGSHOT);

    void Update(float deltaTime, const std::vector<std::shared_ptr<Unit>>& units);
    void Draw() const;

    bool HasShot() const { return justShot; }
    void ResetShot() { justShot = false; }
    Vector2 GetPosition() const { return { x, y }; }
    Vector2 GetTargetPos() const { return targetPos; }
    TurretTier GetTier() const { return tier; }
};