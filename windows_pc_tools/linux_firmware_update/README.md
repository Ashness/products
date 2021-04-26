目前只支持 amd64 ubuntu固件命令行式下载 升级固件

1. 插入linux，使用 ls/dev/ttyU* 查看TTY串口设备是否已经连接上，并且加可读写权限。
2. 打开download_amd64.sh文件，修改其中的tty=/dev/ttyUSB0,改成您的ttyUSB端口号
3. 运行./download_amd64.sh即可。



