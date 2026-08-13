#include "Unit.h"

Unit::Unit(float startX, float startY, Faction f) {
    x = startX;
    y = startY; // y này ta quy ước là tọa độ mặt đất
    faction = f;
    
    // Trạng thái mặc định khi sinh ra
    state = UnitState::MOVING;
    currentCooldown = 0.0f;
    
    // Các chỉ số (stats) sẽ được ghi đè ở class con (Warrior, Tank...)
    // Ở đây ta gán tạm vài giá trị mặc định để tránh lỗi rác bộ nhớ
    width = 20.0f;
    height = 40.0f;
    hp = maxHp = 100;
    speed = 50.0f;
    color = (faction == Faction::PLAYER) ? BLUE : RED;
}

void Unit::Update(float deltaTime, const std::vector<std::shared_ptr<Unit>>& units, Base& playerBase, Base& enemyBase) {
    if (state == UnitState::DEAD) return;

    if (currentCooldown > 0) currentCooldown -= deltaTime;

    // 1. TÌM KIẾM TUYẾN TÍNH O(N): Xác định khoảng cách tới kẻ địch và đồng đội gần nhất
    float minEnemyDist = 999999.0f; 
    float minAllyDist = 999999.0f;

    for (const auto& other : units) {
        if (other.get() == this || other->IsDead()) continue; // Bỏ qua chính mình và xác chết

        float dist = 0.0f;
        bool isInFront = false;

        // Tính khoảng cách dựa trên hướng đi
        if (faction == Faction::PLAYER) {
            dist = other->GetX() - this->x;
            isInFront = (dist > 0); // Chỉ quan tâm mục tiêu phía trước
        } else {
            dist = this->x - other->GetX();
            isInFront = (dist > 0);
        }

        if (isInFront) {
            if (other->GetFaction() != this->faction) {
                if (dist < minEnemyDist) minEnemyDist = dist; // Cập nhật kẻ địch gần nhất
            } else {
                if (dist < minAllyDist) minAllyDist = dist;   // Cập nhật đồng đội gần nhất
            }
        }
    }

    // 2. TÍNH KHOẢNG CÁCH TỚI NHÀ CHÍNH ĐỊCH
    float distToBase = 999999.0f;
    if (faction == Faction::PLAYER) {
        distToBase = enemyBase.GetX() - this->x;
    } else {
        // Phe địch đi sang trái, phải tính tới mép bên phải của nhà chính Player
        distToBase = this->x - (playerBase.GetX() + playerBase.GetWidth()); 
    }
    if (distToBase < minEnemyDist) minEnemyDist = distToBase;

    // 3. QUẢN LÝ TRẠNG THÁI (STATE MACHINE)
    float blockThreshold = width * 0.8f; // Nếu đồng đội đứng cách 80% bề ngang thân thì dừng lại

    if (minEnemyDist <= attackRange) {
        state = UnitState::ATTACKING;
    } else if (minAllyDist <= blockThreshold) {
        state = UnitState::MOVING; // Kẹt đồng đội -> Trạng thái vẫn là đi nhưng Lát nữa sẽ không cộng X
    } else {
        state = UnitState::MOVING;
    }

    // 4. THỰC THI HÀNH ĐỘNG
    if (state == UnitState::MOVING && minAllyDist > blockThreshold) {
        if (faction == Faction::PLAYER) x += speed * deltaTime;
        else x -= speed * deltaTime;
    }
    else if (state == UnitState::ATTACKING) {
        if (currentCooldown <= 0) {
            
            // Xử lý Gây sát thương
            if (distToBase <= attackRange) {
                // Đánh nhà chính nếu vào tầm
                if (faction == Faction::PLAYER) enemyBase.TakeDamage(damage);
                else playerBase.TakeDamage(damage);
            } 
            else {
                // Quét tìm lính địch nằm trong tầm đánh để phang
                for (auto& other : units) {
                    if (other.get() == this || other->IsDead() || other->GetFaction() == this->faction) continue;
                    
                    float dist = 0.0f;
                    if (faction == Faction::PLAYER) dist = other->GetX() - this->x;
                    else dist = this->x - other->GetX();
                    
                    if (dist > 0 && dist <= attackRange) {
                        other->TakeDamage(damage); // Đánh trúng đích!
                        break; // Mỗi đòn chỉ trúng 1 mục tiêu đầu tiên
                    }
                }
            }
            
            // Reset thời gian chờ vung vũ khí
            currentCooldown = attackCooldown; 
        }
    }

}

void Unit::Draw() {
    if (state == UnitState::DEAD) return;
    
    // Vẽ thân Unit (Vẽ ngược từ mặt đất y lên trên)
    DrawRectangle(x, y - height, width, height, color);

    // Vẽ thanh máu (Health Bar) cho lính
    float hpPercentage = (float)hp / maxHp;
    DrawRectangle(x, y - height - 10, width, 5, RED); // Nền đỏ
    DrawRectangle(x, y - height - 10, hpPercentage * width, 5, GREEN); // Máu xanh

    // 3. HIỆU ỨNG CHIẾN ĐẤU (VFX)
    if (state == UnitState::ATTACKING) {
        
        // Thời gian chớp lóe của đòn đánh là 0.15 giây
        float animDuration = 0.15f; 
        
        // Nếu vừa mới tung đòn (Cooldown vừa bị reset về max và bắt đầu đếm ngược)
        if (currentCooldown > attackCooldown - animDuration) {
            
            // Tính toán vị trí cầm vũ khí (Mép trước của nhân vật)
            float weaponX = (faction == Faction::PLAYER) ? (x + width) : (x - 5.0f);
            float weaponY = y - height / 2.0f; // Ở giữa thân

            // Phân loại: Tầm xa (Archer) hay Cận chiến
            if (attackRange > 50.0f) {
                // LOGIC CỦA ARCHER: Vẽ mũi tên/viên đạn bay
                
                // Toán học: Tính % thời gian đạn bay (Từ 0.0 đến 1.0)
                float progress = (attackCooldown - currentCooldown) / animDuration; 
                
                // Quãng đường bay được = Tầm đánh * % thời gian
                float distance = attackRange * progress; 
                
                float bulletX = (faction == Faction::PLAYER) ? (weaponX + distance) : (weaponX - distance);
                
                // Vẽ viên đạn (Hình tròn nhỏ màu vàng)
                DrawCircle((int)bulletX, (int)weaponY, 4.0f, YELLOW);
            } 
            else {
                // LOGIC CỦA WARRIOR/TANK: Vẽ tia chém ngang (Slash)
                
                float slashLength = 20.0f; // Độ vươn của tia chém
                float endX = (faction == Faction::PLAYER) ? (weaponX + slashLength) : (weaponX - slashLength);
                
                // Vẽ tia chém dày 4 pixel
                DrawLineEx({weaponX, weaponY}, {endX, weaponY}, 4.0f, YELLOW);
            }
        }
    }
}
void Unit::TakeDamage(int damageAmount){
    hp -= damageAmount;
    if(hp <= 0){
        state = UnitState::DEAD;
    }
}
bool Unit::IsDead() const {
    return state == UnitState::DEAD;
}

float Unit::GetX() const { return x; }
Faction Unit::GetFaction() const { return faction; }

int Unit::GetCost() const{
    return cost;
}