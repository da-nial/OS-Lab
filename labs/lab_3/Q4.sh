#!/bin/bash

echo "Hi! This script prints certain lines of a given file!"

read -p "Enter the file name: " file
read -p "Enter the starting line number: " x
read -p "Enter the finishing line number: " y

sed -n "$x,$y p" < $file
