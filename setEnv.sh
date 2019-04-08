# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Public Domain
#

# setEnv.sh
# Set envirounment variables, alias and etc.. for PAINEL
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#

# Usage:
# . ./setEnv.sh
# or
# source ./setEnv.sh

#!/bin/sh

export PAINEL_HOME=`pwd`

export PATH=$PATH:$PAINEL_HOME/scripts

alias testcli="./client localhost 9998 client_test.log \"REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\""

ALERT_ERROR()
{
	ret=$?

	echo "Running: $1"

	if [ $ret -ne 0 ]; then
		echo "ERROR! Returned: $ret"
	else
		echo 'Ok!'
	fi
}
export -f ALERT_ERROR

chmod +x ./scripts/*
