#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];

  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  // 将第一个参数（掩码）转换为整数并传递给trace
  if (trace(atoi(argv[1])) < 0) {
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }
  // 将argv中的命令和参数（从第三个参数开始）复制到nargv数组中
  for(i = 2; i < argc && i < MAXARG; i++){
    nargv[i-2] = argv[i];
  }
  // 程序通过exec函数执行nargv中的命令。如果exec成功执行，当前程序将被替换为nargv[0]指定的程序，因此exec之后的exit(0)调用理论上不会被执行。如果exec调用失败，程序将通过exit(0)正常退出
  exec(nargv[0], nargv);
  exit(0);
}
