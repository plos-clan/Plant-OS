#pragma once

#pragma GCC system_header

#define mem_geti(addr)   ({ *(volatile ssize_t *)(addr); })
#define mem_geti8(addr)  ({ *(volatile i8 *)(addr); })
#define mem_geti16(addr) ({ *(volatile i16 *)(addr); })
#define mem_geti32(addr) ({ *(volatile i32 *)(addr); })
#define mem_geti64(addr) ({ *(volatile i64 *)(addr); })

#define mem_getu(addr)   ({ *(volatile size_t *)(addr); })
#define mem_getu8(addr)  ({ *(volatile u8 *)(addr); })
#define mem_getu16(addr) ({ *(volatile u16 *)(addr); })
#define mem_getu32(addr) ({ *(volatile u32 *)(addr); })
#define mem_getu64(addr) ({ *(volatile u64 *)(addr); })

#define mem_get(addr)   mem_getu(addr)
#define mem_get8(addr)  mem_getu8(addr)
#define mem_get16(addr) mem_getu16(addr)
#define mem_get32(addr) mem_getu32(addr)
#define mem_get64(addr) mem_getu64(addr)

#define mem_set(addr, val)   ({ *(volatile size_t *)(addr) = (size_t)(val); })
#define mem_set8(addr, val)  ({ *(volatile u8 *)(addr) = (u8)(val); })
#define mem_set16(addr, val) ({ *(volatile u16 *)(addr) = (u16)(val); })
#define mem_set32(addr, val) ({ *(volatile u32 *)(addr) = (u32)(val); })
#define mem_set64(addr, val) ({ *(volatile u64 *)(addr) = (u64)(val); })
