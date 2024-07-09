// Mutual exclusion lock. 一个自旋锁（spinlock），用于在修改进程状态时保证线程安全。
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};

