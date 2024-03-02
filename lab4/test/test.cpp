#include<stdio.h>

extern "C" void print(int id){
    printf("this is %d\n", id);
}
int id = 100;
int main(void){
    print(id);
    return 0;
}