// Saved registers for kernel context switches.
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null. 这个指针用于指向当前在这个CPU上运行的进程。如果没有进程在该CPU上运行，则此指针为 null。
  struct context context;     // swtch() here to enter scheduler(). 这个结构体用于存储CPU的上下文信息，使得操作系统可以在进程或线程之间进行上下文切换。swtch() 函数会使用这个上下文信息来进入调度器，从而选择下一个要运行的进程或线程。
  int noff;                   // Depth of push_off() nesting. 这一行定义了一个整型变量 noff，用于记录 push_off() 函数调用的嵌套深度。push_off() 是一个通常用于禁用中断的函数
  int intena;                 // Were interrupts enabled before push_off()? 这一行定义了一个整型变量 intena，用于记录在调用 push_off() 函数之前，中断是否被启用。这个变量用于在调用 pop_off() 函数时恢复中断状态。
};

extern struct cpu cpus[NCPU];

// per-process data for the trap handling code in trampoline.S.
// sits in a page by itself just under the trampoline page in the
// user page table. not specially mapped in the kernel page table.
// the sscratch register points here.
// uservec in trampoline.S saves user registers in the trapframe,
// then initializes registers from the trapframe's
// kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// usertrapret() and userret in trampoline.S set up
// the trapframe's kernel_*, restore user registers from the
// trapframe, switch to the user page table, and enter user space.
// the trapframe includes callee-saved user registers like s0-s11 because the
// return-to-user path via usertrapret() doesn't return through
// the entire kernel call stack.
struct trapframe {
  /*   0 */ uint64 kernel_satp;   // kernel page table
  /*   8 */ uint64 kernel_sp;     // top of process's kernel stack
  /*  16 */ uint64 kernel_trap;   // usertrap()
  /*  24 */ uint64 epc;           // saved user program counter
  /*  32 */ uint64 kernel_hartid; // saved kernel tp
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
};

enum procstate { UNUSED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE }; // 已分配、就绪态、运行态、等待I/O中（阻塞态）、退出

// Per-process state 操作系统中的进程状态
struct proc {
  struct spinlock lock;  // 一个自旋锁（spinlock），用于在修改进程状态时保证线程安全。

  // p->lock must be held when using these:
  enum procstate state;        // Process state 进程的状态，是一个枚举类型 procstate，表示进程当前的运行状态，如运行、等待、睡眠等。
  struct proc *parent;         // Parent process 指向父进程的指针，表示当前进程的父进程。
  void *chan;                  // If non-zero, sleeping on chan 如果非零，表示进程正在此通道上睡眠。
  int killed;                  // If non-zero, have been killed 如果非零，表示进程已被杀死。
  int xstate;                  // Exit status to be returned to parent's wait 进程退出时返回给父进程的状态码。
  int pid;                     // Process ID 进程ID，是进程的唯一标识符。

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack 内核栈的虚拟地址。
  uint64 sz;                   // Size of process memory (bytes) 进程内存的大小（字节）。
  pagetable_t pagetable;       // User page table 用户页表，用于管理进程的虚拟内存。
  struct trapframe *trapframe; // data page for trampoline.S 用于 trampoline.S 的数据页，存储进程的寄存器等状态信息，以便于上下文切换。
  struct context context;      // swtch() here to run process 上下文信息，用于 swtch() 函数在进程间切换时保存和恢复进程状态。
  struct file *ofile[NOFILE];  // Open files 打开的文件数组，每个进程可以打开多个文件，这里通过数组管理这些文件的指针。
  struct inode *cwd;           // Current directory 当前工作目录，是一个指向 inode 结构的指针，表示进程的工作目录。
  char name[16];               // Process name (debugging) 进程名，主要用于调试。
};
