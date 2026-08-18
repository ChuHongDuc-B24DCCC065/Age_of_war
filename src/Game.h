#pragma once
#include "raylib.h"
#include <vector>
#include <memory>
#include "Unit.h"
#include "Warrior.h"
#include "Archer.h"
#include "Tank.h"
#include "Base.h"
#include "Button.h"
#include "EnemyAI.h"
#include "Turret.h"
enum class GameState { 
    PLAYING, 
    GAME_OVER 
};

enum class Age {
    STONE_AGE = 0,
    MEDIEVAL_AGE = 1,
    MODERN_AGE = 2
}; 
struct Meteor {
    Vector2 position;
    float speed;
    bool active;
    Faction caster;
};
enum class EffectType { SLASH, ARROW, CANNON_EXPLOSION };

struct VisualEffect {
    Vector2 position;
    Vector2 target;
    EffectType type;
    float timer;
    float maxTimer;
    Faction faction;
};
class Game {
private:
    const int MAX_UNITS = 20;

    const int BUY_EXP_GOLD_COST = 50;     // Dùng 50 Vàng để mua EXP
    const float BUY_EXP_AMOUNT = 80.0f;   // Nhận được 80 EXP
    const float METEOR_EXP_COST = 150.0f;

    Age currentAge;
    float playerExp;
    float expRequirement;

    Age enemyAge;
    float enemyExp;
    float enemyExpRequirement;
    float enemySpecialCooldown;
    float enemyGold;
    
    Base playerBase;
    Base enemyBase;

    
    std::vector<Turret> playerTurrets;
    std::vector<Turret> enemyTurrets;
    std::vector<std::shared_ptr<Unit>> activeUnits;

    float playerGold;
    float goldIncreaseRate;
    EnemyAI enemyAI;

   Button btnWarrior;
    Button btnArcher;
    Button btnTank;
    Button btnTurret;
    Button btnBuyExp;
    Button btnSpecial;
    Button btnEvolve;     
    Button btnRestart;

    float specialCooldown;
    const float MAX_SPECIAL_COOLDOWN = 30.0f;
    const float PASSIVE_EXP_RATE = 10.0f;

    std::vector<Meteor> meteors;

    Texture2D background;
    Texture2D warriorTex;
    Texture2D archerTex; 
    Texture2D tankTex;
    Texture2D baseTex;

    std::vector<VisualEffect> visualEffects;

    GameState currentState;
    int winner;
    void EvolveAge(Faction faction);
    int CountUnits(Faction faction) const;
    float GetAgeMultiplier(Age age) const;
    const char* GetAgeName(Age age) const;

    void CastSpecialAttack(Faction caster);
    void UpdateMeteors(float dt);
    void AddEffect(EffectType type, Vector2 pos, Vector2 target, Faction f);
    void UpdateEffects(float dt);
    void DrawEffects();
public:
    Game();
    ~Game();
    void Update();
    void Draw();
    void ResetGame();
    void SpawnUnit(Faction faction, UnitType type);
};

