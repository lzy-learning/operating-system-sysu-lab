#ifndef MARQUEE_H
#define MARQUEE_H
#include "os_type.h"
class Marquee{
   
public:
    char* info_;
    uint32 len_info_;
    uint32 x_[64];
    uint32 y_[64];
    // 初始化时传入跑马灯信息以及初始起点的坐标
    Marquee();
    void initialize(char* info, uint32 len_info, uint32 start_x, uint32 start_y);
    void move_forward();    // 向前移动
    void move_backward();   // 向后移动
    void move_up();         // 向上移动
    void move_down();       // 向下移动
};

#endif