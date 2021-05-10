file=hi216_next.bin
tty=/dev/ttyUSB0


ls /dev/ttyU*
sudo chmod 777 $tty
sudo chmod 777 ./linux/armv7l/blhost

stty -F $tty raw speed 115200 -echo min 0 time 5
echo "AT+RST" > $tty
echo 0D | xxd -r -ps > $tty
#sleep 0.01
echo 5A A6 | xxd -r -ps > $tty


./blhost-linux-armv7l -p $tty,115200 get-property 1
./blhost-linux-armv7l -p $tty,115200 flash-erase-region 0x1000 0x6C00
./blhost-linux-armv7l -p $tty,115200 write-memory 0x1000 $file
./blhost-linux-armv7l -p $tty,115200 reset
