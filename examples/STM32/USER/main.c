#include "delay.h"
#include "usart.h"
#include "ch_serial.h"
 
/************************************************
 ALIENTEKս��STM32������ʵ��
 ����ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

/************************************************
 �������˵���
 ���Ե���ģ�飺 HI226 HI229
 ���ڽ�����������
 ������ֻ��ѧϰʹ�ã��۲��������������������;
 ����2��������HI226������HI229������
 ����1������2�ɹ����յ������ݴ�ӡ���ն���
 ������ն�һ��ָ����PC���ϴ��ڵ�������
 ������http://www.hipnuc.com
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
    delay_init();	    	                               //��ʱ������ʼ��	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);        //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    uart_init(115200);	                                   //���ڳ�ʼ��Ϊ115200

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


