
# Plant-OS System Call / Plant-OS 系统调用

Plant-OS 遵照 Linux 的系统调用规范，但中断号有所变动使用 `int 0x36` 指令来调用系统调用，`sysenter` 和 `syscall` 指令不受影响。

## 系统调用列表

| ID | Name     | 名称         | Param-Nums | Description                       | 描述                   |
|----|----------|--------------|------------|-----------------------------------|------------------------|
| 0  | exit     | 退出         | 1          | Exit the current process          | 退出当前进程           |
| 1  | putc     | 输出字符     | 1          | Output a character to the console | 输出一个字符到控制台   |
| 2  | fork     | 创建进程     | 0          | Create a new process              | 创建一个新进程         |
| 3  | print    | 打印字符串   | 1          | Print a string to the console     | 打印一个字符串到控制台 |
| 4  | getheap  | 获取堆地址   | 0          | Get the heap address              | 获取堆地址             |
| 5  | heapsize | 获取堆大小   | 0          | Get the heap size                 | 获取堆大小             |
| 6  | mmap     | 内存映射     | 2          | Memory mapping                    | 内存映射               |
| 7  | munmap   | 解除内存映射 | 2          | Unmap memory                      | 解除内存映射           |
| 8  | read     | 读取文件     | 3          | Read from a file                  | 从文件读取             |
| 9  | write    | 写入文件     | 3          | Write to a file                   | 写入文件               |

临时系统调用 (仅用于测试，可能随时删除)

| ID  | Name        | 名称         | Param-Nums | Description   | 描述         |
|-----|-------------|--------------|------------|---------------|--------------|
| 128 | vbe_setmode | 设置VBE模式  | 3          | Set VBE mode  | 设置VBE模式  |
| 129 | vbe_flip    | 翻转VBE      | 0          | Flip VBE      | 翻转VBE      |
| 130 | vbe_flush   | 刷新VBE      | 1          | Flush VBE     | 刷新VBE      |
| 131 | vbe_clear   | 清除VBE      | 3          | Clear VBE     | 清除VBE      |
| 132 | vbe_getmode | 获取VBE模式  | 3          | Get VBE mode  | 获取VBE模式  |
| 192 | file_size   | 获取文件大小 | 1          | Get file size | 获取文件大小 |
| 193 | load_file   | 加载文件     | 3          | Load file     | 加载文件     |
| 194 | save_file   | 保存文件     | 3          | Save file     | 保存文件     |
