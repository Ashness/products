file=atf413_hi229_b0_32k.hex
tty=/dev/ttyUSB0


ls /dev/ttyU*
sudo chmod 777 $tty
sudo chmod 777 ./linux/amd64/blhost

stty -F $tty raw speed 115200 -echo min 0 time 5
echo "AT+RST" > $tty
echo 0D | xxd -r -ps > $tty
#sleep 0.01
echo 5A A6 | xxd -r -ps > $tty


./blhost-linux-armv7l -p $tty,115200 get-property 1
./blhost-linux-armv7l -p $tty,115200 flash-erase-region 0x8000 0x18000
./blhost-linux-armv7l -p $tty,115200 write-memory 0x8000 $file
./blhost-linux-armv7l -p $tty,115200 reset
