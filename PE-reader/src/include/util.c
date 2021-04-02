#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

_Noreturn void err( const char* msg, ... ) {
  va_list args;
  va_start (args, msg);
  vfprintf(stderr, msg, args);
  va_end (args);
  exit(1);
}
