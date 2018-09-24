
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

#include "hash.h"
#include "types.h"

struct __go_rtype *rtypes[kCount] = {0};

void* getString() {
	struct __go_string *s = (struct __go_string *)malloc(sizeof(struct __go_string) + 5);
	s->__data = (uint8_t *)s + sizeof(struct __go_string);
	s->__length = 5;
	for (int i = 0; i < s->__length; i++) {
		((uint8_t *)s->__data)[i] = 'a';
	}
	return s;
}

void* getSlice() {
	struct __go_slice *s = (struct __go_slice *)malloc(sizeof(struct __go_slice) + 8);
	s->__values = (uint8_t *)s + sizeof(struct __go_slice);
	s->__count = 1;
	s->__capacity = 1;
	((int64_t *)s->__values)[0] = 123;
	return s;
}

void checkInterface(uintptr_t i, bool debug) {
	struct __go_interface *inter = (struct __go_interface *)i;
	if (debug) {
  	printf("0x%lx\nsize=%lu\nptrdata=%lu\nhash=%08x\ntflag=%x\n"
  		"align=%x\nfieldAlign=%x\nkind=%d\n"
  		"alg=%p\ngcdata=%p\nstr=%d\nptrToThis=%d\ntyp=%p\nptr=%p\n",
  		i,
  		inter->typ->size,
  		inter->typ->ptrdata,
  		inter->typ->hash,
  		inter->typ->tflag,
  		inter->typ->align,
  		inter->typ->fieldAlign,
  		inter->typ->kind & kindMask,
  		inter->typ->alg,
  		inter->typ->gcdata,
  		inter->typ->str,
  		inter->typ->ptrToThis,
  		inter->typ,
  		inter->ptr);
	}

	int kind = inter->typ->kind & kindMask;
	switch (kind) {
	case kMap: {
		struct __go_map_type* typ = (struct __go_map_type*)inter->typ;
		if (debug) {
			printf("key=%p keytype=%d\nelem=%p elemtype=%d\nbucket=%p buckettype=%d\nhmap=%p hmaptype=%d\n"
  			"keysize=%d\nindirectkey=%x\nvaluesize=%d\nindirectvalue=%x\n"
  			"bucketsize=%u\nreflexivekey=%d\nneedkeyupdate=%d\n",
				typ->key, typ->key->kind & kindMask,
				typ->elem, typ->elem->kind & kindMask,
				typ->bucket, typ->bucket->kind,
				typ->hmap, typ->hmap->kind & kindMask,
				typ->keysize,
				typ->indirectkey,
				typ->valuesize,
				typ->indirectvalue,
				typ->bucketsize,
				typ->reflexivekey,
				typ->needkeyupdate
			);
			struct __go_hmap* hmap = (struct __go_hmap*)(inter->ptr);
			printf("sizoeof(hmap)=%lu\n"
				"sizeof(count)=%lu off(count)=%lu\n"
				"off(flags)=%lu\n"
				"off(B)=%lu\n"
				"off(hash0)=%lu\n"
				"off(buckets)=%lu\n"
				"off(oldbuckets)=%lu\n",
				sizeof(struct __go_hmap),
				sizeof(hmap->count), (uintptr_t)&hmap->count - (uintptr_t)hmap,
				(uintptr_t)&hmap->flags - (uintptr_t)hmap,
				(uintptr_t)&hmap->B - (uintptr_t)hmap,
				(uintptr_t)&hmap->hash0 - (uintptr_t)hmap,
				(uintptr_t)&hmap->buckets - (uintptr_t)hmap,
				(uintptr_t)&hmap->oldbuckets - (uintptr_t)hmap
			);
			printf("hmap len=%lu flags=%d B=%d\nnoverflow=%d hash0=%08x\n"
				"buckets=%p oldbuckets=%p\n"
				"nevacuate=%lu extra=%p\n",
				hmap->count, hmap->flags, hmap->B,
				hmap->noverflow, hmap->hash0,
				hmap->buckets, hmap->oldbuckets,
				hmap->nevacuate, hmap->extra);

			if (hmap->buckets != NULL) {
				// TODO: hash
        struct __go_bmap_offset off;
        uintptr_t dataOffset = (uintptr_t)&off.v - (uintptr_t)&off.bmap;
        printf("dataOffset=%ld\n", dataOffset);

				uintptr_t hash = aeshash("0", hmap->hash0, 1);
        uintptr_t bucket = hash & (((uintptr_t)1<<hmap->B) - 1);
	      struct __go_bmap* b = (struct __go_bmap*)((uintptr_t)hmap->buckets + bucket*((uintptr_t)typ->bucketsize));
	      uint8_t top = (hash >> (sizeof(void *)*8 - 8)) & 0xff;
	      if (top < minTopHash) {
		      top += minTopHash;
	      }
				printf("size=%lu strhash=%lx bucket=%lu top=%x\n"
          "b.tophash=%llx\n",
          sizeof(uintptr_t),
          hash, bucket, top,
          *(uint64_t *)b->tophash);

				/* struct __go_hiter it = {0};
				it.t = typ;
				it.h = hmap;

				it.B = hmap->B;
				it.buckets = hmap->buckets;

				it.startBucket = 0;
				it.offset = 0;
				// iter state
				it.bucket = it.startBucket; */

				int count = 0;
				char str[20];
				for (uintptr_t j = 0; j < (uintptr_t)1 << (hmap->B); j++) {
					b = (struct __go_bmap*)((uintptr_t)hmap->buckets + j*((uintptr_t)typ->bucketsize));
					while (true) {
            for (size_t i = 0; i < bucketCnt; i++) {
              if (b->tophash[i] == empty || b->tophash[i] == evacuatedEmpty) {
  							continue;
  						}
              uintptr_t k = (uintptr_t)b + dataOffset + i * typ->keysize;
              struct __go_string *s = (struct __go_string*)k;
              uintptr_t hash1 = aeshash((void *)s->__data, hmap->hash0, s->__length);
  	          uint8_t top1 = (hash1 >> (sizeof(void *)*8 - 8)) & 0xff;
  	          if (top1 < minTopHash) {
  		          top1 += minTopHash;
  	          }

  						memcpy(str, s->__data, s->__length);
  						str[s->__length] = 0;

              printf("bucket=%ld b=%p k=%p tophash[%lu]=%02x hash=%02x key=%s\n",
  						 	j, b, s, i, b->tophash[i], top1, str);
  						count++;
            }
  					// overflow
  					struct __go_bmap **nb = (struct __go_bmap**)((uintptr_t)b + typ->bucketsize - sizeof(void *));
  					b = *nb;
  					// printf("b=%p, nb=%p\n", b, nb);
  					if (b == NULL) {
  						break;
  					}
					}
				}
				printf("count=%d\n", count);

				if (hmap->extra != NULL) {
					// not for iter
				}
			}
		}
		break;
	}
	}

	// save rtypes
	rtypes[kind] = inter->typ;
}

void* getInterface() {
	struct __go_interface *s = (struct __go_interface *)malloc(
		sizeof(struct __go_interface) + 50);
	//s->typ = (struct __go_rtype *)((uint8_t *)s + sizeof(struct __go_interface));
	//memcpy(s->typ, rtypes + kInt, sizeof(struct __go_interface));
	s->typ = rtypes[kString];
	struct __go_string *s0 = (struct __go_string *)((uint8_t *)s + sizeof(struct __go_interface));
	s0->__data = (uint8_t *)s0 + sizeof(struct __go_string);
	s0->__length = 3;
	memcpy((uint8_t *)s0->__data, "123", s0->__length);
	s->ptr = s0;
	return s;
}

void* getMapInterface() {
	struct __go_bmap_offset off;
  uintptr_t dataOffset = (uintptr_t)&off.v - (uintptr_t)&off.bmap;

	uintptr_t offset = 0;
	struct __go_interface *in = (struct __go_interface *)malloc(
		sizeof(struct __go_interface) + 10000);
	in->typ = rtypes[kMap];  // must be map[string]interface{}
	offset += sizeof(struct __go_interface);
	offset += in->typ->align - (offset % in->typ->align);

	struct __go_map_type *t = (struct __go_map_type*)in->typ;
	struct __go_hmap *h = (struct __go_hmap*)((uintptr_t)in + offset);
	offset += sizeof(struct __go_hmap);
	offset += in->typ->align - (offset % in->typ->align);

	memset(h, 0, sizeof(struct __go_hmap));
	in->ptr = h;

	h->count = 0;
	h->B = 0;  // 2^B = bucket count
	h->hash0 = 0;  // fastrand();
	h->buckets = (struct __go_hmap*)((uintptr_t)in + offset);
	offset += t->bucketsize;
	offset += in->typ->align - (offset % in->typ->align);

	struct __go_bmap *b = (struct __go_bmap*)(h->buckets);  // 0
	memset(b, 0, t->bucketsize);  // empty bucket

	uintptr_t bucket = 0;
  b = (struct __go_bmap*)((uintptr_t)h->buckets + bucket*((uintptr_t)t->bucketsize));

	char str[] = "12345";
	for (int i = 0; i < 100; i++) {
		sprintf(str, "%d", i);
  	uintptr_t hash = aeshash((void *)str, h->hash0, strlen(str));

  	// uintptr_t bucket = hash & (((uintptr_t)1<<h->B) - 1);
  	uint8_t top = (hash >> (sizeof(void *)*8 - 8)) & 0xff;
  	if (top < minTopHash) {
  		top += minTopHash;
  	}
  	printf("bucket=%lu top=%02x\n", bucket, top);

		int offi = i & (bucketCnt - 1);
		if (offi == 0 && i != 0) {
			// new b
			struct __go_bmap *b2 = (struct __go_bmap*)((uintptr_t)in + offset);

			offset += t->bucketsize;
			offset += in->typ->align - (offset % in->typ->align);

			memset(b2, 0, t->bucketsize);  // empty bucket
			*(struct __go_bmap **)((uintptr_t)b + t->bucketsize - sizeof(void *)) = b2;
			b = b2;
		}
  	b->tophash[offi] = top;

  	// key
  	struct __go_string *k = (struct __go_string*)((uintptr_t)b + dataOffset + offi * t->keysize);

  	k->__length = strlen(str);
  	// string char
  	k->__data = (const unsigned char*)((uintptr_t)in + offset);
  	memcpy((char *)k->__data, str, k->__length);

  	offset += k->__length;
  	offset += in->typ->align - (offset % in->typ->align);

  	// value
  	struct __go_interface *v = (struct __go_interface *)((uintptr_t)b + dataOffset
  		+ bucketCnt * t->keysize + offi * t->valuesize);
  	offset += sizeof(struct __go_interface);
  	offset += in->typ->align - (offset % in->typ->align);

  	v->typ = rtypes[kInt];
  	v->ptr = (void *)((uintptr_t)in + offset);
  	*(int64_t *)v->ptr = (int64_t)i;
		h->count++;

		offset += sizeof(int64_t);
		offset += in->typ->align - (offset % in->typ->align);

		printf("%d k=%p v=%p\n", i, k, v);
	}

	printf("offset=%lu\n", offset);

	return in;
}
