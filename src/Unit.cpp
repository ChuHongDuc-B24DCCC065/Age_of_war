#include "Unit.h"
#include "Base.h"
#include <cmath>
#include <algorithm>
Unit::Unit(float startX, float startY, Faction f, float statMultiplier) {
    x = startX;
    y = startY;
    faction = f;
    state = UnitState::MOVING;
    currentCooldown = 0.0f;

    width = 20.0f;
    height = 40.0f;

    // Tỉ lệ thuận chỉ số theo thời đại
    hp = maxHp = (int)(100 * statMultiplier);
    damage = (int)(15 * statMultiplier);
    cost = (int)(50 * statMultiplier);
    speed = 50.0f;
    color = (faction == Faction::PLAYER) ? BLUE : RED;

    // Khởi tạo cờ đánh mặc định:
    justAttacked = false;
    targetX = 0.0f;
}

void Unit::Update(float deltaTime, const std::vector<std::shared_ptr<Unit>>& units, Base& playerBase, Base& enemyBase) {
    if (state == UnitState::DEAD) return;

    if (currentCooldown > 0.0f) {
        currentCooldown -= deltaTime;
    }

    Base& targetBase = (faction == Faction::PLAYER) ? enemyBase : playerBase;

    // 1. Xác định tầm đánh chuẩn theo loại lính
    float attackRange = 50.0f; // Cận chiến (Warrior)
    if (unitType == UnitType::ARCHER) {
        attackRange = 280.0f; // Tầm bắn xa của Cung thủ
    } else if (unitType == UnitType::TANK) {
        attackRange = 160.0f; // Tầm bắn tầm trung của Tank
    }

    // 2. Tìm lính địch gần nhất
    std::shared_ptr<Unit> nearestEnemy = nullptr;
    float minEnemyDist = 99999.0f;

    // 3. Tìm lính đồng minh ở ngay phía trước để tránh đi đè/xuyên qua
    float minAllyDist = 99999.0f;

    for (const auto& other : units) {
        if (other.get() == this || other->IsDead()) continue;

        float dist = (faction == Faction::PLAYER) ? (other->GetX() - x) : (x - other->GetX());

        if (dist > 0) { // Chỉ xét vật thể phía trước mặt
            if (other->GetFaction() != faction) {
                if (dist < minEnemyDist) {
                    minEnemyDist = dist;
                    nearestEnemy = other;
                }
            } else {
                if (dist < minAllyDist) {
                    minAllyDist = dist;
                }
            }
        }
    }

    // 4. Khoảng cách tới nhà chính địch
    float distToBase = (faction == Faction::PLAYER) ? (targetBase.GetX() - x) : (x - targetBase.GetX());

    // 5. Kiểm tra mục tiêu trong tầm đánh
    bool enemyInRange = (nearestEnemy != nullptr && minEnemyDist <= attackRange);
    bool baseInRange  = (distToBase <= attackRange);

    if (enemyInRange) {
        state = UnitState::ATTACKING;
        if (currentCooldown <= 0.0f) {
            nearestEnemy->TakeDamage(damage);
            currentCooldown = (unitType == UnitType::ARCHER) ? 1.0f : 1.2f;

            justAttacked = true;
            targetX = nearestEnemy->GetX();
        }
    } 
    else if (baseInRange) {
        state = UnitState::ATTACKING;
        if (currentCooldown <= 0.0f) {
            targetBase.TakeDamage(damage);
            currentCooldown = 1.2f;

            justAttacked = true;
            targetX = targetBase.GetX();
        }
    } 
    else {
        // Chỉ di chuyển khi phía trước không bị vướng đồng minh (cách nhau tối thiểu 25px)
        if (minAllyDist > 25.0f) {
            state = UnitState::MOVING;
            if (faction == Faction::PLAYER) {
                x += speed * deltaTime;
            } else {
                x -= speed * deltaTime;
            }
        } else {
            // Đứng chờ đồng minh phía trước di chuyển
            state = UnitState::ATTACKING; 
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

    float barWidth = 40.0f;
    float barHeight = 6.0f;
    float barX = x - barWidth / 2.0f;
    float barY = y - 55.0f; // Đặt phía trên đầu lính

    float hpPercent = (float)hp / (float)maxHp;
    if (hpPercent < 0.0f) hpPercent = 0.0f;

    // Vẽ khung thanh máu
    DrawRectangle((int)barX, (int)barY, (int)barWidth, (int)barHeight, DARKGRAY);
    DrawRectangle((int)barX, (int)barY, (int)(barWidth * hpPercent), (int)barHeight, (faction == Faction::PLAYER ? GREEN : RED));
    DrawRectangleLines((int)barX, (int)barY, (int)barWidth, (int)barHeight, BLACK);

    // 3. HIỂN THỊ SỐ MÁU CỦA LÍNH
    const char* hpText = TextFormat("%d/%d", hp, maxHp);
    int fontSize = 10;
    int textWidth = MeasureText(hpText, fontSize);
    int textX = (int)(x - textWidth / 2.0f);
    int textY = (int)(barY - 12.0f);

    // Nền đen mờ phía sau số máu để không bị chìm vào nền trời
    DrawRectangle(textX - 2, textY - 1, textWidth + 4, fontSize + 2, Fade(BLACK, 0.65f));
    DrawText(hpText, textX, textY, fontSize, RAYWHITE);
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

void Unit::DrawHealthBar() const {
    if (state == UnitState::DEAD) return;

    // Chiều dài thanh máu lính
    float barWidth = 50.0f;
    float barHeight = 6.0f;
    float barX = x - (barWidth / 2.0f);
    float barY = y - 95.0f; // Vị trí ngay phía trên đầu lính

    float hpPercent = (maxHp > 0) ? ((float)hp / (float)maxHp) : 0.0f;
    if (hpPercent < 0.0f) hpPercent = 0.0f;

    // 1. Vẽ thanh máu (Nền xám + Máu xanh/đỏ)
    DrawRectangle((int)barX, (int)barY, (int)barWidth, (int)barHeight, DARKGRAY);
    DrawRectangle((int)barX, (int)barY, (int)(barWidth * hpPercent), (int)barHeight, (faction == Faction::PLAYER ? GREEN : RED));
    DrawRectangleLines((int)barX, (int)barY, (int)barWidth, (int)barHeight, BLACK);

    // 2. Vẽ số máu (Nằm ngay sát trên thanh máu 12px)
    const char* hpText = TextFormat("%d/%d", hp, maxHp);
    int fontSize = 10;
    int textWidth = MeasureText(hpText, fontSize);
    int textX = (int)(x - (textWidth / 2.0f));
    int textY = (int)(barY - 12.0f);

    DrawRectangle(textX - 2, textY - 1, textWidth + 4, fontSize + 2, Fade(BLACK, 0.75f));
    DrawText(hpText, textX, textY, fontSize, RAYWHITE);
}