#include <kernel.h>

// pcnet.c
void into_32bitsRW();
void into_16bitsRW();
void reset_card();
void Activate();
void init_pcnet_card();
void PcnetSend(u8 *buffer, usize size);

static inthandler_f PCNET_IRQ;

#define MTU 1500

struct EthernetFrame_head {
  u8  dest_mac[6];
  u8  src_mac[6];
  u16 type;
} __PACKED__;
// 以太网帧--尾部
struct EthernetFrame_tail {
  u32 CRC; // 这里可以填写为0，网卡会自动计算
};

struct InitializationBlock {
  // 链接器所迫 只能这么写了
  u16      mode;
  u8       reserved1numSendBuffers;
  u8       reserved2numRecvBuffers;
  u8       mac0, mac1, mac2, mac3, mac4, mac5;
  u16      reserved3;
  uint64_t logicalAddress;
  u32      recvBufferDescAddress;
  u32      sendBufferDescAddress;
} __PACKED__;

struct BufferDescriptor {
  u32 address;
  u32 flags;
  u32 flags2;
  u32 avail;
} __PACKED__;

extern int PCI_ADDR_BASE;

#define CARD_VENDOR_ID 0x1022
#define CARD_DEVICE_ID 0x2000

//  为了使用该寄存器，必须将RAP设置为这些值
#define CSR0    0
#define CSR1    1
#define CSR2    2
#define CSR3    3
#define CSR4    4
#define BCR18   18
#define BCR20   20
// 16位I/O端口（或到I/O空间开始的偏移）。
#define APROM0  0x00
#define APROM1  0x01
#define APROM2  0x02
#define APROM3  0x03
#define APROM4  0x04
#define APROM5  0x05
// 16位读写模式下
#define RDP16   0x10
#define RAP16   0x12
#define RESET16 0x14
#define BDP16   0x16
// 32位读写模式下
#define RDP32   0x10
#define RAP32   0x14
#define RESET32 0x18
#define BDP32   0x1c

extern u32 gateway, submask, dns, ip, dhcp_ip;

struct InitializationBlock initBlock;
u8                         mac0, mac1, mac2, mac3, mac4, mac5;

static int                      io_base = 0;
static struct BufferDescriptor *sendBufferDesc;
static u8                       sendBufferDescMemory[2048 + 15];
static u8                       sendBuffers[8][2048 + 15];
static u8                       currentSendBuffer;
static struct BufferDescriptor *recvBufferDesc;
static u8                       recvBufferDescMemory[2048 + 15];
static u8                       recvBuffers[8][2048 + 15];
static u8                       currentRecvBuffer;

void into_32bitsRW() {
  // 切换到32位读写模式 DWIO（BCR18,bit7）=1
  // 此时还处于16位读写模式
  // 读取BCR18
  asm_out16(io_base + RAP16, BCR18);
  u16 tmp  = asm_in16(io_base + BDP16);
  tmp     |= 0x80; // DWIO（bit7）=1
  // 写入BCR18
  asm_out16(io_base + RAP16, BCR18);
  asm_out16(io_base + BDP16, tmp);
  // 此时就处于32位读写模式了
}
void into_16bitsRW() {
  // 切换到16位读写模式 与切换到32位读写模式相反
  asm_out32(io_base + RAP32, BCR18);
  u32 tmp  = asm_in32(io_base + BDP32);
  tmp     &= ~0x80;
  asm_out32(io_base + RAP32, BCR18);
  asm_out32(io_base + BDP32, tmp);
}
void reset_card() {
  // PCNET卡复位（约等于切换到16位读写模式
  asm_in16(io_base + RESET16);
  asm_out16(io_base + RESET16, 0x00);
  // 执行完后需等待（sleep(1)）
}
void Activate() {
  // 激活PCNET IRQ中断
  asm_out16(io_base + RAP16, CSR0);
  asm_out16(io_base + RDP16, 0x41);

  asm_out16(io_base + RAP16, CSR4);
  u32 temp = asm_in16(io_base + RDP16);
  asm_out16(io_base + RAP16, CSR4);
  asm_out16(io_base + RDP16, temp | 0xc00);

  asm_out16(io_base + RAP16, CSR0);
  asm_out16(io_base + RDP16, 0x42);
}
static void init_Card_all() {
  currentSendBuffer = 0;
  currentRecvBuffer = 0;

  // 获取MAC地址并保存
  mac0 = asm_in8(io_base + APROM0);
  mac1 = asm_in8(io_base + APROM1);
  mac2 = asm_in8(io_base + APROM2);
  mac3 = asm_in8(io_base + APROM3);
  mac4 = asm_in8(io_base + APROM4);
  mac5 = asm_in8(io_base + APROM5);
  // printk("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n", mac0, mac1, mac2,
  // mac3,
  //        mac4, mac5);
  // 这里约等于 into_16bitsRW();
  reset_card();
  sleep(1);

  asm_out16(io_base + RAP16, BCR20);
  asm_out16(io_base + BDP16, 0x102);
  asm_out16(io_base + RAP16, CSR0);
  asm_out16(io_base + RDP16, 0x0004); // 暂时停止所有传输（用于初始化PCNET网卡

  // initBlock传输初始化（CSR1=IB地址低16位，CSR2=IB地址高16位）
  // &
  // Send/Recv环形缓冲区的初始化
  initBlock.mode                    = 0;
  initBlock.reserved1numSendBuffers = (0 << 4) | 3; // 高4位是reserved1 低4位是numSendBuffers
  initBlock.reserved2numRecvBuffers = (0 << 4) | 3; // 高4位是reserved2 低4位是numRecvBuffers
  initBlock.mac0                    = mac0;
  initBlock.mac1                    = mac1;
  initBlock.mac2                    = mac2;
  initBlock.mac3                    = mac3;
  initBlock.mac4                    = mac4;
  initBlock.mac5                    = mac5;
  initBlock.reserved3               = 0;
  initBlock.logicalAddress          = 0;

  sendBufferDesc = (struct BufferDescriptor *)(((u32)&sendBufferDescMemory[0] + 15) & 0xfffffff0);
  initBlock.sendBufferDescAddress = (u32)sendBufferDesc;
  recvBufferDesc = (struct BufferDescriptor *)(((u32)&recvBufferDescMemory[0] + 15) & 0xfffffff0);
  initBlock.recvBufferDescAddress = (u32)recvBufferDesc;

  for (usize i = 0; i < 8; i++) {
    sendBufferDesc[i].address = (((u32)&sendBuffers[i] + 15) & 0xfffffff0);
    sendBufferDesc[i].flags   = 0xf7ff;
    sendBufferDesc[i].flags2  = 0;
    sendBufferDesc[i].avail   = 0;

    recvBufferDesc[i].address = (((u32)&recvBuffers[i] + 15) & 0xfffffff0);
    recvBufferDesc[i].flags   = 0xf7ff | 0x80000000;
    recvBufferDesc[i].flags2  = 0;
    recvBufferDesc[i].avail   = 0;
    memset((void *)recvBufferDesc[i].address, 0, 2048);
  }
  // CSR1,CSR2赋值（initBlock地址
  asm_out16(io_base + RAP16, CSR1);
  asm_out16(io_base + RDP16, (u16)&initBlock);
  asm_out16(io_base + RAP16, CSR2);
  asm_out16(io_base + RDP16, (u32)&initBlock >> 16);

  Activate();

  // while (gateway == 0xFFFFFFFF && submask == 0xFFFFFFFF && dns == 0xFFFFFFFF && ip == 0xFFFFFFFF) {
  //   initBlock.logicalAddress = ip;
  // }
}

byte bus, dev, func;
bool pcnet_find_card() {
  return pci_get_device(CARD_VENDOR_ID, CARD_DEVICE_ID, -1, &bus, &dev, &func) == 0;
}

void init_pcnet_card() {
  val irq = pci_get_drive_irq(bus, dev, func);
  inthandler_set(0x20 + irq, PCNET_IRQ);
  // 2,写COMMAND和STATUS寄存器
  u32 conf  = pci_read_command_status(bus, dev, func);
  conf     &= 0xffff0000; // 保留STATUS寄存器，清除COMMAND寄存器
  conf     |= 0x7;        // 设置第0~2位（允许PCNET网卡产生中断
  pci_write_command_status(bus, dev, func, conf);

  for (int i = 0; i < 6; i++) {
    val off       = 0x10 + i * 4;
    val bar_value = read_pci(bus, dev, func, off);
    if ((bar_value & 1) == 1) {
      io_base = bar_value & ~0xf;
      break;
    }
  }

  init_Card_all();
}

int  recv = 0;
void Recv() {
  recv = 1;
  //printk("\nPCNET RECV: ");
  for (; (recvBufferDesc[currentRecvBuffer].flags & 0x80000000) == 0;
       currentRecvBuffer = (currentRecvBuffer + 1) % 8) {
    if (!(recvBufferDesc[currentRecvBuffer].flags & 0x40000000) &&
        (recvBufferDesc[currentRecvBuffer].flags & 0x03000000) == 0x03000000) {
      u32 size = recvBufferDesc[currentRecvBuffer].flags & 0xfff;
      if (size > 128) size -= 4;

      u8 *buffer = (u8 *)(recvBufferDesc[currentRecvBuffer].address);
      for (int i = 0; i < (size > 128 ? 128 : size); i++) {
        //printk("%02x ", buffer[i]);
      }
      //printk("\n");
    }
    recv              = 0;
    currentRecvBuffer = 0;

    memset((void *)recvBufferDesc[currentRecvBuffer].address, 0, 2048);
    recvBufferDesc[currentRecvBuffer].flags2 = 0;
    recvBufferDesc[currentRecvBuffer].flags  = 0x8000f7ff;
  }
  currentRecvBuffer = 0;
}
void PcnetSend(u8 *buffer, usize size) {
  while (recv)
    ;
  int sendDesc      = currentSendBuffer;
  currentSendBuffer = (currentSendBuffer + 1) % 8;
  memset((void *)sendBufferDesc[currentSendBuffer].address, 0, 2048);
  kassert(size <= MTU + sizeof(struct EthernetFrame_head) + sizeof(struct EthernetFrame_tail));

  for (u8 *src = buffer + size - 1, *dst = (u8 *)(sendBufferDesc[sendDesc].address + size - 1);
       src >= buffer; src--, dst--)
    *dst = *src;

  sendBufferDesc[sendDesc].avail  = 0;
  sendBufferDesc[sendDesc].flags  = 0x8300f000 | ((u16)((-size) & 0xfff));
  sendBufferDesc[sendDesc].flags2 = 0;

  asm_out16(io_base + RAP16, CSR0);
  asm_out16(io_base + RDP16, 0x48);

  currentSendBuffer = 0;
}

FASTCALL void PCNET_IRQ(i32 id, regs32 *regs) {
  asm_out16(io_base + RAP16, CSR0);
  u16 temp = asm_in16(io_base + RDP16);

  // if ((temp & 0x8000) == 0x8000)
  //   printk("PCNET ERROR\n");
  // else if ((temp & 0x2000) == 0x2000)
  //   printk("PCNET COLLISION ERROR\n");
  // else if ((temp & 0x1000) == 0x1000)
  //   printk("PCNET MISSED FRAME\n");
  // else if ((temp & 0x0800) == 0x0800)
  //   printk("PCNET MEMORY ERROR\n");
  /*else*/
  if ((temp & 0x0400) == 0x0400) Recv();
  // else if ((temp & 0x0200) == 0x0200)
  //   printk("PCNET SEND\n");

  asm_out16(io_base + RAP16, CSR0);
  asm_out16(io_base + RDP16, temp); // 通知PCNET网卡 中断处理完毕

  if ((temp & 0x0100) == 0x0100) klogd("PCNET INIT DONE\n");
}
