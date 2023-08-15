#!/bin/bash

echo "This script prints some patterns."
while true
do
    read -p 'Enter Your Number: ' num
    

    case $num in

    1)
        for((i=1;i<=5;i++)); do
            printf %${i}s |tr " " "$i"
            printf "\n"
        done
        ;;
    2)
        for((i=1;i<=6;i++)); do
            let spaces=6-$i
            printf %${spaces}s |tr " " " "

            seq -s'* ' $i|tr -d '[:digit:]'
            printf "\n"
        done

        for((i=1;i<=6;i++)); do
            let spaces=$i-1
            printf %${spaces}s |tr " " " "

            let stars=7-$i
            seq -s'* ' $stars|tr -d '[:digit:]'
            printf "\n"
        done
        ;;
    3)
        printf "|_\n"
        for((i=2;i<=5;i++)); do
            let bars=$i-1
            seq -s'| ' $bars|tr -d '[:digit:]'

            printf "|_\n"
        done
        ;;
    esac


    read -p 'Do You Want to Continue? ' status

    if [[ $status == "n" ]] || [[ $status == "no" ]] || [[ $status == "No" ]];
    then
        echo "Goodbye $USER!"
        exit
    fi 
done