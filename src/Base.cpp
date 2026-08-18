#include "Base.h"

Base::Base() : x(0), y(0), hp(20000), maxHp(20000), isPlayer(true) {}

Base::Base(float startX, float startY, Texture2D tex, bool isPlayerSide) {
    x = startX;
    y = startY;
    texture = tex;
    isPlayer = isPlayerSide;
    hp = 20000;
    maxHp = 20000;
}
void Base::TakeDamage(float amount) {
    hp -= (int)amount;
    if (hp < 0) hp = 0;
}

void Base::Draw() const {
    float flip = isPlayer ? 1.0f : -1.0f;
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width * flip, (float)texture.height };
    Vector2 position = { x, y - (texture.height / 2.0f) };
    DrawTextureRec(texture, sourceRec, position, WHITE);

    // Thanh máu nhà chính
    float barWidth = 140.0f;
    float barHeight = 18.0f;
    float barX = isPlayer ? 10.0f : 790.0f;
    float barY = 460.0f;

    float hpPercent = (maxHp > 0) ? ((float)hp / (float)maxHp) : 0.0f;
    if (hpPercent < 0.0f) hpPercent = 0.0f;

    DrawRectangle((int)barX, (int)barY, (int)barWidth, (int)barHeight, DARKGRAY);
    DrawRectangle((int)barX, (int)barY, (int)(barWidth * hpPercent), (int)barHeight, (isPlayer ? GREEN : RED));
    DrawRectangleLines((int)barX, (int)barY, (int)barWidth, (int)barHeight, BLACK);

    // Số máu nhà
    const char* hpText = TextFormat("%d / %d", hp, maxHp);
    int fontSize = 12;
    int textWidth = MeasureText(hpText, fontSize);
    int textX = (int)(barX + (barWidth - textWidth) / 2.0f);
    int textY = (int)(barY + (barHeight - fontSize) / 2.0f);

    DrawText(hpText, textX + 1, textY + 1, fontSize, BLACK);
    DrawText(hpText, textX, textY, fontSize, RAYWHITE);
}