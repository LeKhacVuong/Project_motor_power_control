#include "can_hardware.h"
#include "string.h"
#include "interrupt_hw.h"

CAN_Hw can_port;
static void can_send_impl(CAN_Hw* p_hw);
static void can_hardware_init_module(void);
static void can_hardware_filter_init(void);
static void can_hardware_start(void);
static void can_hardware_init_tx(void);
static void can_hardware_init_nvic(void);

void can_hardware_init(void) {
	can_hardware_init_module();
	can_hardware_init_nvic();
	can_hardware_filter_init();
	can_hardware_init_tx();
	can_port.can_send = can_send_impl;
	can_hardware_start();
}

void can_send(CAN_Hw* p_hw, uint8_t* buff){

	HAL_CAN_AddTxMessage(&p_hw->can_module, &p_hw->can_tx,buff, &p_hw->tx_mailbox);
}

void can_set_receive_handle(CAN_Hw* p_hw,void (*receive_handle)(CAN_Hw* p_hw)){
	(void)p_hw;
	can_port.receive_handle=receive_handle;
}


static void can_hardware_filter_init(void){
	CAN_FilterTypeDef  sFilterConfig;

	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;
	HAL_CAN_ConfigFilter(&can_port.can_module, &sFilterConfig);
}

static void can_hardware_init_module(void) {
	__HAL_RCC_CAN1_CLK_ENABLE();
	/* CAN Baudrate 500kHz */
	can_port.can_module.Instance = CAN1;
	can_port.can_module.Init.Prescaler = 3;
	can_port.can_module.Init.Mode = CAN_MODE_NORMAL;
	can_port.can_module.Init.SyncJumpWidth = CAN_SJW_1TQ;
	can_port.can_module.Init.TimeSeg1 = CAN_BS1_15TQ;
	can_port.can_module.Init.TimeSeg2 = CAN_BS2_8TQ;
	can_port.can_module.Init.AutoBusOff = ENABLE;
	can_port.can_module.Init.AutoRetransmission = DISABLE;
	can_port.can_module.Init.AutoWakeUp = DISABLE;
	can_port.can_module.Init.ReceiveFifoLocked = DISABLE;
	can_port.can_module.Init.TimeTriggeredMode = DISABLE;
	can_port.can_module.Init.TransmitFifoPriority = DISABLE;
	if(HAL_CAN_Init(&can_port.can_module) != HAL_OK){
		Error_Handler();
	}
}

static void can_hardware_init_nvic(void){
    /* CAN interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, CAN_IRQN_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

static void can_hardware_start(void){
	if (HAL_CAN_Start(&can_port.can_module) != HAL_OK){
		Error_Handler();
	}

	if (HAL_CAN_ActivateNotification(&can_port.can_module, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK){
		Error_Handler();
	}
}

static void can_hardware_init_tx(void){
	can_port.can_tx.StdId = 1;
	can_port.can_tx.DLC = 8;
	can_port.can_tx.RTR = CAN_RTR_DATA;
	can_port.can_tx.IDE = CAN_ID_STD;
	can_port.can_tx.TransmitGlobalTime = DISABLE;
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}


static void can_send_impl(CAN_Hw* p_hw){
	HAL_CAN_AddTxMessage(&p_hw->can_module, &p_hw->can_tx, p_hw->tx_data, &p_hw->tx_mailbox);
}

void USB_LP_CAN1_RX0_IRQHandler(void)                          //@vuong can receive
{
	HAL_CAN_GetRxMessage(&can_port.can_module, CAN_RX_FIFO0, &can_port.can_rx, can_port.rx_data);
	if(can_port.receive_handle != NULL)
	{
		can_port.receive_handle(&can_port);
	}
	HAL_CAN_IRQHandler(&can_port.can_module);
}

