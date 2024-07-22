#include <define.h>
#include <type.h>
void kernel_main() {
    *(u8 *)(0xB8000) = 'K';
    *(u8 *)(0xB8001) = 0x0c;
    for(;;);
}