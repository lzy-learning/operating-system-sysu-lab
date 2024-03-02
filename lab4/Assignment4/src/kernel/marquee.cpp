#include "os_type.h"
#include "marquee.h"

Marquee::Marquee(){
}

void Marquee::initialize(char* info, uint32 len_info, uint32 start_x, uint32 start_y){
    this->info_ = info;
    this->len_info_ = len_info;
    for(uint32 i = 0;i<len_info;i++){
        this->x_[i] = start_x;
        this->y_[i] = start_y;
        start_y += 1;
    }
}
void Marquee::move_forward(){
    for(uint32 i = 0;i<this->len_info_;i++){
        this->y_[i] += 1;
        this->y_[i] %= 80;
    }
}
void Marquee::move_backward(){
    for(uint32 i = 0;i<this->len_info_;i++){
        this->y_[i] -= 1;
        this->y_[i] += 80;
        this->y_[i] %= 80;
    }
}
void Marquee::move_up(){
    for(uint32 i = 0;i<this->len_info_;i++){
        this->x_[i] -= 1;
        this->x_[i] += 25;
        this->x_[i] %= 25;
    }
}
void Marquee::move_down(){
    for(uint32 i = 0;i<this->len_info_;i++){
        this->x_[i] += 1;
        this->x_[i] %= 25;
    }
}