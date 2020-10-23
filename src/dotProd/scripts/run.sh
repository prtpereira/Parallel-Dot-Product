#!/bin/sh

. ./scripts/kbest.sh

BIN=bin
FILE=time.csv


# node 662
# l1 32k    -> 51,6
# l2 256k   -> 146,1
# l3 30720K -> 1600


printf "variation,L1,L2,L3,RAM\n"
for FILE in `ls $BIN`
do
    printf "%s" $FILE
    #for SIZE in {48,128,1024,2048}
    for SIZE in {50,145,1500,2000}
    do
        EXEC=$BIN/$FILE
        TIME=$(kbest "$EXEC" "$SIZE")
        printf ",%lf" $TIME
    done
    printf "\n"
done
