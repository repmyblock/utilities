#!/bin/bash

mysql -h 192.168.199.18 -u usracct --password=usracct RepMyBlock -e "\
	TRUNCATE VotersComplementInfo; \
	TRUNCATE DataAddress; \
	TRUNCATE DataDistrict; \
	TRUNCATE DataDistrictTemporal; \
	TRUNCATE DataHouse; \
	TRUNCATE VotersIndexes; \
	TRUNCATE Voters; \
"

./01-LoadFieldsFromRawData.pl 20151215
./01-LoadFieldsFromRawData.pl 20221107
