#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

void say_hello() { printf("hello\n"); }

int main() {
  while (true) {
    say_hello();
    sleep(1);
  }
  return 0;
}
