#pragma once
#include "Unit.h"

// Lớp Warrior kế thừa (public) từ lớp Unit
class Warrior : public Unit {
public:
    Warrior(float startX, float startY, Faction faction);
};