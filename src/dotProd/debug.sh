#!/bin/sh

export DEBUG=yes

for i in {IJK,IKJ,JKI}
do
    unset DOT_PROD
    printf "%s\n" $i
    make cleanBuild
    export DOT_PROD=$i
    make
    printf "\n----------------------------------------\n"
done
    unset DOT_PROD
    printf "%s\n" $i
    make cleanBuild
    export DOT_PROD_BLOCK=yes
    make


    printf "\n----------------------------------------\n"
    printf "\n========================================\n"
    printf "\n----------------------------------------\n"

BIN=bin

for FILE in `ls $BIN`
do
    EXEC=$BIN/$FILE
    $EXEC $1
    printf "\n----------------------------------------\n"
done
