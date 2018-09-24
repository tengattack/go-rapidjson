
#include <stdio.h>
#include <inttypes.h>

#include "hash.h"

typedef uint32_t (*fastrandFn)();
typedef struct __go_call {
	void* p0;
	uintptr_t p1;
	uintptr_t p2;
	uintptr_t r0;
} __attribute__((__packed__)) go_call_t;
typedef uintptr_t (*aeshashFn)(go_call_t *args, int, unsigned long);

aeshashFn aeshashfn = NULL;

void setHashFunc(void *memhash_, void *aeshash_, void *fastrand_) {
  fastrandFn fn = (fastrandFn)fastrand_;
  aeshashfn = aeshash_;
}

uintptr_t aeshash(void* p, uintptr_t h, uintptr_t s) {
  go_call_t args;
  args.p0 = p;
  args.p1 = h;
  args.p2 = s;
  aeshashfn(&args, sizeof(args), 0);
  return args.r0;
}
