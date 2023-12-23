#include "string_util.h"
#include "stdint.h"
#include "stdlib.h"

static const uint8_t digit_char[10]={'0','1','2','3','4','5','6','7','8','9'};
static const uint8_t hex_digit[] ={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
static void string_reverse(char* str, int len);
void string_swap(const char* p_buffer,char* p_msg,const uint8_t len){
	uint8_t i=0;
	for(i=0;i<len;i++){
		p_msg[i]=p_buffer[len-i-1];
	}
}

int long_to_string(const uint32_t data,char* s){
	uint32_t remain=data;
	uint8_t digit=0;
	uint8_t index=0;
    uint8_t buffer[16]= {0};
	while(remain >=10){
		digit = remain%10;
		buffer[index]=digit_char[digit];
		index++;
		remain =(uint32_t)(remain/10);
	}
	buffer[index]=digit_char[remain];
        string_swap((char*)buffer,s,index+1);
	s[index+1]='\0';
	return index+1;
}

int slong_to_string(const int32_t data,char* s){
	int32_t temp;
	if(data >=0){
		return long_to_string(data,s);
	}

	temp=-data;
	*s='-';
	return (long_to_string(temp,s+1)+1);
}

void byte_to_hex_ascii(const uint8_t s,char* hex){
    uint8_t low_digit= s & 0x0f;
    uint8_t high_digit=(s & 0xf0)>>4;
    hex[0] = hex_digit[low_digit];
    hex[1]= hex_digit[high_digit];
}

uint16_t slen(const char *str) {

	const char *char_ptr;
	const uint32_t *longword_ptr;
	uint32_t longword, himagic, lomagic;

	/* Handle the first few characters by reading one character at a time.
	 Do this until CHAR_PTR is aligned on a longword boundary.  */
	for (char_ptr = str; ((unsigned long int) char_ptr & 3) != 0; ++char_ptr){
		if (*char_ptr == '\0'){
			return char_ptr - str;
		}
	}

	longword_ptr = (uint32_t*) char_ptr;

	/* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
	 the "holes."  Note that there is a hole just to the left of
	 each byte, with an extra at the end:

	 bits:  01111110 11111110 11111110 11111111
	 bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD

	 The 1-bits make sure that carries propagate to the next 0-bit.
	 The 0-bits provide holes for carries to fall into.  */
	himagic = 0x80808080L;
	lomagic = 0x01010101L;

	for (;;) {
		longword = *longword_ptr++;

		if (((longword - lomagic) & ~longword & himagic) != 0) {
			/* Which of the bytes was the zero?  If none of them were, it was
			 a misfire; continue the search.  */

			const char *cp = (const char*) (longword_ptr - 1);

			if (cp[0] == 0)
				return cp - str;
			if (cp[1] == 0)
				return cp - str + 1;
			if (cp[2] == 0)
				return cp - str + 2;
			if (cp[3] == 0)
				return cp - str + 3;
		}
	}
	return -1;
}

uint32_t string_to_long(char* str)
{
    uint32_t mult = 1;
    uint32_t re = 0;
    uint8_t len = slen(str);
    for(int i = len -1 ; i >= 0 ; i--)
    {
        re = re + ((uint8_t)str[i]- 48)*mult;
        mult = mult*10;
    }
    return re;
}

int float_to_string(const float data, char* s) {
	  int i = 0;
	  int num = (int)data;
	  float decimal = data - num;
	  int decimal_places = 2; 			// Số chữ số sau dấu thập phân
	  if (num == 0) {
	    s[i++] = '0';
	  } else {
	    while (num > 0) {
	      s[i++] = num % 10 + '0';
	      num /= 10;
	    }
	  }
	  string_reverse(s, i);
	  s[i++] = '.';
	  while (decimal_places-- > 0) {
	    decimal *= 10;
	    int digit = (int)decimal;
	    s[i++] = digit + '0';
	    decimal -= digit;
	  }
	  s[i] = '\0';
	  return i;
	}

static void string_reverse(char* str, int len) {
  int i = 0, j = len - 1;
  while (i < j) {
    char temp = str[i];
    str[i] = str[j];
    str[j] = temp;
    i++;
    j--;
  }
}
