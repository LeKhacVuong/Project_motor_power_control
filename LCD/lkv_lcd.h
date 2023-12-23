/*
 * lkv_lcd.h
 *
 * author: le khac vuong
 *
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32f1xx_hal.h"

#define CLEAR_DIS      1
#define NOT_CLEAR_DIS  0

void lkv_lcd_init(void);

int8_t lkv_lcd_process(uint8_t,uint8_t,uint8_t,char*);

char* lkv_lcd_int_to_string(uint32_t number);

char* lkv_lcd_float_to_string(double* value);



#endif /* INC_LCD_H_ */
