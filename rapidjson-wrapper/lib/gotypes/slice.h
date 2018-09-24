#ifndef __GO_SLICE_H_
#define __GO_SLICE_H_

#include <inttypes.h>

struct __go_slice {
  void *__values;
  uintptr_t __count;
  uintptr_t __capacity;
};

#endif  // __GO_SLICE_H_
