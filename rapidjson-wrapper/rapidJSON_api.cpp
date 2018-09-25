/******************************************************************************
 * Objective: Interface between the C functions needed in Lua to the C++
 *            functions that rapidJSON uses. This acts as the actual interface
 * Author:    Kenneth Nierenhausen
 * Date:      July 11, 2014
 *****************************************************************************/
#include <iostream>
#include <cinttypes>

#include "./rapidJSON_api.h"
#include "lib/rapidjson/include/rapidjson/reader.h"
#include "lib/rapidjson/include/rapidjson/writer.h"
#include "lib/rapidjson/include/rapidjson/rapidjson.h"
#include "lib/rapidjson/include/rapidjson/filereadstream.h"
#include "lib/rapidjson/include/rapidjson/stringbuffer.h"
#include "lib/rapidjson/include/rapidjson/document.h"

#include "lib/gotypes/hash.h"
#include "lib/gotypes/types.h"

/* Used to call functions in the rapidJSON namespace */
using namespace rapidjson;

/* A collection of function pointers needed by Reader in rapidJSON */
typedef struct tm_json_r_handler {
  int i;
  uint8_t *ctx;
  tm_json_r_handler(int i_, uint8_t *ctx_)
    : i(i_)
    , ctx(ctx_) {};
  bool Default(void);
  bool Null(void);
  bool Bool(bool);
  bool Int(int);
  bool Uint(unsigned);
  bool Int64(int64_t);
  bool Uint64(uint64_t);
  bool Double(double);
  bool RawNumber(const char*,size_t,bool);
  bool String(const char*,size_t,bool);
  bool StartObject();
  bool Key(const char*,size_t,bool);
  bool EndObject(size_t);
  bool StartArray(void);
  bool EndArray(size_t);
} tm_json_r_handler_t;

/* Converts string to input stream and feeds it to rapidJSON Parse function */
extern "C" int tm_json_parse_str(char* json_s, void *buf) {

	// allocate memory for the struct
	tm_json_r_handler_t rh(0, (uint8_t*)buf);

	// create an input stream from the stringified JSON input
	StringStream is(json_s);

	// create a defaults flags GenericReader object
	Reader reader;

	// call rapidJSON's Parser using the input stream and the given handler
	bool ret = reader.Parse(is,rh);

	if (ret) {
		// length
		return rh.i;
	}
	return 0;
}

/* Converts data to memory stream and feeds it to rapidJSON Parse function */
extern "C" int tm_json_parse(void* json_s, size_t len, void *buf) {

	// allocate memory for the struct
	tm_json_r_handler_t rh(0, (uint8_t*)buf);

	// create an input stream from the stringified JSON input
	MemoryStream is((char *)json_s, len);

	// create a defaults flags GenericReader object
	Reader reader;

	// call rapidJSON's Parser using the input stream and the given handler
	bool ret = reader.Parse(is,rh);

	if (ret) {
		// length
		return rh.i;
	}
	return 0;
}

/* Collection of callbacks that get invoked by rapidJSON when it parses */
bool tm_json_r_handler::Default(void) {
	return true;
}

bool tm_json_r_handler::Null(void) {
	this->ctx[this->i++] = 0;
	return true;
}

bool tm_json_r_handler::Bool(bool val) {
	this->ctx[this->i++] = val ? 1 : 2;
	return true;
}

bool tm_json_r_handler::Int(int val) {
	this->ctx[this->i] = 3;
	*(int*)(this->ctx + this->i + 1) = val;
	this->i += sizeof(int) + 1;
	return true;
}

bool tm_json_r_handler::Uint(unsigned val) {
	this->ctx[this->i] = 4;
	*(int*)(this->ctx + this->i + 1) = val;
	this->i += sizeof(unsigned) + 1;
	return true;
}

bool tm_json_r_handler::Int64(int64_t val) {
	this->ctx[this->i] = 5;
	*(int64_t*)(this->ctx + this->i + 1) = val;
	this->i += sizeof(int64_t) + 1;
	return true;
}

bool tm_json_r_handler::Uint64(uint64_t val) {
	this->ctx[this->i] = 6;
	*(uint64_t*)(this->ctx + this->i + 1) = val;
	this->i += sizeof(uint64_t) + 1;
	return true;
}

bool tm_json_r_handler::Double(double val) {
	this->ctx[this->i] = 7;
	*(double*)(this->ctx + this->i + 1) = val;
	this->i += sizeof(double) + 1;
	return true;
}

bool tm_json_r_handler::RawNumber(const char* val,size_t len,bool set) {
	// that should not running in here
	return true;
}

bool tm_json_r_handler::String(const char* val,size_t len,bool set) {
	this->ctx[this->i] = 8;
	*(int*)(this->ctx + this->i + 1) = int(len);
	memcpy(this->ctx + this->i + 1 + sizeof(int), val, len);
	this->i += len + sizeof(int) + 1;
	return true;
}

bool tm_json_r_handler::StartObject(void) {
	this->ctx[this->i++] = 9;
	return true;
}

bool tm_json_r_handler::Key(const char* val,size_t len,bool set) {
	this->ctx[this->i] = 10;
	*(int*)(this->ctx + this->i + 1) = int(len);
	memcpy(this->ctx + this->i + 1 + sizeof(int), val, len);
	this->i += len + sizeof(int) + 1;
	return true;
}

bool tm_json_r_handler::EndObject(size_t val) {
	this->ctx[this->i++] = 11;
	return true;
}

bool tm_json_r_handler::StartArray(void) {
	this->ctx[this->i++] = 12;
	return true;
}

bool tm_json_r_handler::EndArray(size_t val) {
	this->ctx[this->i++] = 13;
	return true;
}

/* A collection of function pointers needed by Reader in rapidJSON */
typedef struct json_stack {
	int t;
	struct __go_string *key;
	struct __go_interface *val;
} json_stack_t;

struct __go_bmap_offset off;
const uintptr_t dataOffset = (uintptr_t)&off.v - (uintptr_t)&off.bmap;

typedef struct tm_json_r_go_handler {
  size_t offset;
  uint8_t *buf;
	size_t buflen;
	size_t depth;
	json_stack_t stack[16];
  tm_json_r_go_handler(uint8_t *buf_, size_t buflen_)
    : offset(0)
    , buf(buf_)
		, buflen(buflen_)
		, depth(0) {
		struct __go_interface *p = (struct __go_interface*)(this->buf + this->offset);
		// p->typ = NULL;
		// p->ptr = NULL;
		// this->offset += sizeof(struct __go_interface);  // aligned

		this->stack[0] = json_stack_t{0, NULL, p};
		this->depth++;
	};
  bool Default(void);
  bool Null(void);
  bool Bool(bool);
  bool Int(int);
  bool Uint(unsigned);
  bool Int64(int64_t);
  bool Uint64(uint64_t);
  bool Double(double);
  bool RawNumber(const char*,size_t,bool);
  bool String(const char*,size_t,bool);
  bool StartObject();
  bool Key(const char*,size_t,bool);
  bool EndObject(size_t);
  bool StartArray(void);
  bool EndArray(size_t);
	bool TypeEnd(struct __go_interface *p);
} tm_json_r_go_handler_t;

/* Collection of callbacks that get invoked by rapidJSON when it parses */
bool tm_json_r_go_handler::Default(void) {
	return true;
}

bool tm_json_r_go_handler::Null(void) {
	// interface{} -> nil
	struct __go_interface *p = (struct __go_interface*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_interface);  // aligned
	if (this->offset < this->buflen) {
		p->typ = NULL;
		p->ptr = NULL;
	}
	return TypeEnd(p);
}

bool tm_json_r_go_handler::Bool(bool val) {
	struct __go_interface *p = (struct __go_interface*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_interface);
	if (this->offset < this->buflen) {
		p->typ = rtypes[kBool];
		p->ptr = (void *)(this->buf + this->offset);
	}
	this->offset += sizeof(int64_t);
	if (this->offset < this->buflen) {
		*(int64_t *)p->ptr = val ? true : false;
	}
	return TypeEnd(p);
}

bool tm_json_r_go_handler::Int(int val) {
	struct __go_interface *p = (struct __go_interface*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_interface);
	if (this->offset < this->buflen) {
		p->typ = rtypes[kInt];
		p->ptr = (void *)(this->buf + this->offset);
	}
	this->offset += sizeof(int64_t);
	if (this->offset < this->buflen) {
		*(int64_t *)p->ptr = val;
	}
	return TypeEnd(p);
}

bool tm_json_r_go_handler::Uint(unsigned val) {
	struct __go_interface *p = (struct __go_interface*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_interface);
	if (this->offset < this->buflen) {
		p->typ = rtypes[kUint];
		p->ptr = (void *)(this->buf + this->offset);
	}
	this->offset += sizeof(uint64_t);
	if (this->offset < this->buflen) {
		*(uint64_t *)p->ptr = val;
	}
	return TypeEnd(p);
}

bool tm_json_r_go_handler::Int64(int64_t val) {
	struct __go_interface *p = (struct __go_interface*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_interface);
	if (this->offset < this->buflen) {
		p->typ = rtypes[kInt64];
		p->ptr = (void *)(this->buf + this->offset);
	}
	this->offset += sizeof(int64_t);
	if (this->offset < this->buflen) {
		*(int64_t *)p->ptr = val;
	}
	return TypeEnd(p);
}

bool tm_json_r_go_handler::Uint64(uint64_t val) {
	struct __go_interface *p = (struct __go_interface*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_interface);
	if (this->offset < this->buflen) {
		p->typ = rtypes[kUint64];
		p->ptr = (void *)(this->buf + this->offset);
	}
	this->offset += sizeof(uint64_t);
	if (this->offset < this->buflen) {
		*(uint64_t *)p->ptr = val;
	}
	return TypeEnd(p);
}

bool tm_json_r_go_handler::Double(double val) {
	struct __go_interface *p = (struct __go_interface*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_interface);
	if (this->offset < this->buflen) {
		p->typ = rtypes[kFloat64];
		p->ptr = (void *)(this->buf + this->offset);
	}
	this->offset += sizeof(double);
	this->offset += 8 - (this->offset % 8);  // align
	if (this->offset < this->buflen) {
		*(double *)p->ptr = val;
	}
	return TypeEnd(p);
}

bool tm_json_r_go_handler::RawNumber(const char* val,size_t len,bool set) {
	// that should not running in here
	return true;
}

bool tm_json_r_go_handler::String(const char* val,size_t len,bool set) {
	struct __go_interface *p = (struct __go_interface*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_interface);
	if (this->offset < this->buflen) {
		p->typ = rtypes[kString];
		p->ptr = (void *)(this->buf + this->offset);
	}
	struct __go_string *s = (struct __go_string*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_string);
	if (this->offset < this->buflen) {
		s->__length = len;
		s->__data = (const unsigned char*)(this->buf + this->offset);
	}
	this->offset += len;
	this->offset += 8 - (this->offset % 8);  // align
	if (this->offset < this->buflen) {
		memcpy((char *)s->__data, val, len);
	}
	return TypeEnd(p);
}

bool tm_json_r_go_handler::StartObject(void) {
	struct __go_interface *p = (struct __go_interface*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_interface);

	struct __go_map_type *t = (struct __go_map_type*)rtypes[kMap];
	if (this->offset < this->buflen) {
		p->typ = (struct __go_rtype*)t;  // must be map[string]interface{}
		p->ptr = (void *)(this->buf + this->offset);
	}

	struct __go_hmap *h = (struct __go_hmap*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_hmap);
	this->offset += 8 - (this->offset % 8);  // align
	if (this->offset < this->buflen) {
		memset(h, 0, sizeof(struct __go_hmap));  // empty

		h->count = 0;
		h->B = 0;  // 2^B = bucket count
		h->hash0 = 0;  // fastrand();
		h->buckets = (struct __go_hmap*)(this->buf + this->offset);
	}
	this->offset += t->bucketsize;
	this->offset += 8 - (this->offset % 8);  // align

	if (this->offset < this->buflen) {
		struct __go_bmap *b = (struct __go_bmap*)(h->buckets);  // first
		memset(b, 0, t->bucketsize);  // empty bucket
	}

	this->stack[this->depth++] = json_stack_t{kMap, NULL, p};
	return true;
}

bool tm_json_r_go_handler::Key(const char* val,size_t len,bool set) {
	struct __go_string *s = (struct __go_string*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_string);
	if (this->offset < this->buflen) {
		s->__length = len;
		s->__data = (const unsigned char*)(this->buf + this->offset);
	}
	this->offset += len;
	this->offset += 8 - (this->offset % 8);  // align
	if (this->offset < this->buflen) {
		memcpy((char *)s->__data, val, len);
	}
	this->stack[this->depth - 1].key = s;
	return true;
}

bool tm_json_r_go_handler::EndObject(size_t val) {
	struct __go_interface *p = this->stack[--this->depth].val;
	return TypeEnd(p);
}

bool tm_json_r_go_handler::StartArray(void) {
	struct __go_interface *p = (struct __go_interface*)(this->buf + this->offset);
	this->offset += sizeof(struct __go_interface);
	if (this->offset < this->buflen) {
		p->typ = rtypes[kSlice];
		p->ptr = (void *)(this->buf + this->offset);
	}
	struct __go_slice *s = (struct __go_slice *)(this->buf + this->offset);
	this->offset += sizeof(struct __go_slice);
	uintptr_t cap = 10;
	if (this->offset < this->buflen) {
		s->__values = (void *)(this->buf + this->offset);
		s->__count = 0;
		s->__capacity = cap;
	}
	this->offset += sizeof(struct __go_interface) * cap;
	this->stack[this->depth++] = json_stack_t{kSlice, NULL, p};
	return true;
}

bool tm_json_r_go_handler::EndArray(size_t val) {
	struct __go_interface *p = this->stack[--this->depth].val;
	return TypeEnd(p);
}

bool tm_json_r_go_handler::TypeEnd(struct __go_interface *p) {
	json_stack_t *st = &this->stack[this->depth - 1];
	switch (st->t) {
	case kMap: {
		if ((uintptr_t)st->val - (uintptr_t)this->buf + sizeof(struct __go_interface) >= this->buflen) {
			break;
		}
		if ((uintptr_t)st->key - (uintptr_t)this->buf + sizeof(struct __go_string) >= this->buflen) {
			break;
		}

		struct __go_map_type *t = (struct __go_map_type*)st->val->typ;
		struct __go_hmap *h = (struct __go_hmap*)(st->val->ptr);

		if ((uintptr_t)h - (uintptr_t)this->buf + sizeof(struct __go_hmap) >= this->buflen) {
			break;
		}

		uintptr_t hash = aeshash((void *)st->key->__data, h->hash0, st->key->__length);
		uintptr_t bucket = hash & (((uintptr_t)1<<h->B) - 1);
  	uint8_t top = (hash >> (sizeof(void *)*8 - 8)) & 0xff;
  	if (top < minTopHash) {
  		top += minTopHash;
  	}

		struct __go_bmap *b = (struct __go_bmap*)((uintptr_t)h->buckets + bucket*((uintptr_t)t->bucketsize));
		int offi = h->count & (bucketCnt - 1);
		if (h->count > bucketCnt) {
			for (size_t i = 0; i < (h->count-1) / bucketCnt; i++) {
				b = *(struct __go_bmap **)((uintptr_t)b + t->bucketsize - sizeof(void *));
			}
		}
		if (offi == 0 && h->count != 0) {
			// new b
			struct __go_bmap *b2 = (struct __go_bmap*)(this->buf + this->offset);
			this->offset += t->bucketsize;
			this->offset += 8 - (offset % 8);
			if (this->offset < this->buflen) {
				memset(b2, 0, t->bucketsize);  // empty bucket
			}

			*(struct __go_bmap **)((uintptr_t)b + t->bucketsize - sizeof(void *)) = b2;
			b = b2;
		}
		if ((uintptr_t)b - (uintptr_t)this->buf + t->bucketsize < this->buflen) {
  		b->tophash[offi] = top;

			memcpy((struct __go_string*)((uintptr_t)b + dataOffset + offi * t->keysize),
				st->key, sizeof(struct __go_string));
			memcpy((struct __go_interface*)((uintptr_t)b + dataOffset + bucketCnt * t->keysize + offi * t->valuesize),
				p, sizeof(struct __go_interface));
		}

		h->count++;
		break;
	}
	case kSlice: {
		if ((uintptr_t)st->val - (uintptr_t)this->buf + sizeof(struct __go_interface) >= this->buflen) {
			break;
		}
		struct __go_slice *s = (struct __go_slice *)st->val->ptr;
		if ((uintptr_t)s - (uintptr_t)this->buf + sizeof(struct __go_slice) >= this->buflen) {
			break;
		}
		if ((uintptr_t)((struct __go_interface*)s->__values + s->__count) - (uintptr_t)this->buf + sizeof(struct __go_interface) < this->buflen) {
			memcpy(((struct __go_interface*)s->__values) + s->__count, p, sizeof(struct __go_interface));
		}
		s->__count++;
		break;
	}
	default: {
		// do not thing, since `st->val == p` must be true
	}
	}
	return true;
}

/* Converts data to memory stream and feeds it to rapidJSON Parse function */
extern "C" void* tm_json_parse_go(void* json_s, size_t len, void* buf, size_t buflen) {
	// allocate memory for the struct
	tm_json_r_go_handler_t rh((uint8_t*)buf, buflen);

	// create an input stream from the stringified JSON input
	MemoryStream is((char *)json_s, len);

	// create a defaults flags GenericReader object
	Reader reader;

	// call rapidJSON's Parser using the input stream and the given handler
	bool ret = reader.Parse(is,rh);
	if (ret) {
		// length: rh.offset
		return buf;
	}
	return NULL;
}

/* Creates a new Writer object and returns it to C as a void pointer */
extern "C" tm_json_w_handler_t tm_json_write_create() {

	// create the writer handler
	tm_json_w_handler_t wh;

	// allocate the writer and assugn in in the struct
	StringBuffer* sb = new StringBuffer();
	wh.stringBuffer = static_cast<tm_json_stringbuffer_t>(sb);

	// allocate the string buffer and assign it in the struct
	Writer<StringBuffer>* w = new Writer<StringBuffer>(*sb);
	wh.writer = static_cast<tm_json_writer_t>(w);

	// return the actual write handler containing void pointers
	return wh;
}

/* Writes out a String using rapidJSON functions */
extern "C" int tm_json_write_string(tm_json_w_handler_t wh, const char* value) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
	w->String(value);
	return 0;
}

/* Writes out a Bool using rapidJSON functions */
extern "C" int tm_json_write_boolean (tm_json_w_handler_t wh, int value) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
	w->Bool(value);
	return 0;
}

/* Writes out a Number using rapidJSON functions */
extern "C" int tm_json_write_number (tm_json_w_handler_t wh, double value) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
	w->Double(value);
	return 0;
}

/* Writes out Null using rapidJSON functions */
extern "C" int tm_json_write_null (tm_json_w_handler_t wh) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
	w->Null();
	return 0;
}

/* Writes out an object start using rapidJSON functions */
extern "C" int tm_json_write_object_start (tm_json_w_handler_t wh) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
	w->StartObject();
	return 0;
}

/* Writes out an object end using rapidJSON functions */
extern "C" int tm_json_write_object_end (tm_json_w_handler_t wh) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
	w->EndObject();
	return 0;
}

/* Writes out an array start using rapidJSON functions */
extern "C" int tm_json_write_array_start (tm_json_w_handler_t wh) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
	w->StartArray();
	return 0;
}

/* Writes out an array end using rapidJSON functions */
extern "C" int tm_json_write_array_end (tm_json_w_handler_t wh) {
	Writer<StringBuffer>* w = static_cast<Writer<StringBuffer>*>(wh.writer);
	w->EndArray();
	return 0;
}

/* returns Writer<StringBuffer>::getString() */
extern "C" const char* tm_json_write_result (tm_json_w_handler_t wh) {
	StringBuffer* sb = static_cast<StringBuffer*>(wh.stringBuffer);
	return sb->GetString();
}

/* frees the writer, string buffer, and the struct holding them all */
extern "C" int tm_json_write_destroy(tm_json_w_handler_t wh) {
	delete static_cast<Writer<StringBuffer>*>(wh.writer);
	delete static_cast<StringBuffer*>(wh.stringBuffer);
	return 0;
}
