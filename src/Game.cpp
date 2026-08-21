#include "Game.h"
#include <algorithm>
#include <cmath>
Game::Game() 
    : btnWarrior(10, 48, 125, 38, "Chien binh ($50)", Color{ 37, 99, 235, 255 }),      // Xanh Royal
      btnArcher(140, 48, 125, 38, "Cung thu ($75)", Color{ 217, 119, 6, 255 }),       // Amber Gold
      btnTank(270, 48, 130, 38, "Xe tang ($150)", Color{ 185, 28, 28, 255 }),          // Crimson Red
      btnTurret(410, 48, 135, 38, "Thap canh ($120)", Color{ 13, 148, 136, 255 }),    // Teal
      btnBuyExp(555, 48, 130, 38, "+KN ($50)", Color{ 16, 185, 129, 255 }),        // Emerald
      btnSpecial(695, 48, 140, 38, "THIEN THACH (-150)", Color{ 225, 29, 72, 255 }),     // Rose Red
      btnEvolve(845, 48, 145, 38, "TIEN HOA (400)", Color{ 126, 34, 206, 255 }),     // Purple
      btnRestart(400, 380, 200, 45, "CHOI LAI", Color{ 37, 99, 235, 255 }),
      btnAutoSpawn(845, 88, 145, 25, "Tu dong: TAT", Color{ 100, 100, 100, 255 }),
      btnUpgWarrior(10, 88, 125, 22, "Nang cap CB ($100)", Color{ 30, 41, 59, 255 }),
      btnUpgArcher(140, 88, 125, 22, "Nang cap CT ($150)", Color{ 30, 41, 59, 255 }),
      btnUpgTank(270, 88, 130, 22, "Nang cap XT ($300)", Color{ 30, 41, 59, 255 })
{
    background = LoadTexture("assets/thoikidoda.png");
    warriorTex = LoadTexture("assets/chienbinh.png");
    archerTex  = LoadTexture("assets/cungthu.png"); 
    tankTex    = LoadTexture("assets/tank.png");
    baseTex    = LoadTexture("assets/nha.png");
    
    sfxClash = LoadSound("assets/clash.wav");
    sfxWhoosh = LoadSound("assets/whoosh.wav");
    
    ResetGame();
}

Game::~Game() {
    UnloadTexture(background);
    UnloadTexture(warriorTex);
    UnloadTexture(archerTex); 
    UnloadTexture(tankTex);
    UnloadTexture(baseTex);
    UnloadSound(sfxClash);
    UnloadSound(sfxWhoosh);
}

void Game::ResetGame() {
    activeUnits.clear(); 
    playerGold = 100.0f;
    goldIncreaseRate = 15.0f;
    specialCooldown = 30.0f;

    // Khởi tạo Player
    currentAge = Age::STONE_AGE;
    playerExp = 0.0f;
    expRequirement = 400.0f;
    specialCooldown = 0.0f;

    // Khởi tạo AI
    enemyAge = Age::STONE_AGE;
    enemyExp = 0.0f;
    enemyExpRequirement = 400.0f;
    enemySpecialCooldown = 15.0f;
    
    playerBase = Base(10, 530, baseTex, true);   
    enemyBase  = Base(790, 530, baseTex, false);
    btnTurret.SetText("Thap canh ($120) [0/3]");
    
    enemyAI = EnemyAI();
    currentState = GameState::PLAYING;
    winner = 0;
    
    warriorLvl = 1;
    archerLvl = 1;
    tankLvl = 1;
    btnUpgWarrior.SetText("Nang cap CB ($100)");
    btnUpgArcher.SetText("Nang cap CT ($150)");
    btnUpgTank.SetText("Nang cap XT ($300)");

    playerWall.x = 220.0f;
    playerWall.maxHp = 1000;
    playerWall.hp = 1000;
    playerWall.level = 1;
    playerWall.active = true;
    btnHealWall.SetText("Sua tuong ($100)");
}

float Game::GetAgeMultiplier(Age age) const {
    if (age == Age::MEDIEVAL_AGE) return 1.8f;
    if (age == Age::MODERN_AGE)   return 3.2f;
    return 1.0f;
}

const char* Game::GetAgeName(Age age) const {
    if (age == Age::STONE_AGE)    return "THOI KY DO DA";
    if (age == Age::MEDIEVAL_AGE) return "THOI KY TRUNG CO";
    return "THOI KY HIEN DAI";
}
int Game::CountUnits(Faction faction) const {
    int count = 0;
    for (const auto& unit : activeUnits) {
        if (unit->GetFaction() == faction && !unit->IsDead()) {
            count++;
        }
    }
    return count;
}

void Game::SpawnUnit(Faction faction, UnitType type) {
    if (CountUnits(faction) >= MAX_UNITS) {
        return; 
    }

    float spawnX = (faction == Faction::PLAYER) ? 80.0f : 780.0f;
    float spawnY = 650.0f; 

    // Khai báo hệ số nhân sức mạnh dựa theo thời đại của từng phe
    float ageMultiplier = GetAgeMultiplier(faction == Faction::PLAYER ? currentAge : enemyAge);
    float upgradeMultiplier = 1.0f;
    if (faction == Faction::PLAYER) {
        if (type == UnitType::WARRIOR) upgradeMultiplier = 1.0f + (warriorLvl - 1) * 0.3f;
        else if (type == UnitType::ARCHER) upgradeMultiplier = 1.0f + (archerLvl - 1) * 0.3f;
        else if (type == UnitType::TANK) upgradeMultiplier = 1.0f + (tankLvl - 1) * 0.3f;
    }

    std::shared_ptr<Unit> newUnit = nullptr;
    if (type == UnitType::WARRIOR) {
        newUnit = std::make_shared<Warrior>(spawnX, spawnY, faction, warriorTex);
    } else if (type == UnitType::ARCHER) {
        newUnit = std::make_shared<Archer>(spawnX, spawnY, faction, archerTex);
    } else if (type == UnitType::TANK || type == UnitType::BOSS) {
        newUnit = std::make_shared<Tank>(spawnX, spawnY, faction, tankTex);
    }

    if (newUnit != nullptr) {
        newUnit->ScaleStats(ageMultiplier, upgradeMultiplier);
        
        if (type == UnitType::BOSS) {
            newUnit->SetBoss();
            newUnit->ScaleStats(1.0f, 3.0f + enemyAI.currentWave * 0.5f); // Scale boss by wave
        }

        if (faction == Faction::PLAYER) {
            if (playerGold >= newUnit->GetCost()) {
                playerGold -= newUnit->GetCost();
                activeUnits.push_back(newUnit);
            }
        } else {
            activeUnits.push_back(newUnit);
        }
    }
}
void Game::CastSpecialAttack(Faction caster) {
    if (caster == Faction::PLAYER) {
        if (playerExp < METEOR_EXP_COST || specialCooldown > 0.0f) return;

        playerExp -= METEOR_EXP_COST; // Trừ EXP người chơi
        specialCooldown = MAX_SPECIAL_COOLDOWN;
        PlaySound(sfxWhoosh);

        for (int i = 0; i < 15; i++) {
            meteors.push_back({
                { (float)GetRandomValue(250, 900), (float)GetRandomValue(-400, -50) },
                (float)GetRandomValue(500, 800),
                true,
                Faction::PLAYER
            });
        }
    } else {
        if (enemyExp < METEOR_EXP_COST || enemySpecialCooldown > 0.0f) return;

        enemyExp -= METEOR_EXP_COST; // Trừ EXP của AI
        enemySpecialCooldown = MAX_SPECIAL_COOLDOWN;

        for (int i = 0; i < 15; i++) {
            meteors.push_back({
                { (float)GetRandomValue(100, 750), (float)GetRandomValue(-400, -50) },
                (float)GetRandomValue(500, 800),
                true,
                Faction::ENEMY
            });
        }
    }
}

void Game::UpdateMeteors(float dt) {
    for (auto& m : meteors) {
        if (!m.active) continue;

        m.position.y += m.speed * dt;
        m.position.x += (m.caster == Faction::PLAYER ? -1.0f : 1.0f) * (m.speed * 0.3f) * dt;

        if (m.position.y >= 650.0f) {
            m.active = false;
            // Thiên thạch gây sát thương lên phe đối nghịch
            Faction targetFaction = (m.caster == Faction::PLAYER) ? Faction::ENEMY : Faction::PLAYER;
            for (auto& unit : activeUnits) {
                if (unit->GetFaction() == targetFaction && !unit->IsDead()) {
                    if (fabs(unit->GetX() - m.position.x) < 80.0f) {
                        unit->TakeDamage(180);
                    }
                }
            }
        }
    }
}
void Game::EvolveAge(Faction faction) {
    if (faction == Faction::PLAYER) {
        if (currentAge == Age::STONE_AGE) {
            currentAge = Age::MEDIEVAL_AGE;
            playerExp -= expRequirement;
            expRequirement = 1200.0f; // Mốc thời Trung Cổ

            btnWarrior.SetText("Hiep si ($90)");
            btnArcher.SetText("No thu ($135)");
            btnTank.SetText("May ban da ($270)");
            btnEvolve.SetText("TIEN HOA (1200 KN)");
        } else if (currentAge == Age::MEDIEVAL_AGE) {
            currentAge = Age::MODERN_AGE;
            playerExp -= expRequirement;
            expRequirement = 1500.0f; // Giới hạn trần tối đa của Thời Hiện Đại

            btnWarrior.SetText("Linh bo binh ($160)");
            btnArcher.SetText("Linh tia ($240)");
            btnTank.SetText("Xe tang chien dau ($480)");
            btnEvolve.SetText("TOI DA");
            
            if (!achievementAgeIII) {
                achievementAgeIII = true;
                ShowAchievement("THANH TUU: Dat Thoi Ky Hien Dai! (+300 Vang)");
            }
        }
    } else {
        // AI Evolve
        if (enemyAge == Age::STONE_AGE) {
            enemyAge = Age::MEDIEVAL_AGE;
            enemyExp -= enemyExpRequirement;
            enemyExpRequirement = 1200.0f;
        } else if (enemyAge == Age::MEDIEVAL_AGE) {
            enemyAge = Age::MODERN_AGE;
            enemyExp -= enemyExpRequirement;
            enemyExpRequirement = 1500.0f; // Giới hạn trần tối đa AI
        }
    }
}
void Game::Update() {
    if (currentState == GameState::GAME_OVER) {
        if (btnRestart.IsClicked()) ResetGame();
        return; 
    }

    float deltaTime = GetFrameTime();
    
    if (achievementPopupTimer > 0.0f) {
        achievementPopupTimer -= deltaTime;
    }

    // 1. Tự động tăng Vàng & EXP (Chỉ tăng EXP nếu chưa đạt MAX)
    playerGold += goldIncreaseRate * deltaTime;

    if (playerExp < expRequirement) {
        playerExp += PASSIVE_EXP_RATE * deltaTime;
        if (playerExp > expRequirement) playerExp = expRequirement; // Khóa trần
    }

    if (enemyExp < enemyExpRequirement) {
        enemyExp += PASSIVE_EXP_RATE * deltaTime;
        if (enemyExp > enemyExpRequirement) enemyExp = enemyExpRequirement;
    }

    if (specialCooldown > 0.0f) specialCooldown -= deltaTime;
    if (enemySpecialCooldown > 0.0f) enemySpecialCooldown -= deltaTime;

    // 2. AI Nâng cấp thời đại
    if (enemyAge != Age::MODERN_AGE && enemyExp >= enemyExpRequirement) {
        EvolveAge(Faction::ENEMY);
    }

    // 3. AI Dùng Thiên Thạch
    if (enemySpecialCooldown <= 0.0f && enemyExp >= METEOR_EXP_COST && CountUnits(Faction::PLAYER) >= 3) {
        CastSpecialAttack(Faction::ENEMY);
    }

    // 4. Enemy AI Spawn Lính
    enemyAI.Update(deltaTime);
    UnitType aiSpawnType;
    if (enemyAI.DecideSpawn(aiSpawnType)) {
        SpawnUnit(Faction::ENEMY, aiSpawnType);
        int cost = (int)(50 * GetAgeMultiplier(enemyAge));
        enemyAI.OnUnitSpawned(cost);
    }

    // 5. Thao tác điều khiển của Player
    if (btnWarrior.IsClicked() || IsKeyPressed(KEY_ONE))   SpawnUnit(Faction::PLAYER, UnitType::WARRIOR);
    if (btnArcher.IsClicked()  || IsKeyPressed(KEY_TWO))   SpawnUnit(Faction::PLAYER, UnitType::ARCHER);
    if (btnTank.IsClicked()    || IsKeyPressed(KEY_THREE)) SpawnUnit(Faction::PLAYER, UnitType::TANK);
    
    int costUpgWarrior = 100 * warriorLvl;
    if (btnUpgWarrior.IsClicked() && playerGold >= costUpgWarrior) {
        playerGold -= costUpgWarrior;
        warriorLvl++;
        btnUpgWarrior.SetText(TextFormat("Nang cap CB ($%d)", 100 * warriorLvl));
    }
    int costUpgArcher = 150 * archerLvl;
    if (btnUpgArcher.IsClicked() && playerGold >= costUpgArcher) {
        playerGold -= costUpgArcher;
        archerLvl++;
        btnUpgArcher.SetText(TextFormat("Nang cap CT ($%d)", 150 * archerLvl));
    }
    int costUpgTank = 300 * tankLvl;
    if (btnUpgTank.IsClicked() && playerGold >= costUpgTank) {
        playerGold -= costUpgTank;
        tankLvl++;
        btnUpgTank.SetText(TextFormat("Nang cap XT ($%d)", 300 * tankLvl));
    }
    
    if (btnAutoSpawn.IsClicked()) {
        autoSpawnEnabled = !autoSpawnEnabled;
        if (autoSpawnEnabled) {
            btnAutoSpawn.SetText("Tu dong: BAT");
            btnAutoSpawn.SetRect(Rectangle{845, 88, 145, 25});
        } else {
            btnAutoSpawn.SetText("Tu dong: TAT");
            btnAutoSpawn.SetRect(Rectangle{845, 88, 145, 25});
        }
    }

    if (btnHealWall.IsClicked() && playerGold >= 100) {
        playerGold -= 100;
        playerWall.maxHp += 500;
        playerWall.hp = playerWall.maxHp;
        playerWall.level++;
        playerWall.active = true;
    }

    if (autoSpawnEnabled) {
        autoSpawnTimer -= deltaTime;
        if (autoSpawnTimer <= 0.0f) {
            autoSpawnTimer = 1.0f; // Check every second
            float mul = GetAgeMultiplier(currentAge);
            int count = CountUnits(Faction::PLAYER);
            if (count < 6) {
                // Ưu tiên spawn lính theo thứ tự: Tank -> Archer -> Warrior tuỳ vào tiền
                if (playerGold >= (int)(150 * mul)) {
                    SpawnUnit(Faction::PLAYER, UnitType::TANK);
                } else if (playerGold >= (int)(75 * mul)) {
                    SpawnUnit(Faction::PLAYER, UnitType::ARCHER);
                } else if (playerGold >= (int)(50 * mul)) {
                    SpawnUnit(Faction::PLAYER, UnitType::WARRIOR);
                }
            }
        }
    }

    // MUA EXP: Chỉ cho phép mua khi còn chỗ chứa (< expRequirement)
    bool canBuyExp = (playerGold >= BUY_EXP_GOLD_COST) && (playerExp < expRequirement);
    if ((btnBuyExp.IsClicked() || IsKeyPressed(KEY_B)) && canBuyExp) {
        playerGold -= BUY_EXP_GOLD_COST;
        playerExp  += BUY_EXP_AMOUNT;
        if (playerExp > expRequirement) playerExp = expRequirement; // Khóa trần
    }

    // GỌI THIÊN THẠCH
    if ((btnSpecial.IsClicked() || IsKeyPressed(KEY_SPACE)) && specialCooldown <= 0.0f && playerExp >= METEOR_EXP_COST) {
        CastSpecialAttack(Faction::PLAYER);
    }

    // LÊN ĐỜI NHÀ CHÍNH
    bool canEvolve = (currentAge != Age::MODERN_AGE) && (playerExp >= expRequirement);
    if (canEvolve && (btnEvolve.IsClicked() || IsKeyPressed(KEY_E))) {
        EvolveAge(Faction::PLAYER);
    }

    // 6. Cập nhật các Unit
    for (auto& unit : activeUnits) {
        unit->Update(deltaTime, activeUnits, playerBase, enemyBase, playerWall);

        if (unit->IsAttackingFrame()) {
            Vector2 origin = { unit->GetX(), 650.0f };
            Vector2 target = { unit->GetTargetX(), 650.0f };
            
            if (unit->GetType() == UnitType::WARRIOR) {
                AddEffect(EffectType::SLASH, origin, target, unit->GetFaction());
            } else if (unit->GetType() == UnitType::ARCHER) {
                AddEffect(EffectType::ARROW, origin, target, unit->GetFaction());
            } else if (unit->GetType() == UnitType::TANK) {
                AddEffect(EffectType::CANNON_EXPLOSION, origin, target, unit->GetFaction());
            }
            unit->ResetAttackFrame();
        }
    }

    UpdateEffects(deltaTime);
    UpdateMeteors(deltaTime);

    // 7. Nhận thưởng khi tiêu diệt lính
    for (const auto& unit : activeUnits) {
        if (unit->IsDead()) {
            if (unit->GetFaction() == Faction::ENEMY) {
                playerGold += unit->GetCost() * 0.8f;
                totalKills++;
                if (totalKills >= 100 && !achievement100Kills) {
                    achievement100Kills = true;
                    ShowAchievement("THANH TUU: Ha 100 Dich! (+300 Vang)");
                }
                
                if (playerExp < expRequirement) {
                    playerExp += unit->GetExpReward();
                    if (playerExp > expRequirement) playerExp = expRequirement;
                }
            } else {
                if (enemyExp < enemyExpRequirement) {
                    enemyExp += unit->GetExpReward();
                    if (enemyExp > enemyExpRequirement) enemyExp = enemyExpRequirement;
                }
            }
        }
    }

    // 8. Dọn lính chết
    activeUnits.erase(
        std::remove_if(activeUnits.begin(), activeUnits.end(),
            [](const std::shared_ptr<Unit>& u) { return u->IsDead(); }
        ),
        activeUnits.end()
    );

    for (auto& turret : playerTurrets) {
        turret.Update(deltaTime, activeUnits);
        if (turret.HasShot()) {
            AddEffect(EffectType::ARROW, turret.GetPosition(), turret.GetTargetPos(), Faction::PLAYER);
            turret.ResetShot();
        }
    }

    // 2. Cập nhật bắn tỉa cho danh sách trụ Enemy
    for (auto& turret : enemyTurrets) {
        turret.Update(deltaTime, activeUnits);
        if (turret.HasShot()) {
            AddEffect(EffectType::ARROW, turret.GetPosition(), turret.GetTargetPos(), Faction::ENEMY);
            turret.ResetShot();
        }
    }

    // 3. AI tự động sắm tối đa 3 trụ theo tiến độ trận đấu
    Vector2 enemySlots[3] = { { 830.0f, 540.0f }, { 870.0f, 520.0f }, { 850.0f, 490.0f } };
    if (enemyTurrets.size() < 1 && enemyAge >= Age::STONE_AGE && enemyGold >= 120.0f) {
        enemyTurrets.emplace_back(enemySlots[0].x, enemySlots[0].y, Faction::ENEMY, TurretTier::SLINGSHOT);
    }
    if (enemyTurrets.size() < 2 && enemyAge >= Age::MEDIEVAL_AGE) {
        enemyTurrets.emplace_back(enemySlots[1].x, enemySlots[1].y, Faction::ENEMY, TurretTier::BALLISTA);
    }
    if (enemyTurrets.size() < 3 && enemyAge == Age::MODERN_AGE) {
        enemyTurrets.emplace_back(enemySlots[2].x, enemySlots[2].y, Faction::ENEMY, TurretTier::GATLING_GUN);
    }

    // 4. Player Mua Trụ (Tối đa 3 trụ)
    int turretCost = 120 + (int)playerTurrets.size() * 60; // Trụ 1: $120, Trụ 2: $180, Trụ 3: $240
    bool canBuyTurret = (playerTurrets.size() < 3 && playerGold >= turretCost);

    if ((btnTurret.IsClicked() || IsKeyPressed(KEY_T)) && canBuyTurret) {
        playerGold -= turretCost;

        // 3 Tọa độ cắm tháp trên nóc nhà ta
        Vector2 playerSlots[3] = { { 110.0f, 540.0f }, { 70.0f, 520.0f }, { 90.0f, 490.0f } };
        TurretTier tier = (currentAge == Age::STONE_AGE) ? TurretTier::SLINGSHOT : 
                          (currentAge == Age::MEDIEVAL_AGE ? TurretTier::BALLISTA : TurretTier::GATLING_GUN);

        playerTurrets.emplace_back(playerSlots[playerTurrets.size()].x, playerSlots[playerTurrets.size()].y, Faction::PLAYER, tier);

        if (playerTurrets.size() >= 3) {
            btnTurret.SetText("TOI DA THAP (3/3)");
        } else {
            int nextCost = 120 + (int)playerTurrets.size() * 60;
            btnTurret.SetText(TextFormat("Thap canh ($%d) [%d/3]", nextCost, (int)playerTurrets.size()));
        }
    }

    // 9. Thắng / Thua
    if (playerBase.GetHP() <= 0) {
        currentState = GameState::GAME_OVER;
        winner = 2;
    } else if (enemyBase.GetHP() <= 0) {
        currentState = GameState::GAME_OVER;
        winner = 1;
    }
}
void Game::Draw() {
    ClearBackground(BLACK);

    // 1. Vẽ hình nền bản đồ
    DrawTexturePro(background, 
        Rectangle{ 0.0f, 0.0f, (float)background.width, (float)background.height }, 
        Rectangle{ 0.0f, 0.0f, 1000.0f, 800.0f }, 
        Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

    // 2. Vẽ căn cứ và tháp
    playerBase.Draw();
    enemyBase.Draw();

    for (const auto& turret : playerTurrets) {
        turret.Draw();
    }
    for (const auto& turret : enemyTurrets) {
        turret.Draw();
    }

    // 3. Vẽ lính
    for (const auto& unit : activeUnits) {
        unit->Draw();
    }

    // Vẽ Tường
    if (playerWall.active) {
        DrawRectangle(playerWall.x - 10, 480, 20, 170, DARKGRAY); // The wall
        DrawRectangle(playerWall.x - 15, 470, 30, 10, GRAY);      // Wall top
        
        // Wall HP bar
        float wallHpPercent = (float)playerWall.hp / playerWall.maxHp;
        DrawRectangle(playerWall.x - 20, 450, 40, 6, DARKGRAY);
        DrawRectangle(playerWall.x - 20, 450, 40 * wallHpPercent, 6, BLUE);
    }

    // 4. Vẽ hiệu ứng đạn, kiếm, thiên thạch
    DrawEffects();
    
    for (auto& meteor : meteors) {
        if (meteor.active) {
            DrawCircleV(meteor.position, 8.0f, ORANGE);
            DrawCircleV(meteor.position, 4.0f, YELLOW);
            Vector2 tailEnd = { meteor.position.x - (meteor.caster == Faction::PLAYER ? -20.0f : 20.0f), meteor.position.y - 40.0f };
            DrawLineEx(meteor.position, tailEnd, 4.0f, Fade(ORANGE, 0.6f));
        }
    }

    // ==========================================
    // 5. HIỂN THỊ THÔNG TIN CĂN CỨ (BASE OVERLAY)
    // ==========================================
    // Phe ta: Căn cứ (Y = 380 phía trên nóc)
    DrawRectangleRounded({ 15, 395, 140, 22 }, 0.25f, 4, Color{ 15, 23, 42, 220 });
    DrawRectangleRoundedLines({ 15, 395, 140, 22 }, 0.25f, 4, Color{ 34, 197, 94, 255 }); // <-- Xóa 1.0f
    DrawText(GetAgeName(currentAge), 25, 400, 12, Color{ 74, 222, 128, 255 });

    // Phe địch: Căn cứ
    DrawRectangleRounded({ 845, 395, 140, 22 }, 0.25f, 4, Color{ 15, 23, 42, 220 });
    DrawRectangleRoundedLines({ 845, 395, 140, 22 }, 0.25f, 4, Color{ 239, 68, 68, 255 }); // <-- Xóa 1.0f
    DrawText(GetAgeName(enemyAge), 855, 400, 12, Color{ 248, 113, 113, 255 });

    // ==========================================
    // 6. HEADER DASHBOARD (TOP HUD BAR)
    // ==========================================
    // Khung nền chính Topbar
    DrawRectangle(0, 0, 1000, 92, Color{ 15, 23, 42, 235 });
    DrawLine(0, 92, 1000, 92, Color{ 51, 65, 85, 255 });

    // VÀNG (GOLD)
    DrawRectangleRounded({ 10, 8, 120, 30 }, 0.3f, 4, Color{ 30, 41, 59, 200 });
    DrawText("VANG:", 18, 17, 12, Color{ 203, 213, 225, 255 });
    DrawText(TextFormat("%d", (int)playerGold), 62, 15, 16, Color{ 250, 204, 21, 255 });

    // QUÂN SỐ (UNITS)
    int playerCount = 0;
    for (const auto& u : activeUnits) {
        if (u->GetFaction() == Faction::PLAYER && !u->IsDead()) playerCount++;
    }
    DrawRectangleRounded({ 138, 8, 110, 30 }, 0.3f, 4, Color{ 30, 41, 59, 200 });
    DrawText("QUAN SO:", 146, 17, 12, Color{ 203, 213, 225, 255 });
    DrawText(TextFormat("%d/%d", playerCount, 6), 205, 15, 15, (playerCount >= 6 ? Color{ 239, 68, 68, 255 } : Color{ 56, 189, 248, 255 }));

    // KINH NGHIỆM (EXP BAR)
    DrawRectangleRounded({ 266, 8, 240, 30 }, 0.3f, 4, Color{ 30, 41, 59, 200 });
    DrawText("KN:", 274, 17, 12, Color{ 203, 213, 225, 255 });
    
    // Thanh tiến trình EXP
    float expPercent = (expRequirement > 0) ? (playerExp / expRequirement) : 1.0f;
    if (expPercent > 1.0f) expPercent = 1.0f;
    DrawRectangle(298, 15, 110, 15, Color{ 15, 23, 42, 255 });
    DrawRectangle(298, 15, (int)(110 * expPercent), 15, Color{ 168, 85, 247, 255 });
    DrawRectangleLines(298, 15, 110, 15, Color{ 71, 85, 105, 255 });
    DrawText(TextFormat("%d/%d", (int)playerExp, (int)expRequirement), 415, 17, 11, RAYWHITE);

    // THÔNG TIN AI (ĐỐI THỦ)
    DrawRectangleRounded({ 835, 8, 155, 30 }, 0.3f, 4, Color{ 30, 41, 59, 200 });
    DrawText(TextFormat("DOT: %d", enemyAI.currentWave), 843, 17, 11, Color{ 148, 163, 184, 255 });
    
    if (enemyAI.waveTimer > 0) {
        DrawText(TextFormat("Toi sau %ds", (int)enemyAI.waveTimer), 895, 16, 13, Color{ 56, 189, 248, 255 });
    } else {
        DrawText("DANG DANH", 895, 16, 13, Color{ 248, 113, 113, 255 });
    }

    // ==========================================
    // 7. VẼ CÁC NÚT ĐIỀU KHIỂN (ACTION BUTTONS)
    // ==========================================
    float mul = GetAgeMultiplier(currentAge);
    bool hasUnitSlot = (playerCount < 6);

    btnWarrior.Draw(playerGold >= (int)(50 * mul) && hasUnitSlot);
    btnArcher.Draw(playerGold >= (int)(75 * mul) && hasUnitSlot);
    btnTank.Draw(playerGold >= (int)(150 * mul) && hasUnitSlot);

    // Nút Tháp
    int currentTurretCost = 120 + (int)playerTurrets.size() * 60;
    bool canBuyTurret = (playerTurrets.size() < 3 && playerGold >= currentTurretCost);
    btnTurret.Draw(canBuyTurret);

    // Nút Tài nguyên / Kỹ năng
    btnBuyExp.Draw(playerGold >= 50 && playerExp < expRequirement);
    btnSpecial.Draw(playerExp >= 150.0f && specialCooldown <= 0.0f);
    btnEvolve.Draw(playerExp >= expRequirement && currentAge != Age::MODERN_AGE);
    btnAutoSpawn.Draw(true);

    // Vẽ Nút Upgrade
    btnUpgWarrior.Draw(playerGold >= 100 * warriorLvl);
    btnUpgArcher.Draw(playerGold >= 150 * archerLvl);
    btnUpgTank.Draw(playerGold >= 300 * tankLvl);
    
    btnHealWall.SetRect(Rectangle{410, 88, 135, 22});
    btnHealWall.Draw(playerGold >= 100);

    // VẼ TOOLTIP CHỈ SỐ LÍNH
    if (btnWarrior.IsHovered() || btnArcher.IsHovered() || btnTank.IsHovered()) {
        float ageMul = GetAgeMultiplier(currentAge);
        int hp = 100, dmg = 15;
        float atkSpeed = 1.2f;
        float upgMul = 1.0f;
        
        if (btnWarrior.IsHovered()) upgMul = 1.0f + (warriorLvl - 1) * 0.3f;
        else if (btnArcher.IsHovered()) {
            hp = 80; dmg = 25; atkSpeed = 1.0f;
            upgMul = 1.0f + (archerLvl - 1) * 0.3f;
        } else if (btnTank.IsHovered()) {
            hp = 300; dmg = 40; atkSpeed = 1.2f;
            upgMul = 1.0f + (tankLvl - 1) * 0.3f;
        }
        
        const char* tooltip = TextFormat("Mau: %d | ST: %d | Toc Danh: %.1fs", (int)(hp * ageMul * upgMul), (int)(dmg * ageMul * upgMul), atkSpeed);
        int tooltipWidth = MeasureText(tooltip, 12) + 20;
        DrawRectangle(10, 115, tooltipWidth, 24, Fade(BLACK, 0.85f));
        DrawRectangleLines(10, 115, tooltipWidth, 24, RAYWHITE);
        DrawText(tooltip, 20, 121, 12, RAYWHITE);
    }

    // ==========================================
    // 8. MÀN HÌNH KẾT THÚC GAME
    // ==========================================
    if (achievementPopupTimer > 0.0f) {
        DrawRectangle(350, 150, 300, 50, ColorAlpha(GOLD, 0.8f));
        DrawRectangleLines(350, 150, 300, 50, ORANGE);
        DrawText(achievementText.c_str(), 360, 165, 14, BLACK);
    }

    if (currentState == GameState::GAME_OVER) {
        DrawRectangle(0, 0, 1000, 800, Color{ 0, 0, 0, 200 });
        if (winner == 1) {
            DrawText("CHIEN THANG!", 380, 260, 50, Color{ 34, 197, 94, 255 });
        } else {
            DrawText("THAT BAI!", 400, 260, 50, Color{ 239, 68, 68, 255 });
        }
        btnRestart.Draw(true);
    }

}

void Game::ShowAchievement(std::string text) {
    achievementText = text;
    achievementPopupTimer = 4.0f;
    playerGold += 300;
}

void Game::AddEffect(EffectType type, Vector2 pos, Vector2 target, Faction f) {
    if (type == EffectType::SLASH) PlaySound(sfxClash);
    float duration = (type == EffectType::SLASH) ? 0.2f : 0.35f;
    visualEffects.push_back({ pos, target, type, duration, duration, f });
}

void Game::UpdateEffects(float dt) {
    for (auto& ef : visualEffects) {
        ef.timer -= dt;
    }
    visualEffects.erase(
        std::remove_if(visualEffects.begin(), visualEffects.end(),
            [](const VisualEffect& ef) { return ef.timer <= 0.0f; }),
        visualEffects.end()
    );
}

void Game::DrawEffects() {
    for (const auto& ef : visualEffects) {
        float progress = 1.0f - (ef.timer / ef.maxTimer);

        if (ef.type == EffectType::SLASH) {
            // Hiệu ứng chém xoẹt vàng sáng rực
            float slashX = ef.position.x + (ef.faction == Faction::PLAYER ? 40.0f : -40.0f);
            DrawCircleSector({ slashX, ef.position.y - 35 }, 35.0f, 
                             (ef.faction == Faction::PLAYER ? 300 : 120), 
                             (ef.faction == Faction::PLAYER ? 440 : 260), 16, 
                             Fade(YELLOW, 1.0f - progress));
            DrawCircleSectorLines({ slashX, ef.position.y - 35 }, 38.0f, 
                                  (ef.faction == Faction::PLAYER ? 300 : 120), 
                                  (ef.faction == Faction::PLAYER ? 440 : 260), 16, 
                                  Fade(WHITE, 1.0f - progress));
        } 
        else if (ef.type == EffectType::ARROW) {
            // Mũi tên bay thẳng từ Cung thủ sang Mục tiêu
            float currentX = ef.position.x + (ef.target.x - ef.position.x) * progress;
            float currentY = (ef.position.y - 35);
            
            // Vẽ thân tên
            DrawLineEx({ currentX - 15, currentY }, { currentX + 15, currentY }, 4.0f, DARKBROWN);
            // Đầu mũi tên nhọn
            DrawTriangle({ currentX + (ef.faction == Faction::PLAYER ? 18.0f : -18.0f), currentY },
                         { currentX, currentY - 5 }, { currentX, currentY + 5 }, RED);
        } 
        else if (ef.type == EffectType::CANNON_EXPLOSION) {
            // Đạn pháo bay và nổ bùm tại mục tiêu
            if (progress < 0.6f) {
                float bulletX = ef.position.x + (ef.target.x - ef.position.x) * (progress / 0.6f);
                DrawCircle((int)bulletX, (int)(ef.position.y - 30), 8.0f, DARKGRAY);
                DrawCircle((int)bulletX, (int)(ef.position.y - 30), 4.0f, ORANGE);
            } else {
                // Vụ nổ lửa lan to
                float blastRadius = 45.0f * ((progress - 0.6f) / 0.4f);
                DrawCircle((int)ef.target.x, (int)(ef.target.y - 30), blastRadius, Fade(ORANGE, 1.0f - progress));
                DrawCircle((int)ef.target.x, (int)(ef.target.y - 30), blastRadius * 0.6f, Fade(YELLOW, 1.0f - progress));
                DrawCircle((int)ef.target.x, (int)(ef.target.y - 30), blastRadius * 0.3f, Fade(WHITE, 1.0f - progress));
            }
        }
    }
}
