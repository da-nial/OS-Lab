#!/bin/bash

while true
do
    read -p 'Enter Your Number: ' num
    
    echo -n "Reverse is: "
    echo $num | rev
    
    num_len=${#num}
    sum=0

    for((i=$num_len-1;i>=0;i--)); do
        digit=${num:i:1}
        sum=$((sum+digit))
        done

    echo "Sum of Digits of $num is: $sum"

    read -p 'Do You Want to Continue? ' status

    if [[ $status == "n" ]] || [[ $status == "no" ]] || [[ $status == "No" ]];
    then
        echo 'Goodbye!'
        exit
    fi 
done