#include "can_app.h"
#include "gpio.h"
#include "can.h"
#include "adc.h"
#include "string.h"

void dh_can_cmd_remote_req(uint8_t* data);
void dh_can_cmd_mppt_get(void);
void dh_can_cmd_bat_get(void);
void dh_can_cmd_wing_get(void);

void dh_can_cmd_short(uint8_t* data);
void dh_can_cmd_mppt_ctrl(dh_can_cmd_mppt_t cmd);
void dh_can_cmd_bat_ctrl(dh_can_cmd_bat_t cmd);
void dh_can_cmd_wing1_ctrl(dh_can_cmd_wing1_t cmd);
void dh_can_cmd_wing2_ctrl(dh_can_cmd_wing2_t cmd);

dh_can_data_t can_data;
uint8_t can_data_send_buf[MAX_APP_CAN_DATA_LEN];
volatile static uint8_t can_data_send_pkg_sum = 0;
volatile static uint8_t can_data_send_pkg_cur = 0;

dh_can_remote_head_t remote_data_head = {0};

void dh_can_data_cmd_process(uint32_t canid, uint8_t* data)
{
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
//dh_can_mppt_inf_t mppt_data = {0};
void dh_can_cmd_mppt_get(void)
{
	uint8_t i, data_pos = 0;
    uint16_t data_len = 0, len_temp;
    dh_can_mppt_inf_t mppt_data = {0};

    if (LL_GPIO_IsInputPinSet(MPPT1_ST_GPIO_Port, MPPT1_ST_Pin))
    {
        mppt_data.mppt_st1 &= ~(0x1<<0);
        mppt_data.mppt_st1 |= (1<<0); 
    }

    for(i=0; i<MAX_ADC_CHN_NUM; i++)
    {
        mppt_data.value_u[i].data_z = 33 * adc_value_aver[i] / 40950;//计算整数部分
        mppt_data.value_u[i].data_f = ((33 * adc_value_aver[i]) % 40950)*100/40950;//计算小数部分
    }

    memset(can_data_send_buf, 0, MAX_APP_CAN_DATA_LEN);

    data_pos += sizeof(data_len);
    len_temp = sizeof(remote_data_head);
    memcpy(&can_data_send_buf[data_pos], &remote_data_head, len_temp);
    data_pos += len_temp;

    len_temp = sizeof(mppt_data);
    memcpy(&can_data_send_buf[data_pos], &mppt_data, len_temp);
    data_pos += len_temp;

    can_data_send_buf[data_pos] = 0xFC;
    data_pos += 1;

    len_temp = sizeof(data_len);
    data_len = data_pos - len_temp;
    memcpy(&can_data_send_buf[0], &data_len, len_temp);
    can_data_send_pkg_sum = (data_pos - 1)/8 + 1;
    can_data_send_pkg_cur = 0;
}

void dh_can_data_send_process(void)
{
    uint32_t can_id = 0;;

    if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan2) == 0 )
    {
        return;
    }

    if(can_data_send_pkg_sum)
    {
        dh_set_canid(&can_id, CAN_ID_RTR, CAN_ID_RTR_DATA, CAN_ID_IDE-CAN_ID_RTR);
        dh_set_canid(&can_id, CAN_ID_IDE, CAN_ID_IDE_EXTEND, CAN_ID_FC-CAN_ID_IDE);
        dh_set_canid(&can_id, CAN_ID_SA, CAN_ID_ADDRESS_PDCU1_SUB, CAN_ID_DA-CAN_ID_SA);
        dh_set_canid(&can_id, CAN_ID_DA, CAN_ID_ADDRESS_CENRER_CPU, CAN_ID_DT-CAN_ID_DA);
        dh_set_canid(&can_id, CAN_ID_DT, CAN_ID_DT_REMOTE_REQ, CAN_ID_LT-CAN_ID_DT);
        dh_set_canid(&can_id, CAN_ID_LT, CAT_ID_LT_BUSB, CAN_ID_P-CAN_ID_LT);
        dh_set_canid(&can_id, CAN_ID_P, CAT_ID_P_SUB, MAX_CAN_ID_BIT_LEN-CAN_ID_P);

        dh_set_canid(&can_id, CAN_ID_FC, can_data_send_pkg_cur+1, CAN_ID_FT-CAN_ID_FC);
        if(can_data_send_pkg_cur == 0)
        {
            dh_set_canid(&can_id, CAN_ID_FT, CAN_ID_FT_MULTI_FISRT, CAN_ID_SA-CAN_ID_FT);
            dh_can_data_send(can_id, &can_data_send_buf[can_data_send_pkg_cur*8], 8);
            can_data_send_pkg_cur++;
        }
        else if(can_data_send_pkg_cur == (can_data_send_pkg_sum - 1))
        {
            dh_set_canid(&can_id, CAN_ID_FT, CAN_ID_FT_MULTI_END, CAN_ID_SA-CAN_ID_FT);
            dh_can_data_send(can_id, &can_data_send_buf[can_data_send_pkg_cur*8], 8);
            can_data_send_pkg_sum = 0;
            can_data_send_pkg_cur = 0;
        }
        else
        {
            dh_set_canid(&can_id, CAN_ID_FT, CAN_ID_FT_MULTI_MID, CAN_ID_SA-CAN_ID_FT);
            dh_can_data_send(can_id, &can_data_send_buf[can_data_send_pkg_cur*8], 8);
            can_data_send_pkg_cur++;
        }
    }
}

void dh_can_cmd_bat_get(void)
{

}

void dh_can_cmd_wing_get(void)
{
    
}

void dh_can_cmd_mppt_ctrl(dh_can_cmd_mppt_t cmd)
{
    dh_gpio_1pluse(GPIO_PLUSE_MS, (uint16_t)cmd);
}

void dh_can_cmd_bat_ctrl(dh_can_cmd_bat_t cmd)
{
    switch (cmd)
    {
        case CAN_CMD_ENCHARGE_V_GEAR1:
            LL_GPIO_ResetOutputPin(VCHARGE_GEAR0_GPIO_Port, VCHARGE_GEAR0_Pin);
            LL_GPIO_ResetOutputPin(VCHARGE_GEAR1_GPIO_Port, VCHARGE_GEAR1_Pin);
            break;
        case CAN_CMD_ENCHARGE_V_GEAR2:
            LL_GPIO_SetOutputPin(VCHARGE_GEAR0_GPIO_Port, VCHARGE_GEAR0_Pin);
            LL_GPIO_ResetOutputPin(VCHARGE_GEAR1_GPIO_Port, VCHARGE_GEAR1_Pin);
            break; 
        case CAN_CMD_ENCHARGE_V_GEAR3:
            LL_GPIO_ResetOutputPin(VCHARGE_GEAR0_GPIO_Port, VCHARGE_GEAR0_Pin);
            LL_GPIO_SetOutputPin(VCHARGE_GEAR1_GPIO_Port, VCHARGE_GEAR1_Pin);
            break;     
        default:
            break;
    }
}

void dh_can_cmd_wing1_ctrl(dh_can_cmd_wing1_t cmd)
{

}

void dh_can_cmd_wing2_ctrl(dh_can_cmd_wing2_t cmd)
{

}
