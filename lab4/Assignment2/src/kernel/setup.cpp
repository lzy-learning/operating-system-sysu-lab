#include "asm_utils.h"

extern "C" void setup_kernel()
{
    asm_my_info();
    while(1) {

    }
}