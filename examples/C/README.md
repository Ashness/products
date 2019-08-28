C语言数据接收程序，与硬件无关，可以轻松移植到任意平台上。包括ARM CortexM 系列 ubtantu系列

1. `packet.c` `packet.h` 是数据拆包单元负责 将一帧数据接收并且做CRC校验。但不会处理一帧中的数据(payload)部分。`packet.c`, `packet.h`没有任何依赖文件。
2. `imu_data_decode.c/h` 会调用`packet.c/h` 专门负责一帧中数据接收的部分



### 使用方法：
在`imu_data_decode.c` 中的 `imu_data_decode_init` 函数上方注释中 详细介绍了使用方法：

1. 在main中调用 `imu_data_decode_init()` 完成解码器初始化
2. 当接收到一个串口字符时：调用`Packet_Decode`。 每接收到一个字符，都调用一次`Packet_Decode`。 

```
//your UART interrupt function when received a char
void UART_ISR(void)
{
    uint8_t ch;

    // get a UART char 
    ch = uart_read();
    
    Packet_Decode(ch);
}
```



3. 在循环中 调用API来获取当前帧中数据，并加入适当延时函数以免显示过快：

```
    while(1)
    {
        get_raw_acc(Acc);
        get_raw_gyo(Gyo);
        get_raw_mag(Mag);
        get_eular(Eular);
        get_quat(Quat);
        get_id(&ID);
        
    //printf("id:%d\r\n", ID);

    printf("Acc: %d %d %d\r\n", Acc[0], Acc[1], Acc[2]);
    printf("Gyo: %d %d %d\r\n", Gyo[0], Gyo[1], Gyo[2]);
    printf("Mag: %d %d %d\r\n", Mag[0], Mag[1], Mag[2]);
    printf("Eular(P R Y):    %0.2f %0.2f %0.2f\r\n", Eular[0], Eular[1], Eular[2]);
    printf("Quat(W X Y Z):   %0.3f %0.3f %0.3f %0.3f\r\n", Quat[0], Quat[1], Quat[2], Quat[3]);
    
    //delay a while 
    delay();
     
    }
```

