#!/bin/bash

echo "1. Restarting Client Program."
sleep 1 
echo "2. Check & Kill running process."
/bin/kill -9 `ps x | grep sids_client | grep -v grep| awk '{print $1}'` &> /dev/null
sleep 1
/bin/kill -9 `ps x | grep watchDog | grep -v grep| awk '{print $1}'` &> /dev/null
sleep 1
nohup /root/sids_client &
sleep 1
nohup /root/watchDog.sh &
sleep 1
echo "3. Done."




