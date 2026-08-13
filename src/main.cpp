#include "raylib.h"
#include "Game.h"

int main() {
    const int screenWidth = 1000;
    const int screenHeight = 800; // Giữ nguyên thông số 800 của bạn

    InitWindow(screenWidth, screenHeight, "Age of War");
    SetTargetFPS(60);

    // Tạo ra object game. Constructor của Game sẽ tự động tạo ra 2 nhà chính
    Game game; 

    while (!WindowShouldClose()) {
        // 1. Cập nhật logic toán học
        game.Update();

        // 2. Vẽ hình ảnh lên màn hình
        BeginDrawing();
        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}