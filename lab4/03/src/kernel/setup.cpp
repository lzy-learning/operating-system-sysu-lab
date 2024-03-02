# include "asm_utils.h"
extern "C" void setup_kernel(){
    // 该函数汇编代码实现，在qemu显示屏上输出hello world
    asm_hello_world();
    while(1){

    }
}