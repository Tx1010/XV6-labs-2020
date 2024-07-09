#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
// argc表示命令行参数的数量，argv是一个字符串数组，包含了命令行参数
int
main(int argc, char *argv[])
{
  int i;
  // argv[0]通常包含程序的名称
  for(i = 1; i < argc; i++){
    write(1, argv[i], strlen(argv[i]));
    if(i + 1 < argc){
      write(1, " ", 1);
    } else {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
