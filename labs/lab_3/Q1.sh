#!/bin/bash

re='^[0-9]+$'

if ! [[ $1 =~ $re ]] ; then
   echo "ERROR: The First Input is Not a Number"
   exit 1
fi

if ! [[ $2 =~ $re ]] ; then
   echo "ERROR: The Second Input is Not a Number"
   exit 1
fi

expr $1 + $2

if [ $1 -gt $2 ]
then
    echo "The greater value is: $1"
elif [ $2 -gt $1 ]
then 
    echo "The greater value is: $2"
elif [ $2 -eq $1 ]
then
    echo "The values are equal!"
fi
