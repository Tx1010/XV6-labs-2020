#include <iostream>
#include <cstring>

void printSubstring(char* str) {
    // 指向字符串中某个字符的指针
    char* p = str + 5;
    // 打印从该字符开始的子字符串
    std::cout << p << std::endl;
}

int main() {
    char str[] = "Hello, World!";
    printSubstring(str);
    return 0;
}
