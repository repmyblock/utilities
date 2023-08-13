#!/bin/bash


FILE_1="/home/usracct/VoterFiles/NY/$1/AllNYSVoters_$1.txt"
FILE_2="/home/usracct/VoterFiles/NY/$2/AllNYSVoters_$2.txt"

echo "Grepping $1"
RESULT1=`cat -n ${FILE_1} | grep $3`

echo "Grepping $2"
RESULT2=`cat -n ${FILE_2} | grep $3`

echo "FILE1: ${RESULT1}"
echo "FILE2: ${RESULT2}"

