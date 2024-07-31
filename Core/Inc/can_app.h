#ifndef __CAN_APP_H__
#define __CAN_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum
{
    CAN_CMD_REMOTE_WING = 0xFD,
    CAN_CMD_REMOTE_BAT = 0xFE,
    CAN_CMD_REMOTE_MPPT = 0xFF,
}dh_can_cmd_remote_t;

typedef enum
{
    CAN_CMD_CTRL_MPPT = 0x01,
    CAN_CMD_CTRL_BAT = 0x02,
    CAN_CMD_CTRL_WING1 = 0x04,
    CAN_CMD_CTRL_WING2 = 0x07,
}dh_can_cmd_ctrl_t;

typedef enum
{
    CAN_CMD_MPPT1_ST_0 = 0x00,
    CAN_CMD_MPPT1_ST_1 = 0x01,
    CAN_CMD_MPPT2_ST_0 = 0x02,
    CAN_CMD_MPPT2_ST_1 = 0x03,
    CAN_CMD_MPPT3_ST_0 = 0x04,
    CAN_CMD_MPPT3_ST_1 = 0x05,
    CAN_CMD_MPPT4_ST_0 = 0x06,
    CAN_CMD_MPPT4_ST_1 = 0x07,
    CAN_CMD_MPPT5_ST_0 = 0x08,
    CAN_CMD_MPPT5_ST_1 = 0x09,
    CAN_CMD_MPPT6_ST_0 = 0x0A,
    CAN_CMD_MPPT6_ST_1 = 0x0B,
    CAN_CMD_MPPT7_ST_0 = 0x0C,
    CAN_CMD_MPPT7_ST_1 = 0x0D,
    CAN_CMD_MPPT8_ST_0 = 0x0E,
    CAN_CMD_MPPT8_ST_1 = 0x0F,
    CAN_CMD_MPPT9_ST_0 = 0x10,
    CAN_CMD_MPPT9_ST_1 = 0x11,
}dh_can_cmd_mppt_t;

typedef enum
{
    CAN_CMD_LOW_VOL_PROTECT_DISABLE = 0x00,
    CAN_CMD_LOW_VOL_PROTECT_ENABLE = 0x01,
    CAN_CMD_SELF_ENERGIZING_DISABLE = 0x02,
    CAN_CMD_SELF_ENERGIZING_ENABLE = 0x03,
    CAN_CMD_DISCHARGE_DISABLE = 0x04,
    CAN_CMD_DISCHARGE_ENABLE = 0x05,
    CAN_CMD_ENCHARGE_I_GEAR1 = 0x06,
    CAN_CMD_ENCHARGE_I_GEAR2 = 0x07,
    CAN_CMD_ENCHARGE_V_GEAR1 = 0x08,
    CAN_CMD_ENCHARGE_V_GEAR2 = 0x09,
    CAN_CMD_ENCHARGE_V_GEAR3 = 0x0A,
}dh_can_cmd_bat_t;

typedef enum
{
    CAN_CMD_QV_A_DISCONNECNT = 0x00,
    CAN_CMD_QV_A_CONNECNT = 0x01,
    CAN_CMD_QV_B_DISCONNECNT = 0x02,
    CAN_CMD_QV_B_CONNECNT = 0x03,
    CAN_CMD_HALL_DISCONNECNT = 0x04,
    CAN_CMD_HALL_CONNECNT = 0x05,
    CAN_CMD_PHASE_AR_SENDA_DISCONNECNT = 0x06,
    CAN_CMD_PHASE_AR_SENDA_CONNECNT = 0x07,
    CAN_CMD_PHASE_AR_SENDB_DISCONNECNT = 0x08,
    CAN_CMD_PHASE_AR_SENDB_CONNECNT = 0x09,
    CAN_CMD_PHASE_AR_RECV_DISCONNECNT = 0x0A,
    CAN_CMD_PHASE_AR_RECV_CONNECNT = 0x0B,
    CAN_CMD_FLYWHELL_X_DISCONNECNT = 0x0C,
    CAN_CMD_FLYWHELL_X_CONNECNT = 0x0D,
    CAN_CMD_FLYWHELL_Y_DISCONNECNT = 0x0E,
    CAN_CMD_FLYWHELL_Y_CONNECNT = 0x0F,
    CAN_CMD_FLYWHELL_Z_DISCONNECNT = 0x10,
    CAN_CMD_FLYWHELL_Z_CONNECNT = 0x11,
    CAN_CMD_FLYWHELL_S_DISCONNECNT = 0x12,
    CAN_CMD_FLYWHELL_S_CONNECNT = 0x13,
}dh_can_cmd_wing1_t;

typedef enum
{
    CAN_CMD_BOARD_ANTENNA_UNLOCK_DISCONNECT = 0x00,
    CAN_CMD_BOARD_ANTENNA_UNLOCK_CONNECT = 0x01,
    CAN_CMD_BOARD_UNLOCK = 0x02,
    CAN_CMD_LIFT_UNLOCK = 0x03,
    CAN_CMD_ANTENNA_A_UNLOCK = 0x04,
    CAN_CMD_ANTENNA_B_UNLOCK = 0x05,
}dh_can_cmd_wing2_t;

#pragma pack(1)
typedef struct
{
    uint8_t fc;
    uint8_t ft;
    uint8_t sa;
    uint8_t da;
    uint8_t dt;
    uint8_t lt;
    uint8_t p;
}dh_can_data_t;
#pragma pack()

extern void dh_can_data_cmd_process(uint32_t canid, uint8_t* data);

#ifdef __cplusplus
}
#endif

#endif /* __CAN_APP_H__ */
