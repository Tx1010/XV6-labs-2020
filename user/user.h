struct stat;
struct rtcdate;

// system calls
int fork(void);   // fork() 的返回值在父进程中是子进程的 PID，在子进程中是 0
int exit(int) __attribute__((noreturn));
int wait(int*);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);   // 打开一个文件；第二个参数表示read/write；返回一个fd（文件描述符）
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);  // 将打开文件fd的信息放入stat结构体中
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...);
void printf(const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);    // 返回字符串的长度
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
