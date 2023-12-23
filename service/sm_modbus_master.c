#include "sm_modbus_master.h"
#include "nanoModbus/nanomodbus.h"

#define _impl(p) ((sm_modbus_master_impl_t*)(p))

typedef struct sm_modbus_master_impl_t{
    nmbs_t m_instance;

    sm_modbus_send_if_fn_t m_mb_send_if;
    sm_modbus_recv_if_fn_t m_mb_recv_if;
    void* m_arg;
}sm_modbus_master_impl_t;


static int32_t nmbs_platform_read(uint8_t* buf,
                                  uint16_t count,
                                  int32_t byte_timeout_ms,
                                  void* arg){
    return _impl(arg)->m_mb_recv_if(buf, count, byte_timeout_ms, NULL);
}

static int32_t nmbs_platform_write(const uint8_t* buf,
                                   uint16_t count,
                                   int32_t byte_timeout_ms,
                                   void* arg){
    return _impl(arg)->m_mb_send_if(buf, count, byte_timeout_ms, NULL);
}

static sm_modbus_master_impl_t g_mb_master = {
    .m_mb_send_if = NULL,
    .m_mb_recv_if = NULL,
    .m_arg = NULL
};


sm_modbus_master_t* sm_modbus_master_create_default(sm_modbus_send_if_fn_t _send_if, 
                                                    sm_modbus_recv_if_fn_t _recv_if,
                                                    void* _arg){
	g_mb_master.m_mb_recv_if = _recv_if;
	g_mb_master.m_mb_send_if = _send_if;

    nmbs_platform_conf nmbs_if = {
        .read = nmbs_platform_read,
        .write = nmbs_platform_write,
        .arg = &g_mb_master,
        .transport = NMBS_TRANSPORT_RTU
    };

    nmbs_error err = nmbs_client_create(&g_mb_master.m_instance, &nmbs_if);

    sm_modbus_master_set_response_time(&g_mb_master,500);

    if(err != NMBS_ERROR_NONE){
        return NULL;
    }

    return &g_mb_master;                                                       
}

sm_modbus_master_t* sm_modbus_master_create(sm_modbus_send_if_fn_t _send_if, 
                                            sm_modbus_recv_if_fn_t _recv_if,
                                            void* _arg){
    return NULL;                                                   
} 

int32_t sm_modbus_master_destroy(sm_modbus_master_t* _this){
    return 0;
}

int32_t sm_modbus_master_set_response_time(sm_modbus_master_t* _this, uint32_t _timeout){
    nmbs_set_byte_timeout(&_impl(_this)->m_instance, _timeout);
    nmbs_set_read_timeout(&_impl(_this)->m_instance, _timeout);
    return 0;
}

int32_t sm_modbus_master_read_input_registers(sm_modbus_master_t* _this, uint8_t _addr, uint16_t _start_addr, uint16_t _quantity, uint16_t* _regs){
    nmbs_set_destination_rtu_address(&_impl(_this)->m_instance, _addr);
    nmbs_error err = nmbs_read_input_registers(&_impl(_this)->m_instance, _start_addr, _quantity, _regs);

    if(err != NMBS_ERROR_NONE){
//        LOG_ERROR(TAG, "Read Input registers ERROR: %d", err);
    }

    return err;
}

int32_t sm_modbus_master_read_holding_registers(sm_modbus_master_t* _this, uint8_t _addr, uint16_t _start_addr, uint16_t _quantity, uint16_t* _regs){
 nmbs_set_destination_rtu_address(&_impl(_this)->m_instance, _addr);
    nmbs_error err = nmbs_read_holding_registers(&_impl(_this)->m_instance, _start_addr, _quantity, _regs);

    if(err != NMBS_ERROR_NONE){
//        LOG_ERROR(TAG, "Read holding registers ERROR: %d", err);
    }

    return err;
}

int32_t sm_modbus_master_write_single_register(sm_modbus_master_t* _this, uint16_t _addr, uint16_t _start_addr, uint16_t _value){
    nmbs_set_destination_rtu_address(&_impl(_this)->m_instance, _addr);
    nmbs_error err = nmbs_write_single_register(&_impl(_this)->m_instance, _start_addr, _value);

    if(err != NMBS_ERROR_NONE){
        // LOG_ERROR(TAG, "Write single register ERROR: %d", err);
    }

    return err;
}

int32_t sm_modbus_master_write_multiple_registers(sm_modbus_master_t* _this, uint16_t _addr, uint16_t _start_addr, uint16_t _quantity, const uint16_t* _regs){
    nmbs_set_destination_rtu_address(&_impl(_this)->m_instance, _addr);
    nmbs_error err = nmbs_write_multiple_registers(&_impl(_this)->m_instance, _start_addr, _quantity, _regs);

    if(err != NMBS_ERROR_NONE){
        // LOG_ERROR(TAG, "Write write multiple registers ERROR: %d", err);
    }

    return err;
}
