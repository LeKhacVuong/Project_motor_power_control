
#include "host_master.h"
#include "board.h"
#include "string.h"
#include "sm_modbus_master.h"
#include "service/sm_modbus_master.h"

#define RS485_BUFFER_SIZE		        1024
#define CO_QUEUE_SIZE			        32
#define MODBUS_RESPONSE_TIMEOUT_DEFAULT	1000
#define SM_FUNC_READ_INPUT              0x04
#define SM_FUNC_READ_HOLDING            0x03



typedef struct {
	uint8_t data[16];
	uint8_t len;
}sm_canbus_msg_t;

typedef struct {
	uint8_t buf[RS485_BUFFER_SIZE];
	uint16_t head;
	uint16_t tail;
}sm_rs485_ring_buffer_t;

sm_host_t* host_master;

sm_modbus_master_t* g_modbus_master;
sm_rs485_ring_buffer_t g_rs485_buffer;

volatile uint32_t g_time_stamp = 0;
uint32_t g_hmi_time_out = 0;
IWDG_HandleTypeDef g_hiwdg;


static sm_canbus_msg_t g_co_queue[CO_QUEUE_SIZE] = {0,};
static int32_t g_queue_head = 0;
static int32_t g_queue_tail = 0;

uint8_t ring_buffer[1024] = {0};
volatile uint16_t front_index = 0;
volatile uint16_t rear_index = 0;


static void can_receive_handle(CAN_Hw *p_hw);

int32_t host_receive_cmd_callback(int32_t _cmd, const uint8_t* _data, int32_t _len, void* _arg ){
	(void)_arg;
	g_hmi_time_out = g_time_stamp + 3000;

	if(_cmd == SM_CMD_UPGRADE_FW){
		can_port.can_tx.StdId = ((uint16_t)_data[0] << 8) | (uint16_t)_data[1];
		can_port.can_tx.DLC = _len - 2;
		can_send(&can_port, (uint8_t*)(_data + 2));
		return _len;
	}

	/// TODO: Update read Modbus Register

	if(_cmd == SM_CMD_READ_MODBUS){
		uint8_t  function     = _data[0];
		uint8_t  modbusId	 = _data[1];
		uint16_t regAddr     = *(uint16_t*)(_data + 2);
		uint16_t quantity    = *(uint16_t*)(_data + 4);
		int8_t ret = 0;
		uint8_t buff[128];

		for(int i = 0; i < 128; i++){
			*(uint8_t*)(buff + i) = 0;
		}

		for(int i = 0; i < 6; i++){
			buff[i] = _data[i];
		}

		if(function == SM_FUNC_READ_INPUT){
			ret = sm_modbus_master_read_input_registers(g_modbus_master,modbusId, regAddr,quantity, (uint16_t*)&buff[6]);
			if(ret != 0){
				sm_host_send_response(host_master, SM_CMD_READ_MODBUS, ret, (uint8_t*)buff, 6);
				return -1;
			}

		}else if(function == SM_FUNC_READ_HOLDING){
			ret = sm_modbus_master_read_holding_registers(g_modbus_master,modbusId, regAddr,quantity, (uint16_t*)&buff[6]);
			if(ret != 0){
				sm_host_send_response(host_master, SM_CMD_READ_MODBUS, ret, (uint8_t*)buff, 6);
				return -1;
			}
		}else{
			return -1;
		}

		sm_host_send_response(host_master, SM_CMD_READ_MODBUS, ret, (uint8_t*)buff, 6 + 2 * quantity);

		return 0;
	}
	
	return 0;

}

int32_t host_send_data_interface(const uint8_t* _data, int32_t _len){
	HAL_UART_Transmit(&debug_com.uart_module, (uint8_t*)_data, _len, 500);
	HAL_UART_Transmit(&hmi_com.uart_module, (uint8_t*)_data, _len, 500);
	return _len;
}

void sys_init(void) {
	can_set_receive_handle(&can_port, can_receive_handle);
	board_init();
	host_master = sm_host_create(SM_HOST_ADDR_DEFAULT,host_send_data_interface);
	sm_host_reg_handle(host_master,host_receive_cmd_callback,NULL);
}


void iwdg_init(void){

	  g_hiwdg.Instance = IWDG;
	  g_hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
	  g_hiwdg.Init.Reload = 312;
	  HAL_IWDG_Init(&g_hiwdg);

}

int32_t sm_rs485_feed(const uint8_t _data){
	g_rs485_buffer.buf[g_rs485_buffer.head++] = _data;
	if(g_rs485_buffer.head >= RS485_BUFFER_SIZE){
		g_rs485_buffer.head = 0;
	}
	return 1;
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
	__disable_irq();
	sys_init();
//	iwdg_init();
	g_modbus_master = sm_modbus_master_create_default(sm_modbus_send_if, sm_modbus_recv_if, NULL);
	g_hmi_time_out = g_time_stamp + 3000;
	__enable_irq();

	while (1){
		sm_host_process(host_master);

		if(g_queue_head != g_queue_tail){
			sm_host_send_response(host_master, 0x10, 0x00, g_co_queue[g_queue_head].data, g_co_queue[g_queue_head].len);
			g_queue_head++;
			if(g_queue_head >= CO_QUEUE_SIZE){
				g_queue_head = 0;
			}
		}

		if(g_time_stamp >= g_hmi_time_out){
			NVIC_SystemReset();
		}
//		HAL_IWDG_Refresh(&g_hiwdg);
	}
}


static void can_receive_handle(CAN_Hw *p_hw){

	uint32_t cob_id = p_hw->can_rx.StdId;
	uint8_t len = 0;

	g_co_queue[g_queue_tail].data[len++] = (cob_id >> 8) & 0xFF;
	g_co_queue[g_queue_tail].data[len++] = (cob_id) & 0xFF;

	for (uint32_t i = 0; i < p_hw->can_rx.DLC; i++) {
		g_co_queue[g_queue_tail].data[len++] = p_hw->rx_data[i];
	}
	g_co_queue[g_queue_tail].len = len;

	g_queue_tail++;
	if(g_queue_tail >= CO_QUEUE_SIZE){
		g_queue_tail = 0;
	}

}


void HAL_STATE_MACHINE_UPDATE_TICK(void)
{
	  HAL_IncTick();
	g_time_stamp = g_time_stamp + 1;
}

void USART1_IRQHandler(void){
	HAL_UART_IRQHandler(&rs485_com.uart_module);
	uart_receives(&rs485_com, (char*)&rs485_com.rx_data);
//	sm_rs485_feed(rs485_com.rx_data);
	ring_buffer[front_index] = rs485_com.rx_data;
	front_index++;
	if(front_index >= 1024){
		front_index = 0;
	}
}

void USART3_IRQHandler(void){
	HAL_UART_IRQHandler(&hmi_com.uart_module);
	uart_receives(&hmi_com, (char*)&hmi_com.rx_data);
	sm_host_asyn_feed((uint8_t*)&hmi_com.rx_data, 1, host_master);
}


void USART2_IRQHandler(void){
	HAL_UART_IRQHandler(&debug_com.uart_module);
	uart_receives(&debug_com, (char*)&debug_com.rx_data);
	sm_host_asyn_feed((uint8_t*)&debug_com.rx_data, 1, host_master);
}



