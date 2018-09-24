/******************************************************************************
 * Objective: Wrapper over functions needed in the API in order to be able to
 *            read and write stringified JSON
 * Author:    Kenneth Nierenhausen
 * Date:      July 11, 2014
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

/* Guard to make sure that this is used as C */
#ifdef __cplusplus
extern "C" {
#endif

/* Writer and StringBuffer for the JSON writing */
typedef void* tm_json_writer_t;
typedef void* tm_json_stringbuffer_t;

/* Contains pointers to Writer and String Buffer */
typedef struct tm_json_w_handler {
  tm_json_writer_t writer;
  tm_json_stringbuffer_t stringBuffer;
} tm_json_w_handler_t;

/* Reading prototypes */
int tm_json_parse(void *,size_t,void *);
int tm_json_parse_str(char *,void *);
void* tm_json_parse_go(void *,size_t,void *);

/* Writing prototypes */
tm_json_w_handler_t tm_json_write_create();
int tm_json_write_string (tm_json_w_handler_t, const char*);
int tm_json_write_boolean (tm_json_w_handler_t, int);
int tm_json_write_number (tm_json_w_handler_t, double);
int tm_json_write_null (tm_json_w_handler_t);
int tm_json_write_object_start (tm_json_w_handler_t);
int tm_json_write_object_end (tm_json_w_handler_t);
int tm_json_write_array_start (tm_json_w_handler_t);
int tm_json_write_array_end (tm_json_w_handler_t);
const char* tm_json_write_result (tm_json_w_handler_t);
int tm_json_write_destroy(tm_json_w_handler_t);

/* Guard to make sure that this is used as C */
#ifdef __cplusplus
}
#endif
