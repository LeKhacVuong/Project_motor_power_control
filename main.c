
#include "host_master.h"
#include "board.h"
#include "string.h"
#include "sm_modbus_master.h"
#include "service/sm_modbus_master.h"
#include "string_util.h"
#include "lkv_lcd.h"
#include "math.h"

#define EMETER_ADDRESS 0x01

#define EMETER_DATA_REG_NUM 2

#define REG_VOLTAGE_INDEX 0x00
#define REG_CURRENT_INDEX 0x08
#define REG_COS_PHI_INDEX 0x2A
#define REG_FREQUEN_INDEX 0x36

sm_modbus_master_t* g_modbus_master;
volatile uint32_t g_time_stamp = 0;
uint8_t display_stt = 0;


uint8_t ring_buffer[1024] = {0};
volatile uint16_t front_index = 0;
volatile uint16_t rear_index = 0;

float voltage = 0;
float current = 0;
float cos_phi = 0;
float frequen = 0;



void syn_emeter_data(uint8_t _index){
	uint16_t buff_modbus[2] = {0};
	if(_index == 0){
		int8_t ret = sm_modbus_master_read_input_registers(g_modbus_master,EMETER_ADDRESS, REG_VOLTAGE_INDEX,EMETER_DATA_REG_NUM, (uint16_t*)buff_modbus);
		if(ret == 0){
			union {
				uint32_t u;
				float f;
			} temp_union;
			temp_union.u = (buff_modbus[0]<<16) + buff_modbus[1];
			voltage = temp_union.f;

		}else{
			voltage = 0;
		}
		return;
	}else if(_index == 1){
		int8_t ret = sm_modbus_master_read_input_registers(g_modbus_master,EMETER_ADDRESS, REG_CURRENT_INDEX,EMETER_DATA_REG_NUM, (uint16_t*)buff_modbus);
		if(ret == 0){
			union {
				uint32_t u;
				float f;
			} temp_union;
			temp_union.u = (buff_modbus[0]<<16) + buff_modbus[1];
			current = temp_union.f;		}else{
			current = 0;
		}
		return;
	}else if(_index == 2){
		int8_t ret = sm_modbus_master_read_input_registers(g_modbus_master,EMETER_ADDRESS, REG_COS_PHI_INDEX,EMETER_DATA_REG_NUM, (uint16_t*)buff_modbus);
		if(ret == 0){
			union {
				uint32_t u;
				float f;
			} temp_union;
			temp_union.u = (buff_modbus[0]<<16) + buff_modbus[1];
			cos_phi = temp_union.f;
		}else{
			cos_phi = 0;
		}
		return;
	}else if(_index == 3){
		int8_t ret = sm_modbus_master_read_input_registers(g_modbus_master,EMETER_ADDRESS, REG_FREQUEN_INDEX,EMETER_DATA_REG_NUM, (uint16_t*)buff_modbus);
		if(ret == 0){
			union {
				uint32_t u;
				float f;
			} temp_union;
			temp_union.u = (buff_modbus[0]<<16) + buff_modbus[1];
			frequen = temp_union.f;
		}else{
			frequen = 0;
		}
		return;
	}

	return;
}

static int32_t sm_modbus_recv_if(uint8_t* buf,
                                  int32_t count,
                                  int32_t byte_timeout_ms,
                                  void* arg){
	(void)arg;
	uint32_t timeout = g_time_stamp + byte_timeout_ms;
	while(g_time_stamp < timeout){
		int32_t gap = 0;
		if(front_index > rear_index){
			gap = front_index - rear_index;
		}
		if(front_index < rear_index){
			gap = 1024 + front_index - rear_index;
		}
		if(gap >= count){
			for(int i = 0; i < count; i++){
				*(buf + i) = ring_buffer[rear_index];
				rear_index++;
				if(rear_index >= 1024){
					rear_index = 0;
				}
			}
			return count;
		}
	}
	return 0;

}

static int32_t sm_modbus_send_if(const uint8_t* _data,
                                   int32_t _count,
                                   int32_t _timeout,
                                   void* _arg){
    (void)_arg;
    MAX485_SET_DIR_TX;
    for(int i = 0; i < 15000; i++){
    }
	uart_send_bytes(&rs485_com, _data, _count, _timeout);
    for(int i = 0; i < 15000; i++){
    }
	MAX485_SET_DIR_RX;
	return _count;
}

int main(void) {
	board_init();
	lkv_lcd_init();
	g_modbus_master = sm_modbus_master_create_default(sm_modbus_send_if, sm_modbus_recv_if, NULL);

	char buff_lcd[16];


	while (1){
		for(int i = 0; i < 4; i++){
			syn_emeter_data(i);
		}


		if(display_stt == 0){

			lkv_lcd_process(NOT_CLEAR_DIS, 2, 1,"u-");
			float_to_string(voltage, buff_lcd);
			buff_lcd[3] = '\0';
			lkv_lcd_process(NOT_CLEAR_DIS, 2, 3, buff_lcd);


			lkv_lcd_process(NOT_CLEAR_DIS, 2, 6,"   i-");
			float_to_string(current, buff_lcd);
			lkv_lcd_process(NOT_CLEAR_DIS, 2, 11, buff_lcd);

		}else{

			buff_lcd[0] = 0b11100000;
			buff_lcd[1] = '-';
			buff_lcd[2] = '\0';

			lkv_lcd_process(NOT_CLEAR_DIS, 2, 1,buff_lcd);
			float_to_string(cos_phi, buff_lcd);
			lkv_lcd_process(NOT_CLEAR_DIS, 2, 3, buff_lcd);


			lkv_lcd_process(NOT_CLEAR_DIS, 2, 9,"f-");
			float_to_string(frequen, buff_lcd);
			buff_lcd[4] = '\0';
			lkv_lcd_process(NOT_CLEAR_DIS, 2, 11, buff_lcd);


		}


	}
}




void HAL_STATE_MACHINE_UPDATE_TICK(void)
{
	  HAL_IncTick();
	g_time_stamp++;
	if(g_time_stamp % 3500 == 0){
		display_stt = !display_stt;
	}
}

void USART1_IRQHandler(void){
	HAL_UART_IRQHandler(&rs485_com.uart_module);
	uart_receives(&rs485_com, (char*)&rs485_com.rx_data);
	ring_buffer[front_index] = rs485_com.rx_data;
	front_index++;
	if(front_index >= 1024){
		front_index = 0;
	}
}

void USART3_IRQHandler(void){
	HAL_UART_IRQHandler(&hmi_com.uart_module);
	uart_receives(&hmi_com, (char*)&hmi_com.rx_data);
}


void USART2_IRQHandler(void){
	HAL_UART_IRQHandler(&debug_com.uart_module);
	uart_receives(&debug_com, (char*)&debug_com.rx_data);
}



