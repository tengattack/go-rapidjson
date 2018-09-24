#ifndef __GO_STRUCT_H_
#define __GO_STRUCT_H_

#include <inttypes.h>
#include "rtype.h"
#include "slice.h"

struct __go_uncommon_type {
  int32_t /* nameOff */ pkgPath; // import path; empty for built-in types like int, string
	uint16_t mcount;  // number of methods
	uint16_t _unused1;
	uint32_t moff;  // offset from this uncommontype to [mcount]method
	uint32_t _unused2;
};

struct __go_uncommon_struct {
  struct __go_rtype rtype;
  struct __go_uncommon_type u;
};

struct __go_name {
  uint8_t *bytes;
};

struct __go_struct {
  struct __go_rtype rtype;
  struct __go_name pkgPath;
  struct __go_slice fields;  // sorted by offset
};

#endif
