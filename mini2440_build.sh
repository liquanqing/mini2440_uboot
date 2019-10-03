#!/bin/sh


if [ "$1" = "build" ];then
echo "only build begin"
make ARCH=arm CROSS_COMPILE=arm-s3c2440-linux-gnueabi- V=S
echo "######################"
echo "     script end       "
echo "######################"
exit
fi

read -p "input default board(etc.mini2440)" board
echo "make ${board}_config"

if [ ! -n "$board" ];then
	echo "no board to config : etc. mini2440"
	exit 1
fi
make ${board}_config

read -p "go to menuconfig ?(y or n default:n) " cmd_menuconfig
echo ${cmd_menuconfig}

if [ ! -n "$cmd_menuconfig" ];then
	cmd_menuconfig=n
else
	cmd_menuconfig=${cmd_menuconfig}
fi

if [ ${cmd_menuconfig} = y ];then
	echo "make menuconfig"
	make ARCH=arm CROSS_COMPILE=arm-s3c2440-linux-gnueabi- menuconfig
fi

read -p "start build ?(y or n default:n) " cmd_build
echo ${cmd_build}

if [ ! -n "$cmd_build" ];then
	cmd_build=n
fi

if [ ${cmd_build} = y ];then
	echo "build begin"
	make ARCH=arm CROSS_COMPILE=arm-s3c2440-linux-gnueabi-
fi

echo "######################"
echo "     script end       "
echo "######################"