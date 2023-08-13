#!/bin/bash

mysql -h 192.168.199.18 -u usracct --password=usracct RepMyBlock -e "\
	TRUNCATE VotersComplementInfo; \
	TRUNCATE DataDistrictTemporal; \
	TRUNCATE DataHouse; \
	TRUNCATE Voters; \
"

./02-LoadVoters.pl 20151215
./02-LoadVoters.pl 20221107

./03-LoadHouses.pl 20151215
./03-LoadHouses.pl 20221107