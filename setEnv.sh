# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Apache License 2.0
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

#!/bin/bash

DATE=`date +%Y%m%d_%H%M%S`

export PAINEL_HOME=`pwd`

export PATH=$PATH:$PAINEL_HOME/scripts

if [ ! -d running ]; then
	echo 'Creating running directory.'
	mkdir -p running
fi

if [ ! -d running/CFGs ]; then
	echo 'Creating CFGs directory (no cfgs inside).'
	mkdir -p running/CFGs
fi

if [ ! -d running/PIDs ]; then
	echo 'Creating PIDs directory.'
	mkdir -p running/PIDs
fi

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

# Some helper alias
alias clitest="./client localhost 9998 $PAINEL_HOME/log/client_$DATE.log \"REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\""
alias ncclitest="./ncclient localhost 9998 $PAINEL_HOME/log/ncclient_$DATE.log \"REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\""

alias cmdTest="./sendRecvCmd localhost 9998 ../running/sendRecvCmds.text"
