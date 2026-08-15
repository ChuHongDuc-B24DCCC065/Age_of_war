#include "Game.h"
#include "Warrior.h" // Include Warrior để đẻ lính
#include "Archer.h"
#include "Tank.h"
#include <algorithm>
Game::Game() 
    : 
      // Xếp 3 nút nằm ngang nhau ở góc trên bên trái
      btnWarrior(20, 60, 160, 40, "Warrior ($50)", SKYBLUE),
      btnArcher(200, 60, 160, 40, "Archer ($75)", ORANGE),
      btnTank(380, 60, 160, 40, "Tank ($150)", MAROON),
      btnRestart(400, 400, 200, 50, "PLAY AGAIN", LIGHTGRAY)
{
    background = LoadTexture("assets/thoikidoda.png");
    warriorTex = LoadTexture("assets/chienbinh.png");
    archerTex = LoadTexture("assets/cungthu.png"); 
    tankTex = LoadTexture("assets/tank.png");
    baseTex = LoadTexture("assets/nha.png");
    ResetGame();

}
Game::~Game() {
    UnloadTexture(background);
    UnloadTexture(warriorTex);
    UnloadTexture(archerTex); 
    UnloadTexture(tankTex);
    UnloadTexture(baseTex);
}
void Game::ResetGame() {
    activeUnits.clear(); // Thuật toán tự động giải phóng toàn bộ RAM của lính cũ
    playerGold = 100.0f;
    goldIncreaseRate = 15.0f;
    
    // Gán lại nhà chính mới (Máu đầy)
    playerBase = Base(50, 530, baseTex, true);   
    enemyBase = Base(870, 530, baseTex, false);
    
    enemyAI = EnemyAI(); // Đặt lại AI kẻ thù
    
    currentState = GameState::PLAYING;
    winner = 0;
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

    float spawnX = (faction == Faction::PLAYER) ? 100.0f : 850.0f;
    float spawnY = 650.0f; 

    std::shared_ptr<Unit> newUnit = nullptr;
    if (type == UnitType::WARRIOR) {
        newUnit = std::make_shared<Warrior>(spawnX, spawnY, faction, warriorTex);
    } else if (type == UnitType::ARCHER) {
        newUnit = std::make_shared<Archer>(spawnX, spawnY, faction, archerTex);
    } else if (type == UnitType::TANK) {
        newUnit = std::make_shared<Tank>(spawnX, spawnY, faction, tankTex);
    }

    if (newUnit != nullptr) {
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

void Game::Update() {
    // Nếu game đã kết thúc, chỉ kiểm tra nút bấm Restart, KHÔNG chạy logic lính nữa
    if (currentState == GameState::GAME_OVER) {
        if (btnRestart.IsClicked()) {
            ResetGame();
        }
        return; // Lệnh return này sẽ chặn mọi code update phía dưới
    }
    float deltaTime = GetFrameTime();

    // Hệ thống kinh tế: Tăng vàng liên tục
    playerGold += goldIncreaseRate * deltaTime;
    // --- LOGIC CỦA ENEMY AI ---
    enemyAI.Update(deltaTime);
    UnitType aiSpawnType;
    
    // Nếu AI quyết định đẻ lính, aiSpawnType sẽ được gán giá trị
    if (enemyAI.DecideSpawn(aiSpawnType)) {
        SpawnUnit(Faction::ENEMY, aiSpawnType); // Đẻ lính
        
        // Tính tiền để trừ của AI
        int cost = 0;
        if (aiSpawnType == UnitType::WARRIOR) cost = 50;
        else if (aiSpawnType == UnitType::ARCHER) cost = 75;
        else if (aiSpawnType == UnitType::TANK) cost = 150;
        
        enemyAI.OnUnitSpawned(cost);
    }
    // --------------------------

    if (btnWarrior.IsClicked() || IsKeyPressed(KEY_ONE)) SpawnUnit(Faction::PLAYER, UnitType::WARRIOR);
    if (btnArcher.IsClicked() || IsKeyPressed(KEY_TWO)) SpawnUnit(Faction::PLAYER, UnitType::ARCHER);
    if (btnTank.IsClicked() || IsKeyPressed(KEY_THREE)) SpawnUnit(Faction::PLAYER, UnitType::TANK);

    playerBase.Update();
    enemyBase.Update();
    
    for (auto& unit : activeUnits) {
        unit->Update(deltaTime, activeUnits, playerBase, enemyBase);
    }
    for (const auto& unit : activeUnits) {
        if (unit->IsDead()) {
            // Nếu lính bị tiêu diệt thuộc phe địch -> thưởng vàng cho người chơi
            if (unit->GetFaction() == Faction::ENEMY) {
                playerGold += unit->GetCost() * 0.8f; // Thưởng 80% giá trị lính
            }
        }
    }

    // Dọn rác (Giữ nguyên)
    activeUnits.erase(
        std::remove_if(activeUnits.begin(), activeUnits.end(),
            [](const std::shared_ptr<Unit>& u) { return u->IsDead(); }
        ),
        activeUnits.end()
    );
    if (playerBase.GetHP() <= 0) {
        currentState = GameState::GAME_OVER;
        winner = 2; // Địch thắng
    } else if (enemyBase.GetHP() <= 0) {
        currentState = GameState::GAME_OVER;
        winner = 1; // Ta thắng
    }
}

void Game::Draw() {
    ClearBackground(RAYWHITE);

    // 1. Ép ảnh nền co giãn vừa khít toàn bộ màn hình 1000x800
    Rectangle sourceRec = { 0.0f, 0.0f, (float)background.width, (float)background.height };
    Rectangle destRec   = { 0.0f, 0.0f, 1000.0f, 800.0f }; // Kích thước cửa sổ
    DrawTexturePro(background, sourceRec, destRec, { 0, 0 }, 0.0f, WHITE);

    // 2. XÓA HOẶC COMMENT DÒNG NÀY ĐỂ BỎ KHỐI ĐẤT XÁM:
    // DrawRectangle(0, 650, 1000, 150, LIGHTGRAY); 

    // 3. Vẽ nhà chính và lính (sẽ đứng trực tiếp lên thảm cỏ của ảnh nền)
    playerBase.Draw();
    enemyBase.Draw();
    for (auto& unit : activeUnits) {
        unit->Draw();
    }

    // 4. Vẽ thanh UI bán trong suốt ở trên cùng
    DrawRectangle(0, 0, 1000, 120, { 0, 0, 0, 150 });
    DrawText(TextFormat("GOLD: %d", (int)playerGold), 20, 20, 30, YELLOW);
DrawText(TextFormat("UNITS: %d/%d", CountUnits(Faction::PLAYER), MAX_UNITS), 220, 20, 30, SKYBLUE);
    btnWarrior.Draw(playerGold >= 50);
    btnArcher.Draw(playerGold >= 75);
    btnTank.Draw(playerGold >= 150);

    if (currentState == GameState::GAME_OVER) {
        DrawRectangle(0, 0, 1000, 800, { 0, 0, 0, 150 });
        if (winner == 1) DrawText("VICTORY!", 380, 300, 50, GREEN);
        else DrawText("DEFEAT!", 400, 300, 50, RED);
        btnRestart.Draw(true);
    }
}
