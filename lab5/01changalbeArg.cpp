#include<iostream>
// #include<stdarg.h>
typedef char* va_list;
#define _INTSIZEOF(n) ((sizeof(n)+sizeof(int)-1)&~(sizeof(int)-1))
#define va_start(ap, v) (ap=(va_list)&v + _INTSIZEOF(v))
#define va_arg(ap, type) (*(type*)((ap+=_INTSIZEOF(type))-_INTSIZEOF(type)))
#define va_end(ap) (ap=(va_list)0)

void print_any_number_of_integers(int n, ...){
    va_list param;  // 指向可变参数的指针
    va_start(param, n); // param指向可变参数列表的第一个参数
    for (int i =0;i<n;i++){
        // 引用param指向的int参数，va_arg会是param自动指向下一个参数
        std::cout<<va_arg(param, int)<<" ";
    }
    va_end(param);
    std::cout<<std::endl;
}

int main(void){
    print_any_number_of_integers(3, 2, 3, 4);
    print_any_number_of_integers(1, 123);
    return 0;
}