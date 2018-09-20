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
