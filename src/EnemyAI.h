#pragma once
#include "Unit.h"

class EnemyAI {
private:
    float gold;
    float goldRate;
    float nextSpawnDelay;

public:
    EnemyAI();
    
    void Update(float deltaTime);
    
    // Tham chiếu &outType giúp hàm "tuồn" dữ liệu loại lính ra ngoài
    bool DecideSpawn(UnitType& outType); 
    
    void OnUnitSpawned(int cost);
};