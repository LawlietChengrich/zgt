#include "can_app.h"
#include "gpio.h"
#include "can.h"


void dh_can_cmd_remote_req(uint8_t* data);
void dh_can_cmd_mppt_get(void);
void dh_can_cmd_bat_get(void);
void dh_can_cmd_wing_get(void);

void dh_can_cmd_short(uint8_t* data);
void dh_can_cmd_mppt_ctrl(dh_can_cmd_mppt_t cmd);
void dh_can_cmd_bat_ctrl(dh_can_cmd_bat_t cmd);
void dh_can_cmd_wing1_ctrl(dh_can_cmd_wing1_t cmd);
void dh_can_cmd_wing2_ctrl(dh_can_cmd_wing2_t cmd);

//dh_can_data_t can_data;

void dh_can_data_cmd_process(uint32_t canid, uint8_t* data)
{
    dh_can_data_t can_data;
    can_data.fc = (canid >> (CAN_ID_FC-CAN_ID_FC)) & (~(0xff<<(CAN_ID_FT-CAN_ID_FC)));
    can_data.ft = (canid >> (CAN_ID_FT-CAN_ID_FC)) & (~(0xff<<(CAN_ID_SA-CAN_ID_FT)));
    can_data.sa = (canid >> (CAN_ID_SA-CAN_ID_FC)) & (~(0xff<<(CAN_ID_DA-CAN_ID_SA)));
    can_data.da = (canid >> (CAN_ID_DA-CAN_ID_FC)) & (~(0xff<<(CAN_ID_DT-CAN_ID_DA)));
    can_data.dt = (canid >> (CAN_ID_DT-CAN_ID_FC)) & (~(0xff<<(CAN_ID_LT-CAN_ID_DT)));
    can_data.lt = (canid >> (CAN_ID_LT-CAN_ID_FC)) & (~(0xff<<(CAN_ID_P-CAN_ID_LT)));
    can_data.p = (canid >> (CAN_ID_P-CAN_ID_FC)) & (~(0xff<<(MAX_CAN_ID_BIT_LEN-CAN_ID_P)));

    switch ((dh_can_dt_t)can_data.dt)
    {
        case CAN_ID_DT_REMOTE_REQ:
            dh_can_cmd_remote_req(data);
            break;
        case CAN_ID_DT_SHORT_CMD:
            dh_can_cmd_short(data);
            break;
        default:
            break;
    }
}

void dh_can_cmd_remote_req(uint8_t* data)
{
    switch ((dh_can_cmd_remote_t)data[0])
    {
        case CAN_CMD_REMOTE_WING:
            dh_can_cmd_wing_get();
            break;
        case CAN_CMD_REMOTE_BAT:
            dh_can_cmd_bat_get();
            break;
        case CAN_CMD_REMOTE_MPPT:
            dh_can_cmd_mppt_get();
            break;                             
        default:
            break;
    }
}

void dh_can_cmd_short(uint8_t* data)
{
    switch ((dh_can_cmd_ctrl_t)data[0])
    {
        case CAN_CMD_CTRL_MPPT:
            dh_can_cmd_mppt_ctrl((dh_can_cmd_mppt_t)data[1]);
            break;
        case CAN_CMD_CTRL_BAT:
            dh_can_cmd_bat_ctrl((dh_can_cmd_bat_t)data[1]);
            break;
        case CAN_CMD_CTRL_WING1:
            dh_can_cmd_wing1_ctrl((dh_can_cmd_wing1_t)data[1]);
            break;
        case CAN_CMD_CTRL_WING2:
            dh_can_cmd_wing2_ctrl((dh_can_cmd_wing2_t)data[1]);
            break;                                
        default:
            break;
    }
}

void dh_can_cmd_mppt_get(void)
{

}

void dh_can_cmd_bat_get(void)
{

}

void dh_can_cmd_wing_get(void)
{
    
}

void dh_can_cmd_mppt_ctrl(dh_can_cmd_mppt_t cmd)
{

}

void dh_can_cmd_bat_ctrl(dh_can_cmd_bat_t cmd)
{

}

void dh_can_cmd_wing1_ctrl(dh_can_cmd_wing1_t cmd)
{

}

void dh_can_cmd_wing2_ctrl(dh_can_cmd_wing2_t cmd)
{

}
