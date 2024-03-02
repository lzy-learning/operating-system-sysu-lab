#include "os_type.h"

template<typename T>
void swap(T &x, T &y) {
    T z = x;
    x = y;
    y = z;
}


void itos(char *numStr, uint32 num, uint32 mod) {
    // 只能转换2~26进制的整数
    if (mod < 2 || mod > 26 || num < 0) {
        return;
    }

    uint32 length, temp;

    // 进制转换
    length = 0;
    while(num) {
        temp = num % mod;
        num /= mod;
        numStr[length] = temp > 9 ? temp - 10 + 'A' : temp + '0';
        ++length;
    }

    // 特别处理num=0的情况
    if(!length) {
        numStr[0] = '0';
        ++length;
    }

    // 将字符串倒转，使得numStr[0]保存的是num的高位数字
    for(int i = 0, j = length - 1; i < j; ++i, --j) {
        swap(numStr[i], numStr[j]);
    }
    
    numStr[length] = '\0';
}

// 初始化一段内存空间，大小为value*length个字节
void memset(void *memory, char value, int length)
{
    for (int i = 0; i < length; ++i)
    {
        ((char *)memory)[i] = value;
    }
}

void ftos(char *numStr, float num){
    float esp = 1e-6;
    float frac_part;
    int int_part;
    int len_str;

    // 找出小数部分和整数部分
    int_part = (int)num;
    frac_part = num - float(int_part);
    // 转化出来的字符串长度
    len_str = 0;

    // 转化整数部分
    while(int_part!=0){
        numStr[len_str++] = (int_part%10)+'0';
        int_part /= 10;
    }
    for(int l=0, r=len_str-1;l<r;l++,r--)
        swap(numStr[l], numStr[r]);
    
    numStr[len_str++] = '.';
    if(frac_part <= esp){
        numStr[len_str++] = '0';
        numStr[len_str] = '\0';
        return;
    }

    // 转换小数部分
    while(frac_part > esp){
        numStr[len_str++] = (int)(frac_part*10)+'0';
        frac_part = float(frac_part*10.0-10.0);
    }
    numStr[len_str] = '\0';
}
