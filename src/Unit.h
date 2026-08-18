#pragma once
#include "raylib.h"
#include <vector>
#include <memory>

class Base;

enum class Faction { PLAYER, ENEMY };
enum class UnitState { MOVING, ATTACKING, DEAD };
enum class UnitType { WARRIOR, ARCHER, TANK };

class Unit {
protected:
    float x, y;
    int hp, maxHp;
    int damage;
    int cost;
    float speed;
    float attackCooldown;
    float currentCooldown;
    float attackRange;
    float width, height;
    Faction faction;
    UnitState state;
    Color color;

    bool justAttacked = false;
    float targetX = 0.0f;
    UnitType unitType = UnitType::WARRIOR;

public:
    Unit(float startX, float startY, Faction f, float statMultiplier = 1.0f);
    virtual ~Unit() = default;

    virtual void Update(float deltaTime, const std::vector<std::shared_ptr<Unit>>& units, Base& playerBase, Base& enemyBase);
    virtual void Draw() = 0;
    void DrawHealthBar() const;

    float GetWidth() const;
    float GetHeight() const;
    float GetX() const;
    bool IsDead() const;
    Faction GetFaction() const;
    void TakeDamage(int damageAmount);
    int GetCost() const;
    int GetExpReward() const { return (int)(cost * 1.2f); }

    bool IsAttackingFrame() const { return justAttacked; }
    void ResetAttackFrame() { justAttacked = false; }
    float GetTargetX() const { return targetX; }
    UnitType GetType() const { return unitType; }

    void ScaleStats(float multiplier) {
        hp = maxHp = (int)(maxHp * multiplier);
        damage     = (int)(damage * multiplier);
        cost       = (int)(cost * multiplier);
    }
}; 