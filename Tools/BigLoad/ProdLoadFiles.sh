#!/bin/bash

echo "Counter to pass: $1"

GREEN=$'\e[0;32m'
RED=$'\e[0;31m'
YELLOW=$'\e[1;33m'
PINK=$'\e[1;35m'
NC=$'\e[0m'

array=( 20151215 20170515 20180127 20180423 20180529 20180924 20181029 20181203 20190204 20190225 20190325 \
				20190408 20190513 20190617 20190702 20190805 20190903 20191021 20191125 20191209 20200113 20200203 \
				20200218 20200309 20200406 20200420 20200615 20200717 20200721 20201005 20201116 20201221 20210119 \
				20210222 20210308 20210405 20210614 20210816 20210927 20211018 20211122 20220214 20220305 20220316 \
				20220425 20220516 20220613 20220801 20220822 20220919 20221107 )
				
#array=( 20221107 )
				
mysql -h 192.168.199.18 -u usracct --password=usracct RepMyBlock -e "\
	TRUNCATE VotersComplementInfo; \
	TRUNCATE DataDistrict; \
	TRUNCATE DataDistrictTemporal; \
	TRUNCATE DataHouse; \
	TRUNCATE DataMailingAddress; \
	TRUNCATE DataStreetNonStdFormat; \
	TRUNCATE VotersIndexes; \
	TRUNCATE Voters; \
"
# Loading the CD Information in 6627.78 -> 16145805 lines = 110 minutes -> Alomst 2 hours

Counter=0
for i in "${array[@]}"
do
	echo "${PINK}Starting 01-LoadFieldsFromRawData $i${NC}       " `date` ""
	./01-LoadFieldsFromRawData.pl $i
	if [ $? -gt 0 ]; then
		exit
	fi 
	
	echo "${PINK}Starting 02-LoadVoters $i${NC}                  " `date`
	./02-LoadVoters.pl $i 
	if [ $? -gt 0 ]; then
		exit
	fi  
	
	echo "${PINK}Starting 03-LoadHouses $i${NC}                  " `date` 
	./03-LoadHouses.pl $i 
	if [ $? -gt 0 ]; then
		exit
	fi 
	
	echo "${PINK}Starting 04-LoadFinalVoter $i${NC}              " `date` 
	./04-LoadFinalVoter.pl $i
	if [ $? -gt 0 ]; then
		exit
	fi  
	
	echo "${PINK}Starting 05-LoadComplementInfo.pl $i${NC}              " `date` 
	./05-LoadComplementInfo.pl $i
	if [ $? -gt 0 ]; then
		exit
	fi  
	
	echo "Starting the end of round $i"
	echo ""
		
	### End the loop
	((Counter++))
	echo "The Counter is $Counter"
	if [ $1 -lt $Counter ]; then
		exit
	fi
		
done
echo "End the whole Truncate                     " `date`
