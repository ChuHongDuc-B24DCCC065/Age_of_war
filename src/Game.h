#pragma once
#include "Base.h"
#include "Unit.h"
#include <vector>
#include <memory> 
#include "EnemyAI.h"
#include "Button.h"

enum class GameState { PLAYING, GAME_OVER};

class Game {
private:
    Base playerBase;
    Base enemyBase;
    std::vector<std::shared_ptr<Unit>> activeUnits;

    float playerGold;
    float goldIncreaseRate;
    EnemyAI enemyAI;

    Button btnWarrior;
    Button btnArcher;
    Button btnTank;

    GameState currentState;
    int winner;
    Button btnRestart;

    void ResetGame();

public:
    Game(); 
    
    void Update();
    void Draw(); // Dòng này lúc nãy bạn vô tình làm mất
    
    void SpawnUnit(Faction faction, UnitType type); 
};