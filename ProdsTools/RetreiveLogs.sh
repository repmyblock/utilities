#!/bin/sh

FIRST_WEBID=1
while IFS= read -r line; do PASSWD="${PASSWD} ${line}"; done < ~/.RMB_FrontEnd
DBPASS=`head -1 ~/.RMB_ProdDB`
PWDLOCAL="usracct"
SPACESTOP="80"

DIRECTORY_REMOTE=/var/log/nginx
DIRECTORY_LOCAL=/home/usracct/data/logs
DATE=`date +%Y%m%d`
WEBID=1

for pwd in $PWD
do
	echo "Version #" $WEBID
	mkdir -p ${DIRECTORY_LOCAL}/www-0${WEBID}/${DATE}
	sshpass -p $pwd scp root@frontend-0${WEBID}.repmyblock.net:${DIRECTORY_REMOTE}/* ${DIRECTORY_LOCAL}/www-0${WEBID}/${DATE}
	sshpass -p $pwd ssh -l root frontend-0${WEBID}.repmyblock.net "rm ${DIRECTORY_REMOTE}/*.gz"
	WEBID=$(echo $WEBID + 1 | bc)
done





