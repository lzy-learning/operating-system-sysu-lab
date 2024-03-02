#ifndef STDLIB_H
#define STDLIB_H

#include "os_type.h"

template<typename T>
void swap(T &x, T &y);

/*
 * 将一个非负整数转换为指定进制表示的字符串。
 * num: 待转换的非负整数。
 * mod: 进制。
 * numStr: 保存转换后的字符串，其中，numStr[0]保存的是num的高位数字，以此类推。
 */

void itos(char *numStr, uint32 num, uint32 mod);

// 初始化一段连续的内存空间
void memset(void*, char, int);

// 将一个十进制非负小数转成字符串
void ftos(char *numStr, float num);

#endif