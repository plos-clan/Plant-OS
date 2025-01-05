#pragma once
#include <define.h>

#include <libc-base/asm.h>

#include <libc-base/string/alloc.h>
#include <libc-base/string/mem.h>
#include <libc-base/string/str.h>

void abort() __attr(noreturn);
