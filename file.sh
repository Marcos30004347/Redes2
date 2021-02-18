# !/bin/bash

if [ -f ./teste.txt ]; then rm ./teste.txt; fi

for VARIABLE in {1..10000}
do
    echo $VARIABLE >> teste.txt
done