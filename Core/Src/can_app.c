#include "can_app.h"
#include "gpio.h"
#include "can.h"
#include "adc.h"
#include "string.h"

static uint8_t backup_data[BACKUP_DATA_LEN] = {0};

uint8_t dh_can_cmd_remote_req(uint8_t* data);
uint8_t dh_can_cmd_can_reset(uint8_t* data);
void dh_can_cmd_mppt_get(void);
void dh_can_cmd_bat_get(void);
void dh_can_cmd_wing_get(void);

uint8_t dh_can_cmd_short(uint8_t* data);
uint8_t dh_can_cmd_mppt_ctrl(dh_can_cmd_mppt_t cmd);
uint8_t dh_can_cmd_bat_ctrl(dh_can_cmd_bat_t cmd);
uint8_t dh_can_cmd_wing1_ctrl(dh_can_cmd_wing1_t cmd);
uint8_t dh_can_cmd_wing2_ctrl(dh_can_cmd_wing2_t cmd);
uint8_t dh_can_cmd_recv_boardcast_backupdata(uint8_t *data, uint8_t ft, uint8_t fc);

dh_can_data_t can_data;
uint8_t can_data_send_buf[MAX_APP_CAN_DATA_LEN];
volatile static uint8_t can_data_send_pkg_sum = 0;
volatile static uint8_t can_data_send_pkg_cur = 0;

dh_can_remote_head_t remote_data_head = {0};

void dh_can_data_cmd_process(uint32_t canid, uint8_t* data)
{
    uint8_t ret = 0;
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
            ret = dh_can_cmd_remote_req(data);
            if(ret == 0)
            {
                remote_data_head.cmd_cnt.error_cnt++;
            }
            break;
        case CAN_ID_DT_RESET:
            ret = dh_can_cmd_can_reset(data);
            if(ret)
            {
                remote_data_head.cmd_latest = data[0];
                remote_data_head.cmd_cnt.correct_cnt++;
            }
            else
            {
                remote_data_head.cmd_cnt.error_cnt++;
            }
            break;
        case CAN_ID_DT_SHORT_CMD:
            ret = dh_can_cmd_short(data);
            if(ret)
            {
                remote_data_head.cmd_latest = data[0];
                remote_data_head.cmd_cnt.correct_cnt++;
            }
            else
            {
                remote_data_head.cmd_cnt.error_cnt++;
            }
            break;
        case CAN_ID_DT_BACKUP_DATA_REQ:
            //remote_data_head.cmd_latest = data[0];//F5
            break;
        case CAN_ID_DT_LONG_CMD:
            break;
        case CAN_ID_DT_BACKUP_DATA_RET:
            //remote_data_head.cmd_latest = data[0];//EE
            break;
        case CAN_ID_DT_REMOTE_RET:
            break;
        case CAN_ID_DT_BOARDCAST_TIME:
            break;
        case CAN_ID_DT_BOARDCAST_PATH:
            break;
        case CAN_ID_DT_BOARDCAST_POSTURE:
            break;
        case CAN_ID_DT_BOARDCAST_BACKUP_DATA:
            ret = dh_can_cmd_recv_boardcast_backupdata(data, can_data.ft, can_data.fc);
            if(ret == 0)
            {
                remote_data_head.cmd_cnt.error_cnt++;
            }
            //remote_data_head.backup_data_cnt++;
            break;
        default:
            remote_data_head.cmd_cnt.error_cnt++;
            break;
    }
}

uint8_t dh_can_cmd_remote_req(uint8_t* data)
{
    uint8_t ret = 1;
    switch (data[0])
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
			ret = 0;
            break;
    }

    return ret;
}

uint8_t dh_can_cmd_can_reset(uint8_t* data)
{
    uint8_t ret = 1;
    switch (data[0])
    {
        case CAN_CMD_RESET_CAN12:
            break;

        case CAN_CMD_RESET_CAN1:
            break;

        case CAN_CMD_RESET_CAN2:
			__HAL_CAN_RESET_HANDLE_STATE(&hcan2);
            MX_CAN2_Init();
            break;  
     
        default:
            ret = 0;
            break;
    }

    return ret;
}

uint8_t dh_can_cmd_short(uint8_t* data)
{
    uint8_t ret = 0;
    switch ((dh_can_cmd_ctrl_t)data[0])
    {
        case CAN_CMD_CTRL_MPPT:
            ret = dh_can_cmd_mppt_ctrl((dh_can_cmd_mppt_t)data[1]);
            break;
        case CAN_CMD_CTRL_BAT:
            ret = dh_can_cmd_bat_ctrl((dh_can_cmd_bat_t)data[1]);
            break;
        case CAN_CMD_CTRL_WING1:
            ret = dh_can_cmd_wing1_ctrl((dh_can_cmd_wing1_t)data[1]);
            break;
        case CAN_CMD_CTRL_WING2:
            ret = dh_can_cmd_wing2_ctrl((dh_can_cmd_wing2_t)data[1]);
            break;                                
        default:
            break;
    }

    return ret;
}
//dh_can_mppt_inf_t mppt_data = {0};
void dh_can_cmd_mppt_get(void)
{
	uint8_t i, data_pos = 0;
    uint16_t data_len = 0, len_temp;
    uint32_t checksum = 0;
    dh_can_mppt_inf_t mppt_data = {0};

    if (LL_GPIO_IsInputPinSet(MPPT1_ST_GPIO_Port, MPPT1_ST_Pin))
    {
        mppt_data.mppt_st1.mppt1_status = 1;
        mppt_data.mppt_st1.mppt7_status = 1;

        mppt_data.mppt_st2.mppt9_status = 1;
        mppt_data.mppt_st2.mppt_reserve = 0;
    }

    for(i=0; i<MAX_ADC_CHN_NUM; i++)
    {
        mppt_data.value_u[i].data_z = 33 * adc_value_aver[i] / 40950;//计算整数部分
        mppt_data.value_u[i].data_f = ((33 * adc_value_aver[i]) % 40950)*100/40950;//计算小数部分
    }

    memset(can_data_send_buf, 0, MAX_APP_CAN_DATA_LEN);

    data_pos += sizeof(data_len);
    len_temp = sizeof(remote_data_head);
    remote_data_head.tmt = CAN_CMD_REMOTE_MPPT;
    remote_data_head.remote_cmd_cnt++;
    memcpy(&can_data_send_buf[data_pos], &remote_data_head, len_temp);
    data_pos += len_temp;

    len_temp = sizeof(mppt_data);
    memcpy(&can_data_send_buf[data_pos], &mppt_data, len_temp);
    data_pos += len_temp;

    data_pos += 1;//checksum

    len_temp = sizeof(data_len);
    data_len = data_pos - len_temp;
    memcpy(&can_data_send_buf[0], &data_len, len_temp);

    checksum += data_len;
    for(i = len_temp; i < (data_pos-1); i++)
    {
        checksum += can_data_send_buf[i];
    }

    can_data_send_buf[data_pos-1] = checksum&0xFF;

    can_data_send_pkg_sum = (data_pos - 1)/CAN_APP_DATA_UINT_LEN + 1;
    can_data_send_pkg_cur = 0;
}

void dh_can_cmd_bat_get(void)
{
	uint8_t i, data_pos = 0;
    uint16_t data_len = 0, len_temp;
    uint32_t checksum = 0;
    dh_can_bat_inf_t bat_data = {0};

    if (LL_GPIO_IsInputPinSet(MPPT1_ST_GPIO_Port, MPPT1_ST_Pin))
    {
        //bat_dat. &= ~(0x1<<0);
        bat_data.status.low_vol_protect = 1;
        bat_data.status.self_energizing = 0; 
        bat_data.status.discharge_status = 1;
        bat_data.status.reserve = 0;
    }

    bat_data.bus_u.data_z = 33 * adc_value_aver[0] / 40950;//计算整数部分;
    bat_data.bus_u.data_f = ((33 * adc_value_aver[0]) % 40950)*100/40950;//计算小数部分

    bat_data.bat_u.data_z = 33 * adc_value_aver[1] / 40950;//计算整数部分;
    bat_data.bat_u.data_f = ((33 * adc_value_aver[1]) % 40950)*100/40950;//计算小数部分

    bat_data.load_i_sum.data_z = 33 * adc_value_aver[0] / 40950;//计算整数部分;
    bat_data.load_i_sum.data_f = ((33 * adc_value_aver[0]) % 40950)*100/40950;//计算小数部分

    memset(can_data_send_buf, 0, MAX_APP_CAN_DATA_LEN);

    data_pos += sizeof(data_len);
    len_temp = sizeof(remote_data_head);
    remote_data_head.tmt = CAN_CMD_REMOTE_BAT;
    remote_data_head.remote_cmd_cnt++;
    memcpy(&can_data_send_buf[data_pos], &remote_data_head, len_temp);
    data_pos += len_temp;

    len_temp = sizeof(bat_data);
    memcpy(&can_data_send_buf[data_pos], &bat_data, len_temp);
    data_pos += len_temp;

    data_pos += 1;//checksum

    len_temp = sizeof(data_len);
    data_len = data_pos - len_temp;
    memcpy(&can_data_send_buf[0], &data_len, len_temp);

    checksum += data_len;
    for(i = len_temp; i < (data_pos-1); i++)
    {
        checksum += can_data_send_buf[i];
    }

    can_data_send_buf[data_pos-1] = checksum&0xFF;

    can_data_send_pkg_sum = (data_pos - 1)/CAN_APP_DATA_UINT_LEN + 1;
    can_data_send_pkg_cur = 0;
}

void dh_can_cmd_wing_get(void)
{
	uint8_t i, data_pos = 0;
    uint16_t data_len = 0, len_temp;
    uint32_t checksum = 0;
    dh_can_wing_inf_t wing_data = {0};

    if (LL_GPIO_IsInputPinSet(MPPT1_ST_GPIO_Port, MPPT1_ST_Pin))
    {
        wing_data.status1.qv_a = 1;
        wing_data.status1.phase_ar_sda = 1;
        wing_data.status2.flyx = 1;
        wing_data.status2.board_lock_status = 1;

    }

    memset(can_data_send_buf, 0, MAX_APP_CAN_DATA_LEN);

    data_pos += sizeof(data_len);
    len_temp = sizeof(remote_data_head);
    remote_data_head.tmt = CAN_CMD_REMOTE_WING;
    remote_data_head.remote_cmd_cnt++;
    memcpy(&can_data_send_buf[data_pos], &remote_data_head, len_temp);
    data_pos += len_temp;

    len_temp = sizeof(wing_data);
    memcpy(&can_data_send_buf[data_pos], &wing_data, len_temp);
    data_pos += len_temp;

    data_pos += 1;//checksum

    len_temp = sizeof(data_len);
    data_len = data_pos - len_temp;
    memcpy(&can_data_send_buf[0], &data_len, len_temp);

    checksum += data_len;
    for(i = len_temp; i < (data_pos-1); i++)
    {
        checksum += can_data_send_buf[i];
    }

    can_data_send_buf[data_pos-1] = checksum&0xFF;

    can_data_send_pkg_sum = (data_pos - 1)/CAN_APP_DATA_UINT_LEN + 1;
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
        dh_set_canid(&can_id, CAN_ID_DT, CAN_ID_DT_REMOTE_RET, CAN_ID_LT-CAN_ID_DT);
        dh_set_canid(&can_id, CAN_ID_LT, CAT_ID_LT_BUSB, CAN_ID_P-CAN_ID_LT);
        dh_set_canid(&can_id, CAN_ID_P, CAT_ID_P_SUB, MAX_CAN_ID_BIT_LEN-CAN_ID_P);

        dh_set_canid(&can_id, CAN_ID_FC, can_data_send_pkg_cur+1, CAN_ID_FT-CAN_ID_FC);
        if(can_data_send_pkg_cur == 0)
        {
            dh_set_canid(&can_id, CAN_ID_FT, CAN_ID_FT_MULTI_FISRT, CAN_ID_SA-CAN_ID_FT);
            dh_can_data_send(can_id, &can_data_send_buf[can_data_send_pkg_cur*CAN_APP_DATA_UINT_LEN], CAN_APP_DATA_UINT_LEN);
            can_data_send_pkg_cur++;
        }
        else if(can_data_send_pkg_cur == (can_data_send_pkg_sum - 1))
        {
            dh_set_canid(&can_id, CAN_ID_FT, CAN_ID_FT_MULTI_END, CAN_ID_SA-CAN_ID_FT);
            dh_can_data_send(can_id, &can_data_send_buf[can_data_send_pkg_cur*CAN_APP_DATA_UINT_LEN], CAN_APP_DATA_UINT_LEN);
            can_data_send_pkg_sum = 0;
            can_data_send_pkg_cur = 0;
        }
        else
        {
            dh_set_canid(&can_id, CAN_ID_FT, CAN_ID_FT_MULTI_MID, CAN_ID_SA-CAN_ID_FT);
            dh_can_data_send(can_id, &can_data_send_buf[can_data_send_pkg_cur*CAN_APP_DATA_UINT_LEN], CAN_APP_DATA_UINT_LEN);
            can_data_send_pkg_cur++;
        }
    }
}


uint8_t dh_can_cmd_mppt_ctrl(dh_can_cmd_mppt_t cmd)
{
    if(cmd <= CAN_CMD_MPPT9_ST_1)
    {
        dh_gpio_1pluse(GPIO_PLUSE_MS, (uint16_t)cmd);
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t dh_can_cmd_bat_ctrl(dh_can_cmd_bat_t cmd)
{
    uint8_t ret = 1;
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
            ret = 0;
            break;
    }

    return ret;
}

uint8_t dh_can_cmd_wing1_ctrl(dh_can_cmd_wing1_t cmd)
{
	return 0;
}

uint8_t dh_can_cmd_wing2_ctrl(dh_can_cmd_wing2_t cmd)
{
	return 0;
}

uint8_t dh_can_cmd_recv_boardcast_backupdata(uint8_t *data, uint8_t ft, uint8_t fc)
{
    uint8_t i, checksum = 0, len_temp;
    uint8_t* p = 0;
    static dh_can_backup_data_t data_t =
    {
        .datalen = INVAULD_LEN_16,
        .checksum = 0,
    };

    p = (uint8_t*)(&data_t);

    fc--;
    if(fc>(BACKUP_DATA_LEN/CAN_APP_DATA_UINT_LEN))
    {
        goto ERR_DATA_RBB;
    }

    if(ft == CAN_ID_FT_MULTI_FISRT)
    {
        if(data_t.datalen != INVAULD_LEN_16)
        {
            goto ERR_DATA_RBB;
        }
        memcpy((p+CAN_APP_DATA_UINT_LEN*fc), data, CAN_APP_DATA_UINT_LEN);
        len_temp = CAN_APP_DATA_UINT_LEN - sizeof(data_t.datalen);
        if(data_t.datalen >= len_temp)
        {
            data_t.datalen -= len_temp;
        }
        else
        {
            goto ERR_DATA_RBB;
		}
    }
    else if(ft == CAN_ID_FT_MULTI_END)
    {
        len_temp = sizeof(dh_can_backup_data_t) - fc*CAN_APP_DATA_UINT_LEN;
        if(data_t.datalen == INVAULD_LEN_16 || data_t.datalen < len_temp)
        {
            goto ERR_DATA_RBB;
        }
        memcpy((p+CAN_APP_DATA_UINT_LEN*fc), data, len_temp);
				
        data_t.datalen-=len_temp;
    }
    else if(ft == CAN_ID_FT_MULTI_MID)
    {
        if(data_t.datalen == INVAULD_LEN_16  || data_t.datalen < CAN_APP_DATA_UINT_LEN)
        {
            goto ERR_DATA_RBB;
        }
        memcpy((p+CAN_APP_DATA_UINT_LEN*fc), data, CAN_APP_DATA_UINT_LEN);
        data_t.datalen -= CAN_APP_DATA_UINT_LEN;
    }
    else
    {
        goto ERR_DATA_RBB;
    }

    if(data_t.datalen == 0)
    {
        data_t.datalen = INVAULD_LEN_16;
        len_temp  = sizeof(data_t.datalen);
        for(i = 0; i < BACKUP_DATA_LEN; i++)
        {
            checksum += *(p+i+len_temp);
        }

        if(checksum != data_t.checksum)
        {
            goto ERR_DATA_RBB;
        }
        else
        {
            memcpy(backup_data, &data_t.data, BACKUP_DATA_LEN);
            remote_data_head.backup_data_cnt++;
        }
    }

    return 1;

ERR_DATA_RBB:
     data_t.datalen = INVAULD_LEN_16;
     return 0;
}
