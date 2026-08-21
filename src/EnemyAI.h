#pragma once
#include "Unit.h"

class EnemyAI {
private:
    float gold;
    float goldRate;
    float nextSpawnDelay;

public:
    EnemyAI();
    
    int currentWave;
    int unitsToSpawnThisWave;
    float waveTimer;
    bool isBossWave;
    float timeBetweenWaves;

    void Update(float deltaTime);
    
    bool DecideSpawn(UnitType& outType); 
    
    void OnUnitSpawned(int cost);
};