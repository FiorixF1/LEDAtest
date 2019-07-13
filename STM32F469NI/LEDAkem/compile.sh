#!/bin/bash

for i in 1 2 4
do
    for j in 2 3 4
    do
        make SL=$i N0=$j
        cp ./bin/libLEDAkem_sl$i\_N0$j.a ./include/libLEDAkem_sl$i\_N0$j.a
        cp ./bin/libLEDAkem_sl$i\_N0$j.a ../Inc/libLEDAkem_sl$i\_N0$j.a
        make clean SL=$i N0=$j
    done
done
