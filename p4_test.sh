#!/bin/bash

# Test script for launching n instances of p4 with the arguments passed

rm -f endpoints *.txt
i=0
while [ $i -lt $1 ]
do
  ./p4 $1 $2 $3 $4 $5 $6 $7 > $i.txt 2>&1 &
  echo "Instance $i launched"
  sleep 1
  i=$[$i+1]
done
