package rapidjson

// #cgo CFLAGS: -I. -I${SRCDIR}/rapidjson-wrapper
// #cgo LDFLAGS: -L${SRCDIR}/rapidjson-wrapper -lrapidJSON_api -lstdc++
/*
#include "lib/gotypes/hash.h"
#include "lib/gotypes/types.h"
*/
import "C"
import (
	"unsafe"

	forceexport "github.com/alangpierce/go-forceexport"
)

var memhash_ uintptr
var aeshash_ uintptr
var fastrand_ uintptr

func alginit() {
	var err error
	memhash_, err = forceexport.FindFuncWithName("runtime.memhash")
	if err != nil {
		// Handle errors if you care about name possibly being invalid.
		panic(err)
	}
	aeshash_, err = forceexport.FindFuncWithName("runtime.aeshash")
	if err != nil {
		// Handle errors if you care about name possibly being invalid.
		panic(err)
	}
	fastrand_, err = forceexport.FindFuncWithName("runtime.fastrand")
	if err != nil {
		// Handle errors if you care about name possibly being invalid.
		panic(err)
	}
	C.setHashFunc(unsafe.Pointer(memhash_),
		unsafe.Pointer(aeshash_),
		unsafe.Pointer(fastrand_))
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
