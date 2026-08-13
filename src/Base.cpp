#include "Base.h"
#include "raylib.h"
Base::Base(float startX, float startY, Color teamColor){
    x=startX;
    y=startY;
    width=80.0f;
    height=120.0f;
    maxHp =1000;
    hp=maxHp;
    color=teamColor;
}
void Base::Update(){

}
void Base::Draw(){
    DrawRectangle(x,y,width,height,color);
    float hpPercentage=(float)hp / maxHp;
    DrawRectangle(x,y-20,width,10,RED);
    DrawRectangle(x,y-20,hpPercentage*width,10,GREEN);
}
int Base::GetHP() const {
    return hp;
}
float Base::GetX() const{return x;}
float Base::GetWidth() const{return width;}
void Base::TakeDamage (int damageAmount){
    hp -= damageAmount;
    if(hp<0) hp=0;
}