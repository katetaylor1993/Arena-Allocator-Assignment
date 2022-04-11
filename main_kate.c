#include "mavalloc.h"
#include <stdio.h>

int main()
{
  mavalloc_init(6000, FIRST_FIT);
  void* first = mavalloc_alloc(1024);
  void* second = mavalloc_alloc(256);
  void* third = mavalloc_alloc(256);

  mavalloc_free(third);
}
