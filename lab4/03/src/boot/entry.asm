; 一个进入内核函数是C语言实现的，一个setup_kernel是asm代码
global enter_kernel
extern setup_kernel
enter_kernel:
    jmp setup_kernel