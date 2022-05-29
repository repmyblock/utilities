#!/bin/sh

FIRST_WEBID=1
while IFS= read -r line; do PASSWD="${PASSWD} ${line}"; done < ~/.RMB_FrontEnd
DBPASS=`head -1 ~/.RMB_ProdDB`
PWDLOCAL="usracct"
SPACESTOP="80"

if [ "$1" != "" ]; then
    TYPE=$1
else
    TYPE="www"
fi

TIMEEPOCH=`date +'%s'`000000

WEBDIR=/usr/local/webserver

DEVLOCAL=/home/usracct/DevWebSiteToSync
PREPROD=/home/usracct/PrepProd

PHPFPM="php7.4-fpm"
BRANCH="rel-2021"

DIRECTORY_HTDOCS=${DEVLOCAL}/${TYPE}/htdocs
DIRECTORY_LIBS=${DEVLOCAL}/${TYPE}/libs

### BEFORE WE START, CHECK FINAL DIRECTORY SIZE
WEBID=${FIRST_WEBID}
for pwd in ${PASSWD}
do
	echo "Check directory size at WEBID: ${WEBID}"
	SPACEAMOUNT=`sshpass -p $pwd ssh -l root frontend-0${WEBID}.repmyblock.net "df -k ${WEBDIR}" | grep -v Filesystem | awk '{print $5}' | tr -d '%'` 
	echo -n "WEBID ${WEBID} space amount is at ${SPACEAMOUNT}%"
	if [ $SPACEAMOUNT -gt $SPACESTOP ]; then	
		echo " - Issuing stop command"	
		STOPSPACE=1
	else 
		echo " - Good space"
		STOPSPACE=0
	fi
	WEBID=$(echo $WEBID + 1 | bc)	
done

if [ $STOPSPACE  -ne 0 ]; then
	echo "Stopping due to space constrains";
	exit
fi

FILE_TO_REMOVE="forum .gitmodules outraged README.md static .git www/statlib pdf/statlib www/www"
rm -rf ${DEVLOCAL}/*
git clone --single-branch --branch ${BRANCH} https://github.com/repmyblock/website.git ${DEVLOCAL}

for file in ${FILE_TO_REMOVE}
do
	rm -rf ${DEVLOCAL}/${file}
done

WEBID=${FIRST_WEBID}
BIG_ID=0
for pwd in $PWD
do
	LAST_ID=`sshpass -p $pwd ssh -l root frontend-0${WEBID}.repmyblock.net ls ${WEBDIR} | grep ${TYPE}-v`	
	for id in ${LAST_ID}
	do
		MYID=`echo $id | sed "s/${TYPE}-v//g"` 
		
		if [ $MYID -gt $BIG_ID ];
		then 
			BIG_ID=${MYID}
		fi
	done
	
	WEBID=$(echo ${WEBID} + 1 | bc)
done

BIG_ID=$(echo ${BIG_ID} + 1 | bc)
echo "Version #" ${BIG_ID}

if [ -d ${PREPROD}/${TYPE}-v${BIG_ID} ]; then
	rm -r ${PREPROD}/${TYPE}-v${BIG_ID}
fi

mkdir -p ${PREPROD}/${TYPE}-v${BIG_ID}
mv ${DIRECTORY_HTDOCS} ${PREPROD}/${TYPE}-v${BIG_ID}
mv ${DIRECTORY_LIBS} ${PREPROD}/${TYPE}-v${BIG_ID}

WEBID=${FIRST_WEBID}
for pwd in $PWD
do
	echo "Pushing to WEBID: " ${WEBID}
	sshpass -p $pwd rsync -a ${PREPROD}/${TYPE}-v${BIG_ID}/* --exclude=statlib* --exclude=statconfig* root@frontend-0${WEBID}.repmyblock.net:${WEBDIR}/${TYPE}-v$BIG_ID
	sshpass -p $pwd ssh -l root frontend-0${WEBID}.repmyblock.net ln -s ${WEBDIR}/statlibs-${BRANCH}/${TYPE} ${WEBDIR}/${TYPE}-v${BIG_ID}/statlib
	sshpass -p $pwd ssh -l root frontend-0${WEBID}.repmyblock.net "echo \"${BRANCH}-${TYPE}-v${BIG_ID}\" > ${WEBDIR}/${TYPE}-v${BIG_ID}/libs/VERSION.txt"
	
	### This is to insert the version in the header php file.
	## if [ ${TYPE} = "www" ] || [ ${TYPE} = "bugs" ]; then
	##	sshpass -p $pwd ssh -l root frontend-0${WEBID}.repmyblock.net "sed  -i 's/<!--- REPLACELINKWITHVERSIONTAG --->/<?php \$BetaVersion = \"${BRANCH}-${TYPE}-v${BIG_ID}\"; ?>/g' ${WEBDIR}/${TYPE}-v$BIG_ID/htdocs/common/headers.php"	
	## fi
	
	WEBID=$(echo $WEBID + 1 | bc)
done

### Add to the TRAC Debug Database
if [ ${TYPE} != "bugs" ]; then
	mysql --password=${DBPASS} -u theo -h data.repmyblock.net -P 2514 -e "INSERT INTO Trac.version SET name = \"${BRANCH}-${TYPE}-v${BIG_ID}\", time = ${TIMEEPOCH}";
fi

WEBID=${FIRST_WEBID}
for pwd in $PWD
do
	echo "Publishing on Server " $WEBID
	sshpass -p $pwd ssh -l root frontend-0${WEBID}.repmyblock.net "rm ${WEBDIR}/${TYPE}.repmyblock.org; ln -s /usr/local/webserver/${TYPE}-v${BIG_ID} ${WEBDIR}/${TYPE}.repmyblock.org; service ${PHPFPM} restart"
	WEBID=$(echo $WEBID + 1 | bc)
done

