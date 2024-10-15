
# 日志格式

## Plant-OS

```log
[类型] [文件] [函数:行号] 内容
[类型] [文件] [函数:行号] [cpu:编号] [thread:编号] 内容
```

`cpu` 和 `thread` 均为可选项。

## Plant-OS ext

`i32 id`

- 日志消息 `01`

  | 类型 |         |  |
  |------|---------|--|
  | str  | type    |  |
  | str  | file    |  |
  | str  | func    |  |
  | i32  | line    |  |
  | i32  | cpu     |  |
  | i32  | thread  |  |
  | str  | content |  |
