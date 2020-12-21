#!/bin/bash

echo "1. Stop Modbus Program."
sleep 1 
echo "2. Check & Kill running process."
/bin/kill -9 `ps x | grep sids_client | grep -v grep| awk '{print $1}'` &> /dev/null
sleep 1
/bin/kill -9 `ps x | grep sids_server | grep -v grep| awk '{print $1}'` &> /dev/null
sleep 1
/bin/kill -9 `ps x | grep sids_dio | grep -v grep| awk '{print $1}'` &> /dev/null
sleep 1
/bin/kill -9 `ps x | grep watchDog | grep -v grep| awk '{print $1}'` &> /dev/null
echo "3. Done."




