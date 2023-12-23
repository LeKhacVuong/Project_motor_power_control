#include "uart_hw.h"
#include "host_master.h"
UART_hw hmi_com;
UART_hw rs485_com;
UART_hw debug_com;
DMA_HandleTypeDef hdma_usart3_tx;
extern sm_host_t* host_master;
static void uart_hmi_hw_init(void);
static void uart_rs485_hw_init(void);
static void uart_debug_hw_init(void);

static void max485_gpio_hw_init(void);

extern int32_t sm_rs485_feed(const uint8_t _data);

void uart_hw_init(void){
	uart_hmi_hw_init();
	uart_rs485_hw_init();
	uart_debug_hw_init();
	max485_gpio_hw_init();
}

static void max485_gpio_hw_init(void){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOA_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(EN_RS485_PORT, EN_RS485_PIN, GPIO_PIN_RESET);

	  /*Configure GPIO pin : PtPin */
	  GPIO_InitStruct.Pin = EN_RS485_PIN;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(EN_RS485_PORT, &GPIO_InitStruct);
}

static void uart_hmi_hw_init(void){
	hmi_com.uart_module.Instance = HMI_PORT_COM;
	hmi_com.uart_module.Init.BaudRate = UART_BAUDRATE_HMI;
	hmi_com.uart_module.Init.WordLength = UART_WORDLENGTH_8B;
	hmi_com.uart_module.Init.StopBits = UART_STOPBITS_1;
	hmi_com.uart_module.Init.Parity = UART_PARITY_NONE;
	hmi_com.uart_module.Init.Mode = UART_MODE_TX_RX;
	hmi_com.uart_module.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	hmi_com.uart_module.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&hmi_com.uart_module) != HAL_OK)
	{
		Error_Handler();
	}
	uart_receives(&hmi_com, (char*)&hmi_com.rx_data);
}

static void uart_rs485_hw_init(void){
	rs485_com.uart_module.Instance = RS485_PORT_COM;
	rs485_com.uart_module.Init.BaudRate = UART_BAUDRATE_SLAVE;
	rs485_com.uart_module.Init.WordLength = UART_WORDLENGTH_8B;
	rs485_com.uart_module.Init.StopBits = UART_STOPBITS_1;
	rs485_com.uart_module.Init.Parity = UART_PARITY_NONE;
	rs485_com.uart_module.Init.Mode = UART_MODE_TX_RX;
	rs485_com.uart_module.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	rs485_com.uart_module.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&rs485_com.uart_module) != HAL_OK)
	{
		Error_Handler();
	}
	uart_receives(&rs485_com, (char*)&rs485_com.rx_data);
}

void UART_set_baudrate_rs485(int baudrate){
	rs485_com.uart_module.Instance = RS485_PORT_COM;
	rs485_com.uart_module.Init.BaudRate = baudrate;
	rs485_com.uart_module.Init.WordLength = UART_WORDLENGTH_8B;
	rs485_com.uart_module.Init.StopBits = UART_STOPBITS_1;
	rs485_com.uart_module.Init.Parity = UART_PARITY_NONE;
	rs485_com.uart_module.Init.Mode = UART_MODE_TX_RX;
	rs485_com.uart_module.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	rs485_com.uart_module.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&rs485_com.uart_module) != HAL_OK)
	{
		Error_Handler();
	}
	uart_receives(&rs485_com, (char*)&rs485_com.rx_data);
}

static void uart_debug_hw_init(void)
{
	debug_com.uart_module.Instance = DEBUG_PORT_COM;
	debug_com.uart_module.Init.BaudRate = UART_BAUDRATE_DEBUG;
	debug_com.uart_module.Init.WordLength = UART_WORDLENGTH_8B;
	debug_com.uart_module.Init.StopBits = UART_STOPBITS_1;
	debug_com.uart_module.Init.Parity = UART_PARITY_NONE;
	debug_com.uart_module.Init.Mode = UART_MODE_TX_RX;
	debug_com.uart_module.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	debug_com.uart_module.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&debug_com.uart_module) != HAL_OK)
	{
		Error_Handler();
	}
	uart_receives(&debug_com, (char*)&debug_com.rx_data);
}
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle){

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(uartHandle->Instance == RS485_PORT_COM)
	{
		/* USART1 clock enable */
		__HAL_RCC_USART1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();

		/**USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART1 interrupt Init */
		HAL_NVIC_SetPriority(USART1_IRQn, UART_RX_RS485_IRQN_PRIORITY, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}
	else if(uartHandle->Instance == HMI_PORT_COM)
	{
	    /* USART3 clock enable */
	    __HAL_RCC_USART3_CLK_ENABLE();

	    __HAL_RCC_GPIOB_CLK_ENABLE();
	    /**USART3 GPIO Configuration
	    PB10     ------> USART3_TX
	    PB11     ------> USART3_RX
	    */
	    GPIO_InitStruct.Pin = GPIO_PIN_10;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = GPIO_PIN_11;
	    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* USART3 interrupt Init */
		HAL_NVIC_SetPriority(USART3_IRQn, UART_RX_HMI_IRQN_PRIORITY, 0);
		HAL_NVIC_EnableIRQ(USART3_IRQn);

	}
	else if(uartHandle->Instance == DEBUG_PORT_COM)
	{
		/* Peripheral clock enable */
		__HAL_RCC_USART2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART2 GPIO Configuration
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#if USE_DMA_UART_TRANSMIT
		hdma_usart3_tx.Instance = DMA1_Channel7;
		hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart3_tx.Init.Mode = DMA_NORMAL;
		hdma_usart3_tx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_usart3_tx) != HAL_OK) {
			Error_Handler();
		}

		__HAL_LINKDMA(uartHandle, hdmatx, hdma_usart3_tx);
#endif
		/* USART2 setpriority*/
		HAL_NVIC_SetPriority(USART2_IRQn, UART_RX_DEBUG_IRQN_PRIORITY, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn); /// WARNING: Loss connected

	}
}

void uart_sends(UART_hw* p_hw, uint8_t* s){
#if USE_DMA_UART_TRANSMIT
	int size_s = 0;
	for (uint8_t i = 0; i < 200; i++) {
		if(s[i] == '\0'){
			size_s = i;
			break;
		}
	}
//	HAL_UART_Transmit(&p_hw->uart_module, s, 1, 500);
	HAL_UART_Transmit_DMA(&p_hw->uart_module, s, size_s);
//		if(HAL_OK != HAL_UART_Transmit_DMA(&p_hw->uart_module, s, sizeof(s)))
//		{
//			this->send_state = MCU_UART__SEND_STATE__IDLE;
//		}
//		__HAL_DMA_DISABLE_IT(&p_hw->uart_module, DMA_IT_HT);
#else
	while(*s){
			HAL_UART_Transmit(&p_hw->uart_module, s, 1, 500);
			s++;
		}

#endif
}

void uart_send_bytes(UART_hw* p_hw, const uint8_t* _data, int32_t _len, int32_t _timeout){
	HAL_UART_Transmit(&p_hw->uart_module, (uint8_t*)_data, _len, _timeout);
}

char uart_receives(UART_hw* p_hw, char* s){
	HAL_UART_Receive_IT(&p_hw->uart_module, (uint8_t*)s, 1);
	return *s;
}


void UART_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}
