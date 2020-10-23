#!/bin/sh

unset PAPI
unset DOT_PROD
unset DOT_PROD_BLOCK

for i in {IJK,IKJ,JKI}
do
    printf "%s\n" $i
    make cleanBuild
    export DOT_PROD=$i
    make
    unset PAPI
    unset DOT_PROD
    printf "\n----------------------------------------\n"
done

for i in {IJK,IKJ,JKI}
do
    printf "papi %s\n" $i
    make cleanBuild
    export DOT_PROD=$i
    export PAPI=yes
    make
    unset PAPI
    unset DOT_PROD
    printf "\n----------------------------------------\n"
done

# Block 
printf "BLOCK\n"
make cleanBuild
export DOT_PROD_BLOCK=yes
make
unset DOT_PROD_BLOCK

printf "\n----------------------------------------\n"

# PAR 
printf "PAR\n"
make cleanBuild
export DOT_PROD_BLOCK=yes
make
unset DOT_PROD_BLOCK

printf "\n----------------------------------------\n"

# KNC
printf "KNC\n"
make cleanBuild
export DOT_PROD_KNC=yes
make
unset DOT_PROD_KNC

printf "\n----------------------------------------\n"

# KNL
printf "KNL\n"
make cleanBuild
export DOT_PROD_KNL=yes
make
unset DOT_PROD_KNL
