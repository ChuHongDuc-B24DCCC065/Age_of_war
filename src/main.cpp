#include "raylib.h"
#include "Game.h"

int main() {
    const int screenWidth = 1000;
    const int screenHeight = 800; // Giữ nguyên thông số 800 của bạn

    InitWindow(screenWidth, screenHeight, "Age of War");
    InitAudioDevice();
    SetTargetFPS(60);

    // Tạo ra object game. Constructor của Game sẽ tự động tạo ra 2 nhà chính
    Game game; 
    

    while (!WindowShouldClose()) {
        game.Update();
        BeginDrawing();
        game.Draw();                         
    EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}