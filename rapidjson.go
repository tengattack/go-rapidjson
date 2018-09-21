package rapidjson

// #cgo CFLAGS: -I${SRCDIR}/rapidjson-wrapper
// #cgo LDFLAGS: -L${SRCDIR}/rapidjson-wrapper -lrapidJSON_api -lstdc++
// #include "rapidJSON_api.h"
import "C"
import (
	"encoding/binary"
	"errors"
	"math"
	"sync"
	"unsafe"
)

type valNode struct {
	typ int
	val interface{}
}

type bufPool struct {
	pool *sync.Pool
}

const bufSize = 4096

// ErrBad invalid JSON data
var ErrBad = errors.New("Invalid JSON")

var bufs = bufPool{pool: &sync.Pool{
	New: func() interface{} {
		return make([]byte, bufSize)
	},
}}

func (p *bufPool) Get(len int) ([]byte, bool) {
	// we assume the worst case: [0,0,0,0,0,0,0,0,0]
	// buffer memory ratio: (2+5n)/(1+2n) > 2.5
	// so, 4096 / 2.5 = 1638.4
	if len > 1638 {
		return make([]byte, len*3), false
	}
	return p.pool.Get().([]byte), true
}

func (p *bufPool) Put(b []byte) {
	p.pool.Put(b)
}

// UnmarshalString unmarshals string to JSON value
func UnmarshalString(data string, v interface{}) error {
	buf, ok := bufs.Get(len(data))
	if ok {
		defer bufs.Put(buf)
	}
	s := C.CString(data)
	ret := int(C.tm_json_parse_str(s, unsafe.Pointer(&buf[0])))
	C.free(unsafe.Pointer(s))

	if ret == 0 {
		return ErrBad
	}
	return convertValue(buf, ret, v)
}

// Unmarshal unmarshals data to JSON value
func Unmarshal(data []byte, v interface{}) error {
	buf, ok := bufs.Get(len(data))
	if ok {
		defer bufs.Put(buf)
	}
	ret := int(C.tm_json_parse(unsafe.Pointer(&data[0]), C.size_t((len(data))), unsafe.Pointer(&buf[0])))

	if ret == 0 {
		return ErrBad
	}
	return convertValue(buf, ret, v)
}

func convertValue(buf []byte, ret int, v interface{}) error {
	var (
		len   int
		depth int
		key   string
		pv    interface{}
	)
	nesting := []valNode{
		valNode{typ: 0, val: v},
	}
	for i := 0; i < ret; i++ {
		switch buf[i] {
		case 0:
			// null
			pv = nil
		case 1:
			// true
			pv = true
		case 2:
			// false
			pv = false
		case 3:
			// int LE
			if buf[i+4] > 0x7f {
				pv = -((^(int(buf[i+1]) | int(buf[i+2])<<8 | int(buf[i+3])<<16 | int(buf[i+4]&0x7f)<<24) + 1) & 0x7fffffff)
			} else {
				pv = int(buf[i+1]) | int(buf[i+2])<<8 | int(buf[i+3])<<16 | int(buf[i+4])<<24
			}
			i += 4
		case 4:
			// uint LE
			pv = uint(buf[i+1]) | uint(buf[i+2])<<8 | uint(buf[i+3])<<16 | uint(buf[i+4])<<24
			i += 4
		case 5:
			// int64 LE
			// it just works?
			pv = int64(buf[i+1]) | int64(buf[i+2])<<8 | int64(buf[i+3])<<16 | int64(buf[i+4])<<24 |
				int64(buf[i+5])<<32 | int64(buf[i+6])<<40 | int64(buf[i+7])<<48 | int64(buf[i+8])<<56
			i += 8
		case 6:
			// uint64 LE
			pv = uint64(buf[i+1]) | uint64(buf[i+2])<<8 | uint64(buf[i+3])<<16 | uint64(buf[i+4])<<24 |
				uint64(buf[i+5])<<32 | uint64(buf[i+6])<<40 | uint64(buf[i+7])<<48 | uint64(buf[i+8])<<56
			i += 8
		case 7:
			// double
			pv = math.Float64frombits(binary.LittleEndian.Uint64(buf[i+1 : i+9]))
			i += 8
		case 8:
			// string
			len = int(buf[i+1]) | int(buf[i+2])<<8 | int(buf[i+3])<<16 | int(buf[i+4])<<24
			pv = string(buf[i+5 : i+5+len])
			i += 4 + len
		case 9:
			// {
			nesting = append(nesting, valNode{
				typ: 1, // object
				val: &map[string]interface{}{},
			})
			depth++
			continue
		case 10:
			// object key
			len = int(buf[i+1]) | int(buf[i+2])<<8 | int(buf[i+3])<<16 | int(buf[i+4])<<24
			key = string(buf[i+5 : i+5+len])
			i += 4 + len
			continue
		case 11:
			// }
			pv = (*(nesting[depth].val.(*map[string]interface{})))
			nesting = nesting[:depth]
			depth--
		case 12:
			// [
			nesting = append(nesting, valNode{
				typ: 2, // array
				val: &[]interface{}{},
			})
			depth++
			continue
		case 13:
			// ]
			pv = (*(nesting[depth].val.(*[]interface{})))
			nesting = nesting[:depth]
			depth--
		}
		if nesting[depth].typ == 1 {
			// object
			(*(nesting[depth].val.(*map[string]interface{})))[key] = pv
			key = ""
		} else if nesting[depth].typ == 2 {
			// array
			(*(nesting[depth].val.(*[]interface{}))) = append((*(nesting[depth].val.(*[]interface{}))), pv)
		} else {
			// here must be root (depth=0)
			// check type
			switch v.(type) {
			case *interface{}:
				(*(v.(*interface{}))) = pv
			case *map[string]interface{}:
				(*(v.(*map[string]interface{}))), _ = pv.(map[string]interface{})
			case *[]interface{}:
				(*(v.(*[]interface{}))), _ = pv.([]interface{})
			default:
				// TODO: add type
			}
			return nil
		}
	}
	return nil
}
