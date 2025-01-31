#pragma once
#include <define.h>

typedef struct asio *asio_t;

asio_t asio_sync(asio_t asio); // 等待一个任务结束

asio_t asio_run(asio_t asio); // 运行一个任务

void asio_main(); // 开始运行 asio 任务
