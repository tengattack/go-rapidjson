#ifndef __GO_MAP_H_
#define __GO_MAP_H_

#include <stdbool.h>
#include <inttypes.h>

#include "rtype.h"
#include "slice.h"

enum {
  bucketCntBits = 3,
  bucketCnt     = 1 << bucketCntBits,

  empty          = 0, // cell is empty
	evacuatedEmpty = 1, // cell is empty, bucket is evacuated.
	evacuatedX     = 2, // key/value is valid.  Entry has been evacuated to first half of larger table.
	evacuatedY     = 3, // same as above, but evacuated to second half of larger table.
	minTopHash     = 4, // minimum tophash for a normal filled cell.
};

// A header for a Go map.
struct __go_hmap {
	// Note: the format of the hmap is also encoded in cmd/compile/internal/gc/reflect.go.
	// Make sure this stays in sync with the compiler's definition.
	uintptr_t count; /* int */ // # live cells == size of map.  Must be first (used by len() builtin)
	uint8_t   flags;
	uint8_t   B;           // log_2 of # of buckets (can hold up to loadFactor * 2^B items)
	uint16_t  noverflow;   // approximate number of overflow buckets; see incrnoverflow for details
	uint32_t  hash0;       // hash seed

	void      *buckets;    // array of 2^B Buckets. may be nil if count==0.
	void      *oldbuckets; // previous bucket array of half the size, non-nil only when growing
	uintptr_t nevacuate;   // progress counter for evacuation (buckets less than this have been evacuated)

	struct __go_mapextra *extra;  // optional fields
};

// mapextra holds fields that are not present on all maps.
struct __go_mapextra {
	// If both key and value do not contain pointers and are inline, then we mark bucket
	// type as containing no pointers. This avoids scanning such maps.
	// However, bmap.overflow is a pointer. In order to keep overflow buckets
	// alive, we store pointers to all overflow buckets in hmap.extra.overflow and hmap.extra.oldoverflow.
	// overflow and oldoverflow are only used if key and value do not contain pointers.
	// overflow contains overflow buckets for hmap.buckets.
	// oldoverflow contains overflow buckets for hmap.oldbuckets.
	// The indirection allows to store a pointer to the slice in hiter.
	struct __go_slice **overflow;     /* *[]*bmap */
	struct __go_slice **oldoverflow;  /* *[]*bmap */

	// nextOverflow holds a pointer to a free overflow bucket.
	struct __go_bmap *nextOverflow;
};

// A bucket for a Go map.
struct __go_bmap {
	// tophash generally contains the top byte of the hash value
	// for each key in this bucket. If tophash[0] < minTopHash,
	// tophash[0] is a bucket evacuation state instead.
	uint8_t tophash[bucketCnt]; /* [bucketCnt]uint8 */
	// Followed by bucketCnt keys and then bucketCnt values.
	// NOTE: packing all the keys together and then all the values together makes the
	// code a bit more complicated than alternating key/value/key/value/... but it allows
	// us to eliminate padding which would be needed for, e.g., map[int64]int8.
	// Followed by an overflow pointer.
};

struct __go_map_type {
	struct __go_rtype    rtype;    // `reflect:"map"`
	struct __go_rtype*   key;      // map key type
	struct __go_rtype*   elem;     // map element (value) type
	struct __go_rtype*   bucket;   // internal bucket structure
	struct __go_rtype*   hmap;     // internal map header
	uint8_t       keysize;         // size of key slot
	uint8_t       indirectkey;     // store ptr to key instead of key itself
	uint8_t       valuesize;       // size of value slot
	uint8_t       indirectvalue;   // store ptr to value instead of value itself
	uint16_t      bucketsize;      // size of bucket
	bool          reflexivekey;    // true if k==k for all keys
	bool          needkeyupdate;   // true if we need to update key on an overwrite
};

struct __go_bmap_offset {
	struct __go_bmap bmap;
	int64_t v;
};

// A hash iteration structure.
// If you modify hiter, also change cmd/internal/gc/reflect.go to indicate
// the layout of this structure.
struct __go_hiter {
	void *key;       /* unsafe.Pointer */ // Must be in first position.  Write nil to indicate iteration end (see cmd/internal/gc/range.go).
	void *valued;    /* unsafe.Pointer */ // Must be in second position (see cmd/internal/gc/range.go).
	struct __go_map_type *t;         /* *maptype */
	struct __go_hmap *h;         /* *hmap */
	void *buckets;   /* unsafe.Pointer */ // bucket ptr at hash_iter initialization time
	struct __go_bmap *bptr;        /* *bmap */         // current bucket
	struct __go_slice *overflow;   /* *[]*bmap */      // keeps overflow buckets of hmap.buckets alive
	struct __go_slice *oldoverflow; /* *[]*bmap */     // keeps overflow buckets of hmap.oldbuckets alive
	uintptr_t startBucket;        // bucket iteration started at
	uint8_t offset;          // intra-bucket offset to start from during iteration (should be big enough to hold bucketCnt-1)
	bool wrapped;           // already wrapped around from end of bucket array to beginning
	uint8_t B;
	uint8_t i;
	uintptr_t   bucket;
	uintptr_t   checkBucket;
};

#endif  // __GO_MAP_H_
