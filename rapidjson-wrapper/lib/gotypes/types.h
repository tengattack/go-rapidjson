#ifndef _GO_TYPES_H_
#define _GO_TYPES_H_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include <inttypes.h>

#include "rtype.h"
#include "slice.h"
#include "map.h"

extern struct __go_rtype *rtypes[kCount];

// doc: https://golang.org/doc/install/gccgo
struct __go_string {
  const unsigned char *__data;
  uintptr_t __length;
};

struct __go_interface {
  struct __go_rtype *typ;
  void *ptr;
};

// fixed array
struct __go_array {
	struct __go_rtype rtype; // `reflect:"array"`
	struct __go_rtype *elem; // array element type
	struct __go_rtype *slice; // slice type
	uintptr_t len;
};

void* getString();

void* getSlice();

void checkInterface(uintptr_t i, bool debug);

void* getInterface();
void* getMapInterface();

#endif  // _GO_TYPES_H_
