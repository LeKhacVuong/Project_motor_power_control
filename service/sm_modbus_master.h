#ifndef SM_MODBUS_MASTER_H
#define SM_MODBUS_MASTER_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>

typedef void sm_modbus_master_t;

typedef int32_t (*sm_modbus_send_if_fn_t)(const uint8_t*, int32_t, int32_t, void*);
typedef int32_t (*sm_modbus_recv_if_fn_t)(uint8_t*, int32_t, int32_t, void*);

sm_modbus_master_t* sm_modbus_master_create_default(sm_modbus_send_if_fn_t _send_if,
                                                    sm_modbus_recv_if_fn_t _recv_if,
                                                    void* _arg); 

sm_modbus_master_t* sm_modbus_master_create(sm_modbus_send_if_fn_t _send_if, 
                                            sm_modbus_recv_if_fn_t _recv_if,
                                            void* _arg); 

int32_t sm_modbus_master_destroy(sm_modbus_master_t* _this);

int32_t sm_modbus_master_set_response_time(sm_modbus_master_t* _this, uint32_t _time); 

int32_t sm_modbus_master_read_input_registers(sm_modbus_master_t* _this, uint8_t _addr, uint16_t _start_addr, uint16_t _quatity, uint16_t* _regs);

int32_t sm_modbus_master_read_holding_registers(sm_modbus_master_t* _this, uint8_t _addr, uint16_t _start_addr, uint16_t _quatity, uint16_t* _regs);

int32_t sm_modbus_master_write_single_register(sm_modbus_master_t* _this, uint16_t _addr, uint16_t _start_addr, uint16_t _value);

int32_t sm_modbus_master_write_multiple_registers(sm_modbus_master_t* _this, uint16_t _addr, uint16_t _start_addr, uint16_t _quatity, const uint16_t* _regs);

#ifdef __cplusplus
};
#endif

#endif  /* SM_MODBUS_MASTER_H */