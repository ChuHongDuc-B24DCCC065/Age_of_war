#include "EnemyAI.h"
#include "raylib.h"

EnemyAI::EnemyAI() {
    gold = 100.0f;
    goldRate = 15.0f; 
    currentWave = 1;
    unitsToSpawnThisWave = 3;
    waveTimer = 15.0f; 
    timeBetweenWaves = 30.0f;
    nextSpawnDelay = 0.0f;
    isBossWave = false;
}

void EnemyAI::Update(float deltaTime) {
    gold += goldRate * deltaTime;
    
    if (waveTimer > 0) {
        waveTimer -= deltaTime;
    } else if (nextSpawnDelay > 0) {
        nextSpawnDelay -= deltaTime;
    }
}

bool EnemyAI::DecideSpawn(UnitType& outType) {
    if (waveTimer > 0) return false;
    if (nextSpawnDelay > 0) return false;

    if (unitsToSpawnThisWave <= 0 && !isBossWave) {
        isBossWave = true;
        nextSpawnDelay = 3.0f;
        return false;
    }

    if (isBossWave) {
        outType = UnitType::BOSS;
        return true;
    }

    int choice = GetRandomValue(0, 2);
    if (choice == 0) outType = UnitType::WARRIOR;
    else if (choice == 1) outType = UnitType::ARCHER;
    else outType = UnitType::TANK;

    return true;
}

void EnemyAI::OnUnitSpawned(int cost) {
    gold -= cost; 

    if (isBossWave) {
        isBossWave = false;
        currentWave++;
        unitsToSpawnThisWave = 3 + currentWave * 2;
        waveTimer = timeBetweenWaves;
        gold += 200; 
    } else {
        unitsToSpawnThisWave--;
        nextSpawnDelay = (float)GetRandomValue(15, 35) / 10.0f; 
    }
}