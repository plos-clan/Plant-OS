#pragma once
#include <libc-base.h>

extern bool acpi_inited;

void init_acpi();

void gettime_ns(timespec *ptr);
