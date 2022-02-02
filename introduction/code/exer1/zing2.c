#include <stdio.h>
#include <unistd.h>

void zing2 () {
  fprintf("Goodbye %s", getlogin());
  return;
}
