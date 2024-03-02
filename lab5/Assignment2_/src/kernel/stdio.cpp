#include "stdio.h"
#include "os_type.h"
#include "asm_utils.h"
#include "os_modules.h"
#include "stdarg.h"
#include "stdlib.h"

STDIO::STDIO()
{
    initialize();
}

void STDIO::initialize()
{
    screen = (uint8 *)0xb8000;
}

void STDIO::print(uint x, uint y, uint8 c, uint8 color)
{

    if (x >= 25 || y >= 80)
    {
        return;
    }

    uint pos = x * 80 + y;
    screen[2 * pos] = c;
    screen[2 * pos + 1] = color;
}

void STDIO::print(uint8 c, uint8 color)
{
    uint cursor = getCursor();
    screen[2 * cursor] = c;
    screen[2 * cursor + 1] = color;
    cursor++;
    if (cursor == 25 * 80)
    {
        rollUp();
        cursor = 24 * 80;
    }
    moveCursor(cursor);
}

void STDIO::print(uint8 c)
{
    print(c, 0x07);
}


int STDIO::print(const char* const str){
    int i;
    for(i = 0;str[i] != '\0';i++){
        // 如果是换行符则进行换行
        if(str[i] == '\n'){
            uint cur_r = uint(this->getCursor() / 80);
            if(cur_r == 24)this->rollUp();
            else ++cur_r;

            // 记得移动光标位置
            this->moveCursor(cur_r*80);
        }
        else{
            this->print(str[i]);
        }
    }
    return i;
}
void STDIO::moveCursor(uint position)
{
    if (position >= 80 * 25)
    {
        return;
    }

    uint8 temp;

    // 处理高8位
    temp = (position >> 8) & 0xff;
    // 将0x0e写入0x3d4表示处理光标的高8位
    asm_out_port(0x3d4, 0x0e);
    // 将光标位置的高8位写入0x3d5端口，表示更改光标位置
    asm_out_port(0x3d5, temp);
    // 从0x3d5可以读出光标所在位置

    // 处理低8位
    temp = position & 0xff;
    asm_out_port(0x3d4, 0x0f);
    asm_out_port(0x3d5, temp);
}

uint STDIO::getCursor()
{
    uint pos;
    uint8 temp;

    pos = 0;
    temp = 0;
    // 处理高8位
    asm_out_port(0x3d4, 0x0e);
    asm_in_port(0x3d5, &temp);
    pos = ((uint)temp) << 8;

    // 处理低8位
    asm_out_port(0x3d4, 0x0f);
    asm_in_port(0x3d5, &temp);
    pos = pos | ((uint)temp);

    return pos;
}

void STDIO::moveCursor(uint x, uint y)
{
    if (x >= 25 || y >= 80)
    {
        return;
    }

    moveCursor(x * 80 + y);
}
// 字符占满屏幕后我们需要向上滚屏
void STDIO::rollUp()
{
    uint length;
    length = 25 * 80;
    for (uint i = 80; i < length; ++i)
    {
        screen[2 * (i - 80)] = screen[2 * i];
        screen[2 * (i - 80) + 1] = screen[2 * i + 1];
    }

    for (uint i = 24 * 80; i < length; ++i)
    {
        screen[2 * i] = ' ';
        screen[2 * i + 1] = 0x07;
    }
}

// 将字符c输出到缓冲区，如果缓冲区已满则打印缓冲区字符串到显示屏
int print_to_buf(char* buf, char c, int &buf_len, const int MAX_LEN){
    int count = 0;
    buf[buf_len++] = c;
    if(buf_len == MAX_LEN){
        buf[buf_len] = '\0';
        // stdio实例在os_module.h中声明
        count = stdio.print(buf);
        buf_len = 0;
    }
    return count;
}
// 格式化输出字符串
int printf(const char* const fmt, ...){
    // 缓冲区大小
    const int BUF_LEN = 64;
    char buf[BUF_LEN+1];
    // 保护模式下最长32位，33位是为了放\0
    char number[33];

    int buf_len = 0, count = 0;
    va_list ap;

    va_start(ap, fmt);
    
    for(int i = 0;fmt[i] != '\0';i++){
        // 如果是普通字符则直接输出到缓冲区
        if(fmt[i] != '%')count+=print_to_buf(buf, fmt[i], buf_len, BUF_LEN);

        // 否则进行格式化输出
        else{
            i++;
            if(fmt[i] == '\0')break;
            switch (fmt[i]){
                // 转义输出
                case '%':
                    count += print_to_buf(buf, fmt[i], buf_len, BUF_LEN);
                    break;
                // 输出字符
                case 'c':
                    count += print_to_buf(buf, va_arg(ap, char), buf_len, BUF_LEN);
                    break;
                // 输出字符串
                case 's':
                    buf[buf_len] = '\0';
                    buf_len = 0;
                    count += stdio.print(buf);
                    count += stdio.print(va_arg(ap, const char*));
                    break;

                // 输出任意进制的整数
                case 'd':
                case 'x':
                    int tmp = va_arg(ap, int);
                    // 如果是负数要先输出负号然后输出正数
                    if(tmp < 0){
                        count += print_to_buf(buf, '-', buf_len, BUF_LEN);
                        tmp = -tmp;
                    }
                    itos(number, tmp, (fmt[i] == 'd' ? 10 : 16));
                    for(int j = 0;number[j] != '\0';j++)
                        count += print_to_buf(buf, number[j], buf_len, BUF_LEN);
                    break;
                // 输出float浮点数
                // case 'f':
                    float num = va_arg(ap, float);
                    if(num < 0){
                        count += print_to_buf(buf, '-', buf_len, BUF_LEN);
                        num = -num;
                    }
                    ftos(number, num);
                    for(int j = 0;number[j] != '\0';j++)count += print_to_buf(buf, number[j], buf_len, BUF_LEN);
                    break;
            }
        }
    }
    // 将缓冲区剩余内容清空
    buf[buf_len] = '\0';
    count += stdio.print(buf);
    return count;
}