# mm

Plant OS内存管理的实现

## Warnings

## Using

## Testing

## TODO

## Principle

在32位模式下，线性地址的组成分为三个部分

```[graph]
digraph G {
    "dir" [
        label = "<f0>Directory | <f1>Table | <f2>Offset"
        shape = record
    ]
    "cr3" [
        label = "cr3"
        shape = record
    ]
    "first+" [
        label = "+"
    ]
    "second+" [
        label = "+"
    ]
    "third+" [
        label = "+"
    ]
    subgraph _ {
        "页目录" [
            label = "页目录||<f0>|"
            shape = record
        ]
        "页表" [
            label = "页表||<f0>|"
            shape = record
        ]
        "页" [
            label = "页||<f0>|"
            shape = record
        ]
        rank = same
    }
    "cr3" -> "first+"
    "dir":f0 -> "first+"
    "first+" -> "页目录":f0
    "页目录":f0 -> "second+"
    "dir":f1 -> "second+"
    "second+" -> "页表":f0
    "页表":f0 -> "third+"
    "dir":f2 -> "third+"
    "third+" -> "页":f0
    
}
```

依据此原理，我们可以维护页目录和页表来设置线性地址与物理地址的对应关系

通过设置页目录和页表的属性，例如，关闭某个页的写属性，那么在ring3下访问这个页，就会报PF（Page Fault），那么我们就可以处理这个PF，重新设置这个页的属性，这也是COW（Copy on write，写时复制)的原理。

## Dependencies

mem.c -- malloc free realloc( 依赖于 `libc/alloc`) kmalloc kree krealloc(依赖于`page.c`)的实现

## Contribute
