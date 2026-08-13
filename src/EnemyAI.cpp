#include "EnemyAI.h"
#include "raylib.h"

EnemyAI::EnemyAI() {
    gold = 100.0f;
    goldRate = 15.0f; // Ngang bằng với người chơi
    nextSpawnDelay = 2.0f; // Vừa vào game, địch sẽ chờ 2 giây mới đánh
}

void EnemyAI::Update(float deltaTime) {
    gold += goldRate * deltaTime;
    
    if (nextSpawnDelay > 0) {
        nextSpawnDelay -= deltaTime;
    }
}

bool EnemyAI::DecideSpawn(UnitType& outType) {
    // Đang trong thời gian chờ -> Không đẻ lính
    if (nextSpawnDelay > 0) return false;

    // Tung xúc xắc chọn ngẫu nhiên: 0(Warrior), 1(Archer), 2(Tank)
    int choice = GetRandomValue(0, 2);
    
    int requiredGold = 0;
    if (choice == 0) requiredGold = 50;
    else if (choice == 1) requiredGold = 75;
    else if (choice == 2) requiredGold = 150;

    // Nếu đủ tiền thì mới quyết định mua
    if (gold >= requiredGold) {
        if (choice == 0) outType = UnitType::WARRIOR;
        else if (choice == 1) outType = UnitType::ARCHER;
        else if (choice == 2) outType = UnitType::TANK;
        return true;
    }

    return false; // Tiền chưa đủ -> Chờ thêm
}

void EnemyAI::OnUnitSpawned(int cost) {
    gold -= cost; // Trừ tiền AI
    // Reset thời gian chờ (Random từ 1.5s đến 4.0s) để lính không ra liên tục như súng liên thanh
    nextSpawnDelay = (float)GetRandomValue(15, 40) / 10.0f; 
}