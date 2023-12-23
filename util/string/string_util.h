#include "stdint.h"
#include "string.h"
void string_swap(const char* p_buffer,char* p_msg,const uint8_t len);
int long_to_string(const uint32_t data,char* s);
int slong_to_string(const int32_t data,char* s);
void byte_to_hex_ascii(const uint8_t s,char* hex);
uint16_t slen(const char *str);
void string_join(char* dst,const char* str);
uint32_t string_to_long(char* str);
int float_to_string(float x,char* p);
