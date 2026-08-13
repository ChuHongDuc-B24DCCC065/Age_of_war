#include "Game.h"
#include "Warrior.h" // Include Warrior để đẻ lính
#include "Archer.h"
#include "Tank.h"
#include <algorithm>
Game::Game() 
    : playerBase(50, 530, BLUE), enemyBase(870, 530, RED),
      // Xếp 3 nút nằm ngang nhau ở góc trên bên trái
      btnWarrior(20, 60, 160, 40, "Warrior ($50)", SKYBLUE),
      btnArcher(200, 60, 160, 40, "Archer ($75)", ORANGE),
      btnTank(380, 60, 160, 40, "Tank ($150)", MAROON),
      btnRestart(400, 400, 200, 50, "PLAY AGAIN", LIGHTGRAY)
{
    ResetGame();
}
void Game::ResetGame() {
    activeUnits.clear(); // Thuật toán tự động giải phóng toàn bộ RAM của lính cũ
    playerGold = 100.0f;
    goldIncreaseRate = 15.0f;
    
    // Gán lại nhà chính mới (Máu đầy)
    playerBase = Base(50, 530, BLUE);
    enemyBase = Base(870, 530, RED);
    
    enemyAI = EnemyAI(); // Đặt lại AI kẻ thù
    
    currentState = GameState::PLAYING;
    winner = 0;
}
void Game::SpawnUnit(Faction faction, UnitType type) {
    // Vị trí đẻ lính (Ngay trước nhà chính)
    float spawnX = (faction == Faction::PLAYER) ? 100.0f : 850.0f;
    float spawnY = 650.0f; // Mặt đất

    std::shared_ptr<Unit> newUnit=nullptr;
    // Tạo lính dựa trên loại yêu cầu
    if (type == UnitType::WARRIOR) {
        newUnit = std::make_shared<Warrior>(spawnX, spawnY, faction);
    } else if (type == UnitType::ARCHER) {
        newUnit = std::make_shared<Archer>(spawnX, spawnY, faction);
    } else if (type == UnitType::TANK) {
        newUnit = std::make_shared<Tank>(spawnX, spawnY, faction);
    }

    if (newUnit != nullptr) {
        if (faction == Faction::PLAYER) {
            // Kiểm tra xem có đủ tiền không
            if (playerGold >= newUnit->GetCost()) {
                playerGold -= newUnit->GetCost(); // Trừ tiền
                activeUnits.push_back(newUnit);   // Đẩy lính ra trận
            }
        } else {
            // Tạm thời phe Địch đẻ lính không tốn tiền (AI sẽ xử lý vụ tiền của địch sau)
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
    DrawRectangle(0, 650, 1000, 150, LIGHTGRAY); 
    playerBase.Draw();
    enemyBase.Draw();
    for (auto& unit : activeUnits) {
        unit->Draw();
    }
    DrawText(TextFormat("GOLD: %d", (int)playerGold), 20, 20, 30, ORANGE);
    btnWarrior.Draw(playerGold >= 50);
    btnArcher.Draw(playerGold >= 75);
    btnTank.Draw(playerGold >= 150);

    if (currentState == GameState::GAME_OVER) {
        // Vẽ hình chữ nhật đen mờ (Alpha 150/255) đè lên cả màn hình
        DrawRectangle(0, 0, 1000, 800, { 0, 0, 0, 150 });
        
        if (winner == 1) {
            DrawText("VICTORY!", 380, 300, 50, GREEN);
        } else {
            DrawText("DEFEAT!", 400, 300, 50, RED);
        }
        
        btnRestart.Draw(true); // Vẽ nút Play Again (luôn đủ điều kiện bấm = true)
    }
}