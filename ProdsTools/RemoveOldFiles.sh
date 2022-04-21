#!/bin/sh

FIRST_WEBID=1
while IFS= read -r line; do PASSWD="${PASSWD} ${line}"; done < ~/.RMB_FrontEnd
PWDLOCAL="usracct"
SPACESTOP="80"

if [ "$1" != "" ]; then
    TYPE=$1
else
    TYPE="www"
fi

#REMOTE=usracct@192.168.199.199:/usr/local/webserver/www.repmyblock.nyc/
TIMEEPOCH=`date +'%s'`000000

WEBDIR=/usr/local/webserver

WEBID=${FIRST_WEBID}
BIG_ID=0
for pwd in ${PASSWD}
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
	
	ACTIVEID=`sshpass -p $pwd ssh -l root frontend-0${WEBID}.repmyblock.net find ${WEBDIR} -maxdepth 1 -type l -ls | grep ${TYPE}-v`
	LINKEDVER=`echo ${ACTIVEID} | sed "s/.*${TYPE}-v//g"`
	
	echo "My Active: ${ACTIVEID}"
	echo "Oldest Version for ${TYPE} => ${BIG_ID}"
	echo "Current Linked version for ${TYPE} => ${LINKEDVER}"
	echo "Version #" ${BIG_ID}
	echo ""
	
	LAST_ID=`sshpass -p $pwd ssh -l root frontend-0${WEBID}.repmyblock.net ls ${WEBDIR} | grep ${TYPE}-v`	

	for id in ${LAST_ID}
	do
		MYID=`echo $id | sed "s/${TYPE}-v//g"` 		
		if [ ${MYID} != ${LINKEDVER} ];
		then
			echo "Removing: ${WEBDIR}/${id}"
			sshpass -p $pwd ssh -l root frontend-0${WEBID}.repmyblock.net rm -r ${WEBDIR}/${id}
		fi
	
	done
	WEBID=$(echo ${WEBID} + 1 | bc)
done


