#!/bin/bash

for i in 1 2 4
do
    for j in 2 3 4
    do
        gcc main.c -m32 -o 32_SL$i\_N0$j -DCATEGORY=$i -DN0=$j -Wall -fdata-sections -ffunction-sections -std=c11 -g3 -gdwarf-2 -L./KEM32/include -IKEM32/include -Wl,--start-group -lgcc -lc -lm -Wl,--end-group ./KEM32/bin/libLEDAkem_sl$i\_N0$j.a ./KEM32/include/libkeccak.a
        
        #gcc main.c -o 64_SL$i\_N0$j -DCATEGORY=$i -DN0=$j -Wall -fdata-sections -ffunction-sections -std=c11 -g3 -gdwarf-2 -L./KEM64/include -IKEM64/include -Wl,--start-group -lgcc -lc -lm -Wl,--end-group ./KEM64/bin/libLEDAkem_sl$i\_N0$j.a ./KEM64/include/libkeccak.a
        
    done
done





