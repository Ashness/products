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


void SysTick_Handler(void); 
void SysTick_Init(void); 

static raw_t raw;                                         /* IMU stram read/control struct */
static uint8_t decode_succ;                               /* 0: no new frame arrived, 1: new frame arrived */

static void dump_imu_data(raw_t *raw)
{
    int i;
    if(raw->item_code[0] != KItemGWSOL) /* HI222(HI221GW) */
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
            printf("0x%02X ", raw->item_code[i]);
        }
        printf("\r\n");
    }
    else /* HI226 HI229 CH100 CH110 */
    {
        printf("gateway: %s%d, %s%d\r\n",       "gwid:",      raw->gwid, "node cnt:", raw->nimu);
        for(i=0; i<raw->nimu; i++)
        {
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

/* SysTick initialization */
void SysTick_Init(void)
{
    SysTick->LOAD = (float)SystemCoreClock / 40;             
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; 
}


