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

echo "Existing from here because nothing has been done yet ..."
exit

TIMEEPOCH=`date +'%s'`000000

WEBDIR=/usr/local/webserver

DEVLOCAL=/home/usracct/DevWebSiteToSync
PREPROD=/home/usracct/PrepProd

PHPFPM="php7.4-fpm"
BRANCH="rel-2021"

WEBID=${FIRST_WEBID}
for pwd in $PWD
do
	echo "Publishing on Server " $WEBID
	sshpass -p $pwd ssh -l root frontend-0${WEBID}.repmyblock.net "rm ${WEBDIR}/${TYPE}.repmyblock.org; ln -s /usr/local/webserver/${TYPE}-v${BIG_ID} ${WEBDIR}/${TYPE}.repmyblock.org; service ${PHPFPM} restart"
	WEBID=$(echo $WEBID + 1 | bc)
done

