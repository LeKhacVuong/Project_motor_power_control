/*
 * lkv_lcd.c
 *
 * author: le khac vuong
 *
 */

#include <lkv_lcd.h>

#define LCD_PIN_RS_PORT   GPIOA
#define LCD_PIN_RS        GPIO_PIN_0

#define LCD_PIN_RW_PORT   GPIOA
#define LCD_PIN_RW        GPIO_PIN_1

#define LCD_PIN_EN_PORT   GPIOA
#define LCD_PIN_EN        GPIO_PIN_2

#define LCD_PIN_D4_PORT   GPIOA
#define LCD_PIN_D4        GPIO_PIN_4

#define LCD_PIN_D5_PORT   GPIOA
#define LCD_PIN_D5        GPIO_PIN_5

#define LCD_PIN_D6_PORT   GPIOA
#define LCD_PIN_D6        GPIO_PIN_6

#define LCD_PIN_D7_PORT   GPIOA
#define LCD_PIN_D7        GPIO_PIN_7



#define LCD_CMD_FUNCTION_SET             0x28    /* 4bits datalen, 2 line, 5x8 format        */
#define LCD_CMD_DISPLY_ONOFF_CONTROL     0x0E    /* Display on, Cursor on, Blink off         */
#define LCD_CMD_DISPLAY_CLEAR            0x01    /* Clear Diplay                             */
#define LCD_CMD_RETURN_HOME              0x02    /* Return Home                              */
#define LCD_CMD_ENTRY_MODE_SET           0x06    /* Cursor move increment, Display not shift */


static char* IntToString1char(uint32_t number);
static char* IntToString2char(uint32_t number);
static char* IntToString3char(uint32_t number);
static char* IntToString4char(uint32_t number);
static char* IntToString5char(uint32_t number);
static char* FloatToStringX_XXX(double* value);
static char* FloatToStringXX_XX(double* value);
static char* FloatToStringXXX_X(double* value);
static void lkv_lcd_send_char(uint8_t Data);
static void lkv_lcd_send_cmd(uint8_t cmd);
static void lkv_lcd_sends_tring(char* msg);
static void lkv_lcd_display_clear(void);
static void lkv_lcd_set_cursor(uint8_t row,uint8_t column);
static void lkv_lcd_write_data(uint8_t data);
static void lkv_lcd_enable(void);
static void lkv_ldc_delay(uint32_t time);
void lkv_lcd_retur_nhome(void);


void lkv_lcd_init(void){

	/* enable GPIO LCD PORTs clock */

	 __HAL_RCC_GPIOA_CLK_ENABLE();

	/* enable GPIO LCD PINs */

	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	  GPIO_InitStruct.Pin = LCD_PIN_RS;
	  HAL_GPIO_Init(LCD_PIN_RS_PORT, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_PIN_RW;
	  HAL_GPIO_Init(LCD_PIN_RW_PORT, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_PIN_EN;
	  HAL_GPIO_Init(LCD_PIN_EN_PORT, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_PIN_D4;
	  HAL_GPIO_Init(LCD_PIN_D4_PORT, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_PIN_D5;
	  HAL_GPIO_Init(LCD_PIN_D5_PORT, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_PIN_D6;
	  HAL_GPIO_Init(LCD_PIN_D6_PORT, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_PIN_D7;
	  HAL_GPIO_Init(LCD_PIN_D7_PORT, &GPIO_InitStruct);




	/* LCD Init (datasheet) */
	/*step 1 wait more than 40 ms */
	HAL_Delay(60);

	/*step 2 write 0 0 0 0 1 1 to RS RW D7 D6 D5 D4 */
	HAL_GPIO_WritePin(LCD_PIN_RS_PORT, LCD_PIN_RS, 0);
	HAL_GPIO_WritePin(LCD_PIN_RW_PORT, LCD_PIN_RW, 0);
	lkv_lcd_write_data(0x3);

	/*step 3 wait more than 4.1 ms */
	HAL_Delay(6);

	/*step 5 write 0 0 0 0 1 1 to RS RW D7 D6 D5 D4 again */
	lkv_lcd_write_data(0x3);

	/*step 6 wait more than 100 us */
	lkv_ldc_delay(150);

	/*step 7 write 0 0 0 0 1 1 to RS RW D7 D6 D5 D4 again */
	lkv_lcd_write_data(0x3);

	/*step 8 write 0 0 0 0 1 0 to RS RW D7 D6 D5 D4  */
	lkv_lcd_write_data(0x2);

	/*step 9 Function set -> Display off -> Display clear (wait 2ms) -> Entry mode set  */
	lkv_lcd_send_cmd(LCD_CMD_FUNCTION_SET);
	lkv_lcd_send_cmd(LCD_CMD_DISPLY_ONOFF_CONTROL);
	lkv_lcd_display_clear();
	lkv_lcd_send_cmd(LCD_CMD_ENTRY_MODE_SET);
}

int8_t lkv_lcd_process(uint8_t _clear,uint8_t _row, uint8_t _colum,char* _msg){
	if((_row < 1) || (_colum < 1)){
		return -1;
	}

	if(_clear == CLEAR_DIS){
		lkv_lcd_display_clear();
	}

	lkv_lcd_set_cursor(_row,_colum);
	lkv_lcd_sends_tring(_msg);

	return 0;
}




void lkv_lcd_enable(void){
	HAL_GPIO_WritePin(LCD_PIN_EN_PORT, LCD_PIN_EN, 1);
	lkv_ldc_delay(10); /* thoi gian de ghi cau lenh */
	HAL_GPIO_WritePin(LCD_PIN_EN_PORT, LCD_PIN_EN, 0);
	lkv_ldc_delay(100); /* thoi gian de ghi cau lenh + 37 us thuc thi lenh*/
}


void lkv_lcd_write_data(uint8_t data){
	HAL_GPIO_WritePin(LCD_PIN_D4_PORT, LCD_PIN_D4, 1 & (data >> 0));
	HAL_GPIO_WritePin(LCD_PIN_D5_PORT, LCD_PIN_D5, 1 & (data >> 1));
	HAL_GPIO_WritePin(LCD_PIN_D6_PORT, LCD_PIN_D6, 1 & (data >> 2));
	HAL_GPIO_WritePin(LCD_PIN_D7_PORT, LCD_PIN_D7, 1 & (data >> 3));
    lkv_lcd_enable();
}



void lkv_lcd_send_cmd(uint8_t cmd){
	/* RS = 0 to chose command register */
	HAL_GPIO_WritePin(LCD_PIN_RS_PORT, LCD_PIN_RS, 0);
	/* RW = 0 to chose Write mode */
	HAL_GPIO_WritePin(LCD_PIN_RW_PORT, LCD_PIN_RW, 0);

	/* Send 4 high bits first */
	lkv_lcd_write_data(cmd >> 4);

	/* Send 4 high bits first */
	lkv_lcd_write_data(cmd & 0x0F);
}

void lkv_lcd_send_char(uint8_t Data){
	/* RS = 1 to chose Data register */
	HAL_GPIO_WritePin(LCD_PIN_RS_PORT, LCD_PIN_RS, 1);

	/* RW = 0 to chose Write mode */
	HAL_GPIO_WritePin(LCD_PIN_RW_PORT, LCD_PIN_RW, 0);

	/* Send 4 high bits first */
	lkv_lcd_write_data(Data >> 4);

	/* Send 4 high bits first */
	lkv_lcd_write_data(Data & 0x0F);

}


void lkv_lcd_sends_tring(char *msg){
	do{
		lkv_lcd_send_char((uint8_t)*msg);
		msg++;
	}
	while( *msg != '\0' );
}


void lkv_lcd_display_clear(void){
	lkv_lcd_send_cmd(LCD_CMD_DISPLAY_CLEAR);
	HAL_Delay(2);
}

void lkv_lcd_retur_nhome(void){
	lkv_lcd_write_data(LCD_CMD_RETURN_HOME);
	HAL_Delay(2);
}

void lkv_lcd_set_cursor(uint8_t row, uint8_t column){
	column--;
	if(row == 1){
		lkv_lcd_send_cmd( (column |= 0x80) );
	}else if(row == 2){
		lkv_lcd_send_cmd( (column |= 0xC0) );
	}
}

/* Tranform Int Number to String */

char* IntToString1char(uint32_t number){
	static char buffer[2];
	buffer[1] = '\0';
	buffer[0] = (char)(number % 10 + 48);
  return buffer;
}

char* IntToString2char(uint32_t number){
	static char buffer[3];
	buffer[2] = '\0';
	buffer[1] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[0] = (char)(number % 10 + 48);
  return buffer;
}

char* IntToString3char(uint32_t number){
	static char buffer[5];
	buffer[4] = '\0';
	buffer[3] = ' ';
	buffer[2] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[1] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[0] = (char)(number % 10 + 48);
  return buffer;
}

char* IntToString4char(uint32_t number){
	static char buffer[5];
	buffer[4] = '\0';
	buffer[3] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[2] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[1] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[0] = (char)(number % 10 + 48);
  return buffer;
}

char* IntToString5char(uint32_t number){
	static char buffer[6];
	buffer[5] = '\0';
	buffer[4] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[3] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[2] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[1] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[0] = (char)(number % 10 + 48);
  return buffer;
}

char* lkv_lcd_int_to_string(uint32_t number){
	if(number<10){
		return IntToString1char(number);
	}else if(number<100){
		return IntToString2char(number);
	}else if(number<1000){
		return IntToString3char(number);
	}else if(number<10000){
		return IntToString4char(number);
	}
	else{
	return IntToString5char(number);
	}
}



/* Tranform  Float Number to String */


char* FloatToStringX_XXX(double* value){
	static char buffer[6];
	buffer[5] = '\0';
	buffer[1] = '.' ;
	uint32_t number = (*value) * 1000.001;
	buffer[4] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[3] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[2] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[0] = (char)(number % 10 + 48);
	return buffer;
}


char* FloatToStringXX_XX(double* value){

	static char buffer[6];
	buffer[5] = '\0';
	buffer[2] = '.' ;
	uint32_t number = (*value) * 100.001;
	buffer[4] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[3] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[1] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[0] = (char)(number % 10 + 48);
	return buffer;

}


char* FloatToStringXXX_X(double* value){
	static char buffer[6];
	buffer[5] = '\0';
	buffer[3] = '.' ;
	uint32_t number = (*value) * 10.001;
	buffer[4] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[2] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[1] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[0] = (char)(number % 10 + 48);
	return buffer;
}

char* FloatToStringXXXX(double* value){
	static char buffer[5];
	buffer[4] = '\0';
	uint32_t number = (*value) * 1.0001;
	buffer[3] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[2] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[1] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[0] = (char)(number % 10 + 48);
	return buffer;

}

char* FloatToStringXXXXX(double* value){
	static char buffer[6];
	buffer[5] = '\0';
	uint32_t number = (*value) * 1.000001;
	buffer[4] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[3] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[2] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[1] = (char)(number % 10 + 48);
	number = number / 10 ;
	buffer[0] = (char)(number % 10 + 48);
	return buffer;

}



char* lkv_lcd_float_to_string(double* value){

//	uint32_t temp = (uint32_t)(*value);

	if(*value < 10){

		return FloatToStringX_XXX(value);

	}else if(*value < 100){

		return FloatToStringXX_XX(value);

	}else if(*value < 1000){

		return FloatToStringXXX_X(value);

	}else if(*value < 10000){

		return FloatToStringXXXX(value);

	}else {

		return FloatToStringXXXXX(value);
	}
}

void lkv_ldc_delay(uint32_t time){
	for(int i = 0; i < time*10 ; i++){
	}
}






