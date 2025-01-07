
# 分页

## ia32

```c
usize mk_linear_addr3(usize table, usize page, [usize off])
```

| Position | Size | Description     | 描述       |
|----------|------|-----------------|------------|
| 0        | 1    | present         | 存在       |
| 1        | 1    | writeable       | 可写       |
| 2        | 1    | user-accessible | 用户可访问 |
| 3        | 1    | write-through   | 直写       |
| 4        | 1    | cache-disable   | 禁用缓存   |
| 5        | 1    | accessed        | 已访问     |
| 6        | 1    | dirty           | 脏页       |
| 7        | 1    | page-size       | 页大小     |
| 8        | 1    | global          | 全局       |
| 9-11     | 3    | available       | 可用       |
| 12-31    | 20   | address         | 地址       |

`9-11` 位可用于存储一些额外的信息，在 plos 中如下：

| Position | Description   | 描述     |
|----------|---------------|----------|
| 9        | user-writable | 用户可写 |
| 10       | shared        | 共享     |
| 11       | reserved      | 保留位   |

`user-writable` 的存在是因为用户空间中的页面会被设置为只读以便实现写时复制，复制完成后应该按照 `user-writable` 位的值来设置页面的写权限。

<!-- 同步 amd64 部分更新 -->

## amd64

```c
usize mk_linear_addr(usize pml4, usize pdpt, [usize pd, [usize pt, [usize off]]])
```

| Position | Size | Description     | 描述       |
|----------|------|-----------------|------------|
| 0        | 1    | present         | 存在       |
| 1        | 1    | writeable       | 可写       |
| 2        | 1    | user-accessible | 用户可访问 |
| 3        | 1    | write-through   | 直写       |
| 4        | 1    | cache-disable   | 禁用缓存   |
| 5        | 1    | accessed        | 已访问     |
| 6        | 1    | dirty           | 脏页       |
| 7        | 1    | page-size       | 页大小     |
| 8        | 1    | global          | 全局       |
| 9-11     | 3    | available       | 可用       |
| 12-51    | 40   | address         | 地址       |
| 52-62    | 11   | available       | 可用       |
| 63       | 1    | no-execute      | 不可执行   |

`9-11` `52-62` 位可用于存储一些额外的信息，在 plos 中如下：

| Position | Description   | 描述     |
|----------|---------------|----------|
| 9        | user-writable | 用户可写 |
| 10       | shared        | 共享     |
| 11       | reserved      | 保留位   |
| 52-62    | reserved      | 保留位   |

`user-writable` 的存在是因为用户空间中的页面会被设置为只读以便实现写时复制，复制完成后应该按照 `user-writable` 位的值来设置页面的写权限。

<!-- 同步 ia32 部分更新 -->

## 外部资料

- [OSDev Wiki](https://wiki.osdev.org/Paging)
- [Intel 64 and IA-32 Architectures Software Developer's Manual](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [AMD64 Architecture Programmer’s Manual, Volume 3](https://www.amd.com/content/dam/amd/en/documents/processor-tech-docs/programmer-references/24594.pdf)

*[wt]: write-through (直写)<br>写入时同时写入缓存和内存

*[write-through]: write-through (直写)<br>写入时同时写入缓存和内存

*[cd]: cache-disable (禁用缓存)<br>不使用缓存而直接从内存读取

*[cache-disable]: cache-disable (禁用缓存)<br>不使用缓存而直接从内存读取

*[ps]: page-size (页大小)<br>在某一级别使用大页

*[page-size]: page-size (页大小)<br>在某一级别使用大页
