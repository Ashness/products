#include "delay.h"
#include "usart.h"
#include "ch_serial.h"
 
/************************************************
 ALIENTEK战舰STM32开发板实验
 串口实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

/************************************************
 北京超核电子
 惯性导航模块： HI226 HI229
 串口接收数据例程
 本例程只供学习使用，观察数据输出，不做其他用途
 串口2接收来自HI226或者是HI229的数据
 串口1将串口2成功接收到的数据打印到终端上
 这里的终端一般指的是PC机上串口调试助手
 官网：http://www.hipnuc.com
************************************************/

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))

static raw_t raw = {0};                                         /* IMU stram read/control struct */
static uint8_t decode_succ;                               /* 0: no new frame arrived, 1: new frame arrived */

typedef struct
{
    uint8_t code;
    char    name[8];
}item_code_name_t;

const item_code_name_t item_code_name[] = 
{
    {0x90, "id"},
    {0xA0, "acc"},
    {0xB0, "gyr"},
    {0xC0, "mag"},
    {0xD0, "eul"},
    {0xD1, "quat"},
    {0xF0, "pressure"},
    {0x91, "IMUSOL"},   /* collection of acc,gyr,mag,eul etc. to replace A0,B0,C0,D0... see user manual*/
    {0x60, "GWSOL"},    /* HI221 node imu data collection  see user manual */
};

static const char *code2name(uint8_t code)
{
    const char *p = NULL;
    int i;
    for(i=0; i<ARRAY_SIZE(item_code_name); i++)
    {
        if(code == item_code_name[i].code)
        {
            p = item_code_name[i].name;
        }
    }
    return p;
}

static void dump_imu_data(raw_t *raw)
{
    int i;
    if(raw->item_code[0] != KItemGWSOL) /* HI226 HI229 CH100 CH110 */  
    {
        printf("%-16s%d\r\n",       "id:",  raw->imu[0].id);
        printf("%-16s%.3f %.3f %.3f\r\n",       "acc(G):",        raw->imu[0].acc[0], raw->imu[0].acc[1],  raw->imu[0].acc[2]);
        printf("%-16s%.3f %.3f %.3f\r\n",       "gyr(deg/s):",    raw->imu[0].gyr[0], raw->imu[0].gyr[1],  raw->imu[0].gyr[2]);
        printf("%-16s%.3f %.3f %.3f\r\n",       "mag(uT):",       raw->imu[0].mag[0], raw->imu[0].mag[1],  raw->imu[0].mag[2]);
        printf("%-16s%.3f %.3f %.3f\r\n",       "eul(deg):",      raw->imu[0].eul[0], raw->imu[0].eul[1],  raw->imu[0].eul[2]);
        printf("%-16s%.3f %.3f %.3f %.3f\r\n",  "quat:",          raw->imu[0].quat[0], raw->imu[0].quat[1],  raw->imu[0].quat[2], raw->imu[0].quat[3]);
        printf("%-16s%.3f\r\n",       "presure(pa):",  raw->imu[0].pressure);
        printf("%-16s%d\r\n",       "timestamp(ms):",  raw->imu[0].timestamp);
        printf("item: ");
        for(i=0; i<raw->nitem_code; i++)
        {
            printf("0x%02X(%s)", raw->item_code[i], code2name(raw->item_code[i]));
        }
        printf("\r\n");
    }
    else /* HI222(HI221GW) */
    {
        putchar(10);
        printf("gateway: %s%d, %s%d\r\n",       "gwid:",      raw->gwid, "node cnt:", raw->nimu);
        for(i=0; i<raw->nimu; i++)
        {
            putchar(10);
            printf("%-16s%d\r\n",       "id:",  raw->imu[i].id);
            printf("%-16s%.3f %.3f %.3f\r\n",       "acc(G):",        raw->imu[i].acc[0], raw->imu[i].acc[1],  raw->imu[i].acc[2]);
            printf("%-16s%.3f %.3f %.3f\r\n",       "gyr(deg/s):",    raw->imu[i].gyr[0], raw->imu[i].gyr[1],  raw->imu[i].gyr[2]);
            printf("%-16s%.3f %.3f %.3f\r\n",       "mag(uT):",       raw->imu[i].mag[0], raw->imu[i].mag[1],  raw->imu[i].mag[2]);
            printf("%-16s%.3f %.3f %.3f\r\n",       "eul(deg):",      raw->imu[i].eul[0], raw->imu[i].eul[1],  raw->imu[i].eul[2]);
            printf("%-16s%.3f %.3f %.3f %.3f\r\n",  "quat:",          raw->imu[i].quat[0], raw->imu[i].quat[1],  raw->imu[i].quat[2], raw->imu[i].quat[3]);
            printf("%-16s%.3f\r\n",       "presure(pa):",  raw->imu[i].pressure);
            printf("%-16s%d\r\n",       "timestamp(ms):",  raw->imu[i].timestamp);
        }
    }
}


int main(void)
{		
    delay_init();	    	                               //延时函数初始化	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);        //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    uart_init(115200);	                                   //串口初始化为115200

    while(1)
    {
        if(decode_succ)
        {
            decode_succ = 0;
            delay_ms(800);
            dump_imu_data(&raw);
        }
	}	 
}

void USART2_IRQHandler(void)
{
	uint8_t ch;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        ch = USART_ReceiveData(USART2);	
    }
    
    /* decode each byte */
    decode_succ = ch_serial_input(&raw, ch);
} 
