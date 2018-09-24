#ifndef __GO_RTYPE_H_
#define __GO_RTYPE_H_

#include <inttypes.h>

enum {
	kindMask = (1 << 5) - 1,
};

enum Kind {
  kInvalid = 0,
	kBool,
	kInt,
	kInt8,
	kInt16,
	kInt32,
	kInt64,
	kUint,
	kUint8,
	kUint16,
	kUint32,
	kUint64,
	kUintptr,
	kFloat32,
	kFloat64,
	kComplex64,
	kComplex128,
	kArray,
	kChan,
	kFunc,
	kInterface, /* 20 */
	kMap,    /* 21 */
	kPtr,
	kSlice,  /* 23 */
	kString, /* 24 */
	kStruct, /* 25 */
	kUnsafePointer,
	kCount  // count
};

// doc: go/1.10/libexec/src/reflect/type.go
struct __go_rtype {
  uintptr_t /* uintptr */ size;
	uintptr_t /* uintptr */ ptrdata;    // number of bytes in the type that can contain pointers
	uint32_t /* uint32 */ hash;         // hash of type; avoids computation in hash tables
	uint8_t /* tflag */ tflag;          // extra type information flags
	uint8_t /* uint8 */ align;          // alignment of variable with this type
	uint8_t /* uint8 */ fieldAlign;     // alignment of struct field with this type
	uint8_t /* uint8 */ kind;           // enumeration for C
	void* /* *typeAlg */ alg;           // algorithm table
	uint8_t* /* *byte */ gcdata;        // garbage collection data
	int32_t /* nameOff */ str;          // string form
	int32_t /* typeOff */ ptrToThis;    // type for pointer to this type, may be zero
};

#endif  // __GO_RTYPE_H_
