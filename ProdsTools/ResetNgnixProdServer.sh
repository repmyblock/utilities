#!/bin/sh

FIRST_WEBID=1
while IFS= read -r line; do PASSWD="${PASSWD} ${line}"; done < ~/.RMB_FrontEnd
PWDLOCAL="usracct"
SPACESTOP="80"

PHPFPM="php7.4-fpm"

WEBID=${FIRST_WEBID}
for pwd in $PWD
do
	echo "Restarting Server " $WEBID
	sshpass -p $pwd ssh -l root frontend-0${WEBID}.repmyblock.net "service ${PHPFPM} stop; service nginx stop; sleep 1; service ${PHPFPM} start; service nginx start"
	WEBID=$(echo $WEBID + 1 | bc)
done

