#!/bin/bash

mysql -h 192.168.199.18 -u usracct --password=usracct RepMyBlock -e "\
	TRUNCATE DataDistrictTemporal; \
	TRUNCATE Voters; \
"

./04-LoadFinalVoter.pl 20151215  

./01-LoadFieldsFromRawData.pl 20221107
./02-LoadVoters.pl 20221107
./03-LoadHouses.pl 20221107
./04-LoadFinalVoter.pl 20221107  