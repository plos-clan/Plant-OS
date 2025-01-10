
# 应用程序返回值

应用程序返回值是一个 32 位整数（即使在 64 位系统上也是如此）。<br>
The application return value is a 32-bit integer (even on 64-bit systems).

应用程序返回值的高 16 位不应当被应用程序设置，而只应被操作系统或系统库设置。<br>
The high 16 bits of the application return value should not be set by the application, but only by the operating system or system library.

应用程序退出码应当是一个 8 位无符号整数，如果需要也可以是一个 16 位无符号整数。<br>
The application exit code should be an 8-bit unsigned integer, and can be a 16-bit unsigned integer if necessary.

- `0-7`: 应用程序退出码 / Application exit code.
- `8-15`: 可用 / Available.
- `16-23`: 错误码 / Error code.
- `24-28`: 保留 / Reserved.
- `29`: 链接错误 / Link error.
- `30`: 非应用程序主动退出 / Non-application initiated exit.
- `31`: 保留 / Reserved.

第 31 位永远不会被使用，第 24-28 位可能在未来被使用。<br>
The 31st bit will never be used, and the 16th-28th bits may be used in the future.

第 30 位标记为非应用程序主动退出，可能是因为启动时链接错误、未处理信号、因某些原因被杀死等。<br>
The 30th bit is marked as a non-application initiated exit, which may be due to a link error at startup, unhandled signals, being killed for some reason, etc.

第 29 位标记为链接错误，可能是因为未找到依赖库、文件数据错误等。<br>
The 29th bit is marked as a link error, which may be due to missing dependent libraries, file data errors, etc.

第 16-23 位是错误码，用于标记应用程序的错误类型。<br>
The 16th-23rd bits are error codes used to mark the error type of the application.

非应用程序主动退出时低 16 位将被填充为 0xffff。<br>
When a non-application initiated exit occurs, the low 16 bits will be filled with 0xffff.

应用程序退出码的值应当在 0 到 255 之间，其中 0 表示成功，非 0 表示失败。<br>
The value of the application exit code should be between 0 and 255, where 0 indicates success and non-zero indicates failure.

## 链接错误

- `0` 未知错误 / Unknown error.
- `1` 未定义入口点 / Entry point undefined.
- `2` 无法找到文件 / Unable to find file.
- `3` 无法读取文件 / Unable to read file.
- `4` 无法解析文件 / Unable to parse file.
- `5` 无法找到依赖库 / Unable to find dependent library.
- `6` 无法读取依赖库 / Unable to read dependent library.
- `7` 无法解析依赖库 / Unable to parse dependent library.
- `8` 内存不足 / Out of memory.
- `9` 架构不匹配 / Architecture mismatch.
- `255` 传入参数错误 / Invalid input parameters.
