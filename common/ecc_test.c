
#include <stdio.h>
#include "ecc_test.h"
#include "flash.h"
#include "stdlib.h"
#include "string.h"
#include "boot.h"

int nand_calculate_ecc(uint8_t *dat, uint8_t *ecc_code);

//固定的ECC校验数组TABLE
static const uint8_t nand_ecc_precalc_table[ECC_CHECK_SIZE] = {
    0x00,0x55,0x56,0x03,0x59,0x0C,0x0F,0x5A,0x5A,0x0F,0x0C,0x59,0x03,0x56,0x55,0x00,
    0x65,0x30,0x33,0x66,0x3C,0x69,0x6A,0x3F,0x3F,0x6A,0x69,0x3C,0x66,0x33,0x30,0x65,
    0x66,0x33,0x30,0x65,0x3F,0x6A,0x69,0x3C,0x3C,0x69,0x6A,0x3F,0x65,0x30,0x33,0x66,
    0x03,0x56,0x55,0x00,0x5A,0x0F,0x0C,0x59,0x59,0x0C,0x0F,0x5A,0x00,0x55,0x56,0x03,
    0x69,0x3C,0x3F,0x6A,0x30,0x65,0x66,0x33,0x33,0x66,0x65,0x30,0x6A,0x3F,0x3C,0x69,
    0x0C,0x59,0x5A,0x0F,0x55,0x00,0x03,0x56,0x56,0x03,0x00,0x55,0x0F,0x5A,0x59,0x0C,
    0x0F,0x5A,0x59,0x0C,0x56,0x03,0x00,0x55,0x55,0x00,0x03,0x56,0x0C,0x59,0x5A,0x0F,
    0x6A,0x3F,0x3C,0x69,0x33,0x66,0x65,0x30,0x30,0x65,0x66,0x33,0x69,0x3C,0x3F,0x6A,
    0x6A,0x3F,0x3C,0x69,0x33,0x66,0x65,0x30,0x30,0x65,0x66,0x33,0x69,0x3C,0x3F,0x6A,
    0x0F,0x5A,0x59,0x0C,0x56,0x03,0x00,0x55,0x55,0x00,0x03,0x56,0x0C,0x59,0x5A,0x0F,
    0x0C,0x59,0x5A,0x0F,0x55,0x00,0x03,0x56,0x56,0x03,0x00,0x55,0x0F,0x5A,0x59,0x0C,
    0x69,0x3C,0x3F,0x6A,0x30,0x65,0x66,0x33,0x33,0x66,0x65,0x30,0x6A,0x3F,0x3C,0x69,
    0x03,0x56,0x55,0x00,0x5A,0x0F,0x0C,0x59,0x59,0x0C,0x0F,0x5A,0x00,0x55,0x56,0x03,
    0x66,0x33,0x30,0x65,0x3F,0x6A,0x69,0x3C,0x3C,0x69,0x6A,0x3F,0x65,0x30,0x33,0x66,
    0x65,0x30,0x33,0x66,0x3C,0x69,0x6A,0x3F,0x3F,0x6A,0x69,0x3C,0x66,0x33,0x30,0x65,
    0x00,0x55,0x56,0x03,0x59,0x0C,0x0F,0x5A,0x5A,0x0F,0x0C,0x59,0x03,0x56,0x55,0x00
};

/*
以APP_BK0作为ECC纠错对象区域
每256字节APP_BK0的数据对应生成3字节ECC校验数据
APP_BK0 共128K字节数据对应生成128*1024/256*3 = 1536字节的ECC校验数据
12*128 = 1536,, 分128次写进去FLASH_ECC_INF_ADDR
*/
void dh_create_app_bk_ecc_process(void)
{
	uint32_t i = 0, j = 0, z = 0;
	uint32_t current_addr = 0;
    uint8_t app_main_raw_data[ECC_CHECK_SIZE] = {0};
    uint8_t ecc_sum_buf[12] = {0};

    i = 0;
    j = 0;
    for(i = 0; i < 128; i++)
    {
        z = 0;
        j = 0;

        while(j < sizeof(ecc_sum_buf))
        {
            current_addr = FLASH_APP_BK0_ADDR + (4*i + z) * ECC_CHECK_SIZE;
            dh_read_flash_data(current_addr, app_main_raw_data, ECC_CHECK_SIZE);
            nand_calculate_ecc(app_main_raw_data, &ecc_sum_buf[j]);
            j += 3;
            z++;
        }

        dh_write_flash_data(FLASH_ECC_INF_ADDR + sizeof(ecc_sum_buf)*i, ecc_sum_buf, sizeof(ecc_sum_buf));
    }
}

/**
dat  - 输入256字节正确的原始数据
ecc_code  - 计算输出3字节的数据对应的正确的ecc纠错码
 * */
int nand_calculate_ecc(uint8_t *dat, uint8_t *ecc_code)
{
    uint8_t idx, reg1, reg2, reg3, tmp1, tmp2;
    int i;
    
    /* Initialize variables */
    reg1 = reg2 = reg3 = 0;

    /* Build up column parity */
    for(i = 0; i < ECC_CHECK_SIZE; i++) {
        /* Get CP0 - CP5 from table */
        idx = nand_ecc_precalc_table[*dat++];

        reg1 ^= (idx & 0x3f);

        /* All bit XOR = 1 ? */
        if (idx & 0x40) {
            reg3 ^= (uint8_t) i;
            reg2 ^= ~((uint8_t) i);
        }
    }

    /* Create non-inverted ECC code from line parity */
    tmp1  = (reg3 & 0x80) >> 0; /* B7 -> B7 */
    tmp1 |= (reg2 & 0x80) >> 1; /* B7 -> B6 */
    tmp1 |= (reg3 & 0x40) >> 1; /* B6 -> B5 */
    tmp1 |= (reg2 & 0x40) >> 2; /* B6 -> B4 */
    tmp1 |= (reg3 & 0x20) >> 2; /* B5 -> B3 */
    tmp1 |= (reg2 & 0x20) >> 3; /* B5 -> B2 */
    tmp1 |= (reg3 & 0x10) >> 3; /* B4 -> B1 */
    tmp1 |= (reg2 & 0x10) >> 4; /* B4 -> B0 */

    tmp2  = (reg3 & 0x08) << 4; /* B3 -> B7 */
    tmp2 |= (reg2 & 0x08) << 3; /* B3 -> B6 */
    tmp2 |= (reg3 & 0x04) << 3; /* B2 -> B5 */
    tmp2 |= (reg2 & 0x04) << 2; /* B2 -> B4 */
    tmp2 |= (reg3 & 0x02) << 2; /* B1 -> B3 */
    tmp2 |= (reg2 & 0x02) << 1; /* B1 -> B2 */
    tmp2 |= (reg3 & 0x01) << 1; /* B0 -> B1 */
    tmp2 |= (reg2 & 0x01) << 0; /* B7 -> B0 */

    
    /* Calculate final ECC code */
    ecc_code[0] = ~tmp2;
    ecc_code[1] = ~tmp1;
    ecc_code[2] = ((~reg1) << 2) | 0x03;


    return 0;
}

static inline int countbits(uint32_t byte)
{
    int res = 0;
    
    for (;byte; byte >>= 1)
        res += byte & 0x01;
    return res;
}

/*
read_ecc - 正确数据ECC
calc_ecc - 需纠错数据的ECC
data - 输入需被纠错的数据
若数据有错，该函数每256字节最多只能纠错1bit，其他情况都算是错误
*/
int nand_correct_data( uint8_t *read_ecc, uint8_t *calc_ecc, uint8_t *data)
{
    uint8_t s0, s1, s2;
    uint32_t byteoffs, bitnum;
    s0 = calc_ecc[0] ^ read_ecc[0];
    s1 = calc_ecc[1] ^ read_ecc[1];
    s2 = calc_ecc[2] ^ read_ecc[2];

    if ((s0 | s1 | s2) == 0)
        return 0;//no err
    
    /* Check for a single bit error */
    if( ((s0 ^ (s0 >> 1)) & 0x55) == 0x55 &&
        ((s1 ^ (s1 >> 1)) & 0x55) == 0x55 &&
        ((s2 ^ (s2 >> 1)) & 0x54) == 0x54) {
        
            //跳进来这里，有可能是1bit错误，也可能是多bit错误
            //如果是只有1bit错误，跳进来这里是可以纠错的，如果是多bit跳进来，则会纠错无效。
            byteoffs = (s1 << 0) & 0x80;
            byteoffs |= (s1 << 1) & 0x40;
            byteoffs |= (s1 << 2) & 0x20;
            byteoffs |= (s1 << 3) & 0x10;
            
            byteoffs |= (s0 >> 4) & 0x08;
            byteoffs |= (s0 >> 3) & 0x04;
            byteoffs |= (s0 >> 2) & 0x02;
            byteoffs |= (s0 >> 1) & 0x01;
            
            bitnum = (s2 >> 5) & 0x04;
            bitnum |= (s2 >> 4) & 0x02;
            bitnum |= (s2 >> 3) & 0x01;

            data[byteoffs] ^= (1 << bitnum);

            return 1;
    }
    
    if(countbits(s0 | ((uint32_t)s1 << 8) | ((uint32_t)s2 <<16)) == 1)
        return 1;
    
    return -1;//2 bit err 
}

dh_flash_check_t ecc_correct_process(void)
{
	uint8_t data_buf[ECC_CHECK_SIZE] = {0};
    uint8_t ecc_sum_real[ECC_CODE_LEN] = {0};
    uint8_t ecc_sum_new[ECC_CODE_LEN] = {0};
    uint32_t i = 0, j = 0, left_sz = 0;
    uint32_t current_addr = 0, app_checksum = 0;
    uint32_t loop_v = 0;
	
    //纠错流程，循环读取256字节进行纠错
    loop_v = FLASH_APP_BK0_SIZE/ECC_CHECK_SIZE;
    for(i = 0; i < loop_v; i++)
    {
        dh_read_flash_data(FLASH_APP_BK0_ADDR + i*ECC_CHECK_SIZE, data_buf, ECC_CHECK_SIZE);
        dh_read_flash_data(FLASH_ECC_INF_ADDR + i*ECC_CODE_LEN, ecc_sum_real, ECC_CODE_LEN);
        nand_calculate_ecc( data_buf, ecc_sum_new);
        if(nand_correct_data( ecc_sum_real, ecc_sum_new , data_buf) == 1)
        {
            dh_write_flash_data(FLASH_APP_BK0_ADDR  + i*ECC_CHECK_SIZE, data_buf, ECC_CHECK_SIZE);
        }
    }

    //纠错完，重新读取检查正确性
    dh_read_flash_data(FLASH_APP_INF0_ADDR, (uint8_t*)&current_app_inf, sizeof(current_app_inf));
    if(current_app_inf.app_hex_size  >= FLASH_APP_MAIN_SIZE || current_app_inf.app_hex_size  == 0)
    {
        return DH_FLASH_ERR;
    }

    i = 0;
    current_addr = FLASH_APP_BK0_ADDR;
    while(i < current_app_inf.app_hex_size)
    {
        left_sz = current_app_inf.app_hex_size - i;
        memset(data_buf, 0xff, sizeof(data_buf));
        if(left_sz < sizeof(data_buf))
        {
            dh_read_flash_data(current_addr+i, data_buf, left_sz);
        }
        else
        {
            left_sz = sizeof(data_buf);
            dh_read_flash_data(current_addr+i, data_buf, left_sz);
        }

        for(j = 0; j < left_sz; j++)
        {
            app_checksum += data_buf[j];
        }

        i += left_sz;

        if(app_checksum == current_app_inf.app_hex_checksum)
        {
            return DH_FLASH_NO_ERR;
        }
    }

    return DH_FLASH_ERR;
}

void dh_ecc_repair_process(void)
{
    uint8_t app_status[FLASH_APP_NUM] = {0,1,0,0};//指定BK0数据区域是正确的，进行对其他区域进行修复
    if(DH_FLASH_NO_ERR == ecc_correct_process())
    {
        repair_the_appsection(app_status);
        dh_bootup_process_retry();
    }
}
