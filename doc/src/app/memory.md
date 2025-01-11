
# 应用程序内存布局

## 32 位

`0000 0000 - 6fff ffff` 内核空间
`7000 0000 - efff ffff` 用户空间
`f000 0000 - ffff ffff` 内核共享数据

- `70000000 - 8fffffff` 代码段、数据段、bss 段
- `90000000 - afffffff` 堆
- `b0000000 - dfffffff` mmap 区域
- `e0000000 - efffffff` 栈
- `f0000000 - ffffffff` 内核共享数据

## 64 位

`0000 0000 0000 - 7fff ffff ffff` 内核空间
`8000 0000 0000 - efff ffff ffff` 用户空间
`f000 0000 0000 - ffff ffff ffff` 内核共享数据

- `8000 0000 0000 - 8fff ffff ffff` 代码段、数据段、bss 段
- `9000 0000 0000 - 9fff ffff ffff` 堆
- `a000 0000 0000 - dfff ffff ffff` mmap 区域
- `e000 0000 0000 - efff ffff ffff` 栈
- `f000 0000 0000 - ffff ffff ffff` 内核共享数据
