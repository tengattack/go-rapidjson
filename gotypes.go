package rapidjson

// #cgo CFLAGS: -I. -I${SRCDIR}/rapidjson-wrapper
// #cgo LDFLAGS: -L${SRCDIR}/rapidjson-wrapper -lrapidJSON_api -lstdc++
/*
#include "lib/gotypes/types.h"
*/
import "C"
import (
	"unsafe"

	forceexport "github.com/alangpierce/go-forceexport"
)

var memhash_ func(p unsafe.Pointer, seed, s uintptr) uintptr
var aeshash_ func(p unsafe.Pointer, seed, s uintptr) uintptr
var fastrand_ func() uint32

func alginit() {
	err := forceexport.GetFunc(&memhash_, "runtime.memhash")
	if err != nil {
		// Handle errors if you care about name possibly being invalid.
		panic(err)
	}
	err = forceexport.GetFunc(&aeshash_, "runtime.aeshash")
	if err != nil {
		// Handle errors if you care about name possibly being invalid.
		panic(err)
	}
	err = forceexport.GetFunc(&fastrand_, "runtime.fastrand")
	if err != nil {
		// Handle errors if you care about name possibly being invalid.
		panic(err)
	}
}

//export memhash
func memhash(p unsafe.Pointer, seed, s C.uintptr_t) C.uintptr_t {
	return C.uintptr_t(memhash_(p, uintptr(seed), uintptr(s)))
}

//export aeshash
func aeshash(p unsafe.Pointer, seed, s C.uintptr_t) C.uintptr_t {
	return C.uintptr_t(aeshash_(p, uintptr(seed), uintptr(s)))
}

//export fastrand
func fastrand() C.uint32_t {
	return C.uint32_t(fastrand_())
}

func init() {
	alginit()
	// register types
	var u C.uintptr_t
	ins := []interface{}{true, int(1), int8(1), int16(1), int32(1), int64(1),
		uint(1), uint8(1), uint16(1), uint32(1), uint64(1), uintptr(1),
		float32(1), float64(1), complex64(1), complex128(1),
		"1", []interface{}{1}, map[string]interface{}{"1": 1}}
	for _, i := range ins {
		u = C.uintptr_t(uintptr(unsafe.Pointer(&i)))
		C.checkInterface(u, false)
	}
}
