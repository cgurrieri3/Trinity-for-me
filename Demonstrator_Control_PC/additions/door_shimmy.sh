#!/bin/bash  
echo "Shimmy Door"

# Set default value for -dt option
dt_value=1

while true; do
  echo "------------------"
  echo "Starting door down"
  echo "------------------"
  uswitch -dir down -dt "$dt_value"
  echo "---------"
  echo "Door down"
  echo "---------"
  sleep 10
  echo "----------------"
  echo "Starting door up"
  echo "----------------"
  uswitch -dir up -dt "$dt_value" 
  echo "-------" 
  echo "Door up"
  echo "-------"
  sleep 10
done
