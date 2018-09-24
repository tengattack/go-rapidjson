#ifndef _GO_HASH_H_
#define _GO_HASH_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void setHashFunc(void *memhash_, void *aeshash_, void *fastrand_);

extern uintptr_t aeshash(void* p0, uintptr_t h, uintptr_t s);

#ifdef __cplusplus
}
#endif

#endif  // _GO_HASH_H_
