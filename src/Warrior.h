#include "Unit.h" 
#include "raylib.h"

class Warrior : public Unit {
private:
    Texture2D texture; 

public:
    Warrior(float startX, float startY, Faction faction, Texture2D tex); 
    
    // Thêm chữ "const" vào đây cho khớp với lớp cha và file .cpp
    void Draw()  override; 
};