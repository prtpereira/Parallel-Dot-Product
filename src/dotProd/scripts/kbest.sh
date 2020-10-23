#!/bin/sh

kbest(){
    EXEC=$1
    ARGS=$2

    KBEST=3
    TOTAL=8

    for i in `seq $TOTAL`
    do 
        $EXEC $ARGS
    done  | sort -n | head -n $KBEST  | awk '{sum+=$1} END {print sum/NR}'
}



