#!/bin/bash

echo "Hi! This script is a float calculator which supports the 4 main operations."

while true
do
    read -p 'Enter The First Operand: ' operand1
    read -p 'Enter The Operator: ' operator
    read -p 'Enter The Second Operand: ' operand2

    echo -n 'The Result is: '

    case $operator in

    '+')
        echo "$operand1 + $operand2" | bc -l
        ;;
    '-')
        echo "$operand1 - $operand2" | bc -l
        ;;
    '*')
        echo "$operand1 * $operand2" | bc -l
        ;;
    '/')
        echo "$operand1 / $operand2" | bc -l
        ;;
    esac

    read -p 'Do You Want to Continue? ' status

    if [[ $status == "n" ]] || [[ $status == "no" ]] || [[ $status == "No" ]];
    then
        echo "Goodbye! $USER"
        exit
    fi 
done