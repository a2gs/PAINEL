# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Apache License 2.0
#

# listPIDs.sh
# List all processes PIDs.
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#

#!/bin/bash

echo '--- PROCESSOS NO AR -----------------------------------'
ps -C serv,servList,select_html -o pid,cmd | sed 's/^ *//' | column -t

if [ -f "$PAINEL_HOME/running/PIDs/servlock" ]; then
	echo '--- SERVLOCK ------------------------------------------'
	echo "There is a servlock [$PAINEL_HOME/running/PIDs/servlock] file! Content:"
	cat $PAINEL_HOME/running/PIDs/servlock
fi

echo '--- PORTAS EM LISTNING --------------------------------'
PROCS_PID_LIST=$(ps -C serv,servList,select_html --no-headers -o pid,cmd | sed 's/^ *//' | cut -f1 -d ' ')

if [ -z "$PROCS_PID_LIST" ]; then
	echo "There are no listening processes running."
else
	netstat -nap --tcp --listening 2>/dev/null | grep "$PROCS_PID_LIST"
fi
