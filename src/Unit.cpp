#include "Unit.h"

Unit::Unit(float startX, float startY, Faction f) {
    x = startX;
    y = startY; 
    faction = f;
    
    state = UnitState::MOVING;
    currentCooldown = 0.0f;
    
    width = 20.0f;
    height = 40.0f;
    hp = maxHp = 100;
    speed = 50.0f;
    color = (faction == Faction::PLAYER) ? BLUE : RED;
}

void Unit::Update(float deltaTime, const std::vector<std::shared_ptr<Unit>>& units, Base& playerBase, Base& enemyBase) {
    if (state == UnitState::DEAD) return;

    if (currentCooldown > 0) currentCooldown -= deltaTime;

    // 1. Quét tìm mục tiêu kẻ địch gần nhất (Bỏ qua hoàn toàn đồng minh để cho phép đi xuyên qua nhau)
    float minEnemyDist = 999999.0f;
    std::shared_ptr<Unit> targetEnemy = nullptr;

    for (const auto& other : units) {
        if (other.get() == this || other->IsDead() || other->GetFaction() == this->faction) continue;

        float dist = 0.0f;
        if (faction == Faction::PLAYER) {
            dist = other->GetX() - (this->x + this->width);
        } else {
            dist = this->x - (other->GetX() + other->GetWidth());
        }

        // Kẻ địch nằm phía trước hoặc đang chạm nhẹ (-10px bù trừ hitbox)
        if (dist >= -10.0f && dist < minEnemyDist) {
            minEnemyDist = dist;
            targetEnemy = other;
        }
    }

    // 2. Tính khoảng cách tới nhà chính đối phương
    float distToBase = 999999.0f;
    if (faction == Faction::PLAYER) {
        distToBase = enemyBase.GetX() - (this->x + this->width);
    } else {
        distToBase = this->x - (playerBase.GetX() + playerBase.GetWidth());
    }

    // 3. Quản lý trạng thái: Tấn công nếu có địch hoặc nhà chính trong tầm đánh
    bool targetInRange = (minEnemyDist <= attackRange) || (distToBase <= attackRange);

    if (targetInRange) {
        state = UnitState::ATTACKING;
    } else {
        state = UnitState::MOVING;
    }

    // 4. Thực thi hành động
    if (state == UnitState::MOVING) {
        if (faction == Faction::PLAYER) x += speed * deltaTime;
        else x -= speed * deltaTime;
    } 
    else if (state == UnitState::ATTACKING) {
        if (currentCooldown <= 0) {
            // Ưu tiên đánh lính địch trước, nếu không có lính địch thì đánh nhà chính
            if (minEnemyDist <= attackRange && targetEnemy != nullptr) {
                targetEnemy->TakeDamage(damage);
            } else if (distToBase <= attackRange) {
                if (faction == Faction::PLAYER) enemyBase.TakeDamage(damage);
                else playerBase.TakeDamage(damage);
            }

            // Đặt lại thời gian hồi chiêu
            currentCooldown = attackCooldown;
        }
    }
}

void Unit::Draw() {
    if (state == UnitState::DEAD) return;
    
    DrawRectangle(x, y - height, width, height, color);

    // Thanh máu
    float hpPercentage = (float)hp / maxHp;
    DrawRectangle(x, y - height - 10, width, 5, RED);
    DrawRectangle(x, y - height - 10, hpPercentage * width, 5, GREEN);

    // Hiệu ứng tấn công (VFX)
    if (state == UnitState::ATTACKING) {
        float animDuration = 0.15f; 
        if (currentCooldown > attackCooldown - animDuration) {
            float weaponX = (faction == Faction::PLAYER) ? (x + width) : (x - 5.0f);
            float weaponY = y - height / 2.0f;

            if (attackRange > 50.0f) {
                // Tầm xa (Archer)
                float progress = (attackCooldown - currentCooldown) / animDuration; 
                float distance = attackRange * progress; 
                float bulletX = (faction == Faction::PLAYER) ? (weaponX + distance) : (weaponX - distance);
                DrawCircle((int)bulletX, (int)weaponY, 4.0f, YELLOW);
            } else {
                // Cận chiến (Warrior / Tank)
                float slashLength = 20.0f; 
                float endX = (faction == Faction::PLAYER) ? (weaponX + slashLength) : (weaponX - slashLength);
                DrawLineEx({weaponX, weaponY}, {endX, weaponY}, 4.0f, YELLOW);
            }
        }
    }
}

void Unit::TakeDamage(int damageAmount) {
    hp -= damageAmount;
    if (hp <= 0) {
        state = UnitState::DEAD;
    }
}

bool Unit::IsDead() const {
    return state == UnitState::DEAD;
}

float Unit::GetX() const { return x; }
float Unit::GetWidth() const { return width; }
Faction Unit::GetFaction() const { return faction; }
int Unit::GetCost() const { return cost; }