#ifndef __CAN_APP_H__
#define __CAN_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define MPPT_NUM    (9)
#define MAX_APP_CAN_DATA_LEN (64)
#define CAN_APP_DATA_UINT_LEN   (8)

#define CAN_CMD_REMOTE_WING (0xFD)
#define CAN_CMD_REMOTE_BAT  (0xFE)
#define CAN_CMD_REMOTE_MPPT (0xFF)

#define CAN_CMD_RESET_CAN12 (0xF9)
#define CAN_CMD_RESET_CAN1  (0xF8)
#define CAN_CMD_RESET_CAN2  (0xF7)


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

typedef struct
{
    uint8_t mppt1_status:1;
    uint8_t mppt2_status:1;
    uint8_t mppt3_status:1;
    uint8_t mppt4_status:1;
    uint8_t mppt5_status:1;
    uint8_t mppt6_status:1;
    uint8_t mppt7_status:1;
    uint8_t mppt8_status:1;
}bit_union_mppt_t;

typedef struct
{
    uint8_t mppt9_status:1;
    uint8_t mppt_reserve:7;
}bit_union_mppt1_t;

typedef struct
{
    uint8_t low_vol_protect:1;
    uint8_t self_energizing:1;
    uint8_t discharge_status:1;
    uint8_t reserve:5;
}bit_union_bat_t;

typedef struct
{
    uint8_t qv_a:1;
    uint8_t qv_b:1;
    uint8_t hall:1;
    uint8_t phase_ar_sda:1;
    uint8_t phase_ar_sdb:1;
    uint8_t phase_ar_rv:1;
    uint8_t reserve:2;
}bit_union_wing1_t;

typedef struct
{
    uint8_t flyx:1;
    uint8_t flyy:1;
    uint8_t flyz:1;
    uint8_t flys:1;
    uint8_t board_lock_status:1;
    uint8_t reserve:3;
}bit_union_wing2_t;

typedef struct
{
    uint8_t correct_cnt:4;
    uint8_t error_cnt:4;
}bit_union_cmd_cnt_t;

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

typedef struct
{
    uint8_t data_z;
    uint8_t data_f;
}dh_can_ui_t;

typedef struct
{
    dh_can_ui_t value_u[MPPT_NUM];
    dh_can_ui_t value_i[MPPT_NUM];
    bit_union_mppt_t mppt_st1;
    bit_union_mppt1_t mppt_st2;
}dh_can_mppt_inf_t;

typedef struct
{
    dh_can_ui_t bus_u;
    dh_can_ui_t bat_u;
    dh_can_ui_t load_i_sum;
    dh_can_ui_t bat_i;
    bit_union_bat_t status;
}dh_can_bat_inf_t;

typedef struct
{
    bit_union_wing1_t status1;
    bit_union_wing2_t status2;
}dh_can_wing_inf_t;

typedef struct
{
    uint8_t tmt;
    uint8_t remote_cmd_cnt;
    bit_union_cmd_cnt_t cmd_cnt;//high 4:correct cmd, low 4:err cmd
    uint8_t cmd_latest;
    uint8_t backup_data_cnt;
}dh_can_remote_head_t;
#pragma pack()

extern void dh_can_data_cmd_process(uint32_t canid, uint8_t* data);
extern void dh_can_data_send_process(void);

#ifdef __cplusplus
}
#endif

#endif /* __CAN_APP_H__ */
