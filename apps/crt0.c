// #include <define.h>

// extern void __libc_start_main(int argc, char **argv, char **envp);

// static void libc_start_main(int argc, char **argv, char **envp) {
//   __libc_start_main(argc, argv, envp);
// }

// NAKED void _start() {
//   asm volatile("push %%edx\n\t"
//                "push %%esi\n\t"
//                "push %%edi\n\t"
//                "call *%0\n\t"
//                "mov %%ebx, 0xffff\n\t"
//                "mov %%eax, 0\n\t"
//                "int $0x36\n\t"
//                "ud2\n\t"
//                "int $3\n\t"
//                "1:"
//                "jmp 1\n\t" ::"r"(&libc_start_main));
// }
