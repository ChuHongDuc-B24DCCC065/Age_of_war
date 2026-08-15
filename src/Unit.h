#pragma once
#include "raylib.h"
#include <vector>
#include <memory>
#include "Base.h" 

enum class Faction {
    PLAYER,
    ENEMY
};

enum class UnitState {
    MOVING,
    ATTACKING,
    DEAD
};
enum class UnitType{ WARRIOR, ARCHER, TANK};
class Unit {
protected:
    float x, y;
    float width, height;
    int hp, maxHp;
    int damage;
    float speed;
    float attackRange;
    float attackCooldown;
    float currentCooldown;
    int cost;
    float GetWidth() const;
    Faction faction;
    UnitState state;
    Color color;

public:
    Unit(float startX, float startY, Faction faction);
    virtual ~Unit() = default;

    // Chỉ có 1 hàm Update và 1 hàm Draw ở đây thôi nhé!
    virtual void Update(float deltaTime, const std::vector<std::shared_ptr<Unit>>& units, Base& playerBase, Base& enemyBase);
    virtual void Draw();

    bool IsDead() const;
    float GetX() const;
    Faction GetFaction() const;
    void TakeDamage(int damageAmount);
    int GetCost() const;
};