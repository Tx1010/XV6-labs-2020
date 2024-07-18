#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

void main();
void timerinit();

// entry.S needs one stack per CPU.
__attribute__ ((aligned (16))) char stack0[4096 * NCPU];

// scratch area for timer interrupt, one per CPU.
uint64 mscratch0[NCPU * 32];

// assembly code in kernelvec.S for machine-mode timer interrupt.
extern void timervec();

// entry.S jumps here in machine mode on stack0.  从机器模式到监督者模式
void
start()   
{
  // set M Previous Privilege mode to Supervisor, for mret.
  unsigned long x = r_mstatus();   // 读取mstatus寄存器的当前值。mstatus寄存器包含了多个状态位，用于控制和反映当前CPU的各种状态。
  x &= ~MSTATUS_MPP_MASK;    // 清除MPP位
  x |= MSTATUS_MPP_S;        // 设置MPP位为监督者模式
  w_mstatus(x);              // 写回mstatus寄存器

  // set M Exception Program Counter to main, for mret.
  // requires gcc -mcmodel=medany
  w_mepc((uint64)main);   ///将mepc（Machine Exception Program Counter）寄存器设置为main函数的地址。这样，当执行mret指令时，CPU会跳转到main函数执行。

  // disable paging for now. 禁用分页机制
  w_satp(0);

  // delegate all interrupts and exceptions to supervisor mode.当中断或异常发生时，将由Supervisor模式而不是Machine模式来处理
  w_medeleg(0xffff);
  w_mideleg(0xffff);
  w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

  // ask for clock interrupts.
  timerinit();

  // keep each CPU's hartid in its tp register, for cpuid().
  int id = r_mhartid(); // 通过读取mhartid（Machine Hardware Thread ID）寄存器并将其值写入tp（Thread Pointer）寄存器，为每个CPU核心保存其唯一的硬件线程ID
  w_tp(id);

  // switch to supervisor mode and jump to main().
  asm volatile("mret");
}

// set up to receive timer interrupts in machine mode,
// which arrive at timervec in kernelvec.S,
// which turns them into software interrupts for
// devintr() in trap.c.
void
timerinit()
{
  // each CPU has a separate source of timer interrupts.
  int id = r_mhartid();

  // ask the CLINT for a timer interrupt.
  int interval = 1000000; // cycles; about 1/10th second in qemu.
  *(uint64*)CLINT_MTIMECMP(id) = *(uint64*)CLINT_MTIME + interval;

  // prepare information in scratch[] for timervec.
  // scratch[0..3] : space for timervec to save registers.
  // scratch[4] : address of CLINT MTIMECMP register.
  // scratch[5] : desired interval (in cycles) between timer interrupts.
  uint64 *scratch = &mscratch0[32 * id];
  scratch[4] = CLINT_MTIMECMP(id);
  scratch[5] = interval;
  w_mscratch((uint64)scratch);

  // set the machine-mode trap handler.
  w_mtvec((uint64)timervec);

  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
}
