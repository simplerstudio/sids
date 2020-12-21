#!/bin/bash

echo "*****************************************************************"
echo "*****************************  SIDS *****************************"
echo "************* MOXA GATEWAY AUTO INSTALL PROCESSING START ********"
echo "*****************************************************************"
echo " 								       "
sudo rm -f /etc/ssh/sshd_config
sudo rm -f /etc/network/interfaces
sudo rm -f /etc/rc.local
sudo cp  sshd_config /etc/ssh/sshd_config

sudo cp  interfaces /etc/network/interfaces
sudo ifdown eth1
sudo ifup eth1

sudo chmod 777 sids_server
sudo chmod 777 sids_client
sudo chmod 777 sids_dio
sudo chmod 777 stop.sh
sudo chmod 777 start.sh
sudo chmod 777 rc.local
sudo chmod 777 client_restart.sh
sudo chmod 777 watchDog.sh
sudo chmod 777 client_auto.sh

sudo cp sids_server /root
sudo cp sids_client /root
sudo cp sids_dio /root
sudo cp start.sh /root
sudo cp stop.sh /root
sudo cp watchDog.sh /root
sudo cp client_restart.sh /root
sudo cp tcp_config.txt /root
sudo cp interval.txt /root
sudo cp rtu_config.txt /root
sudo cp port_setting.txt /root
sudo cp client_auto.sh /root
sudo cp dio_auto.sh /root

sudo cp  rc.local /etc/

echo "                                                             "
echo "*************************************************************"
echo "******************** FINISH TO INSTALL **********************"
echo "******************* MOXA  WAS REBOOTED **********************"
echo "*************************************************************"
sudo reboot


