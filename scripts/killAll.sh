# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Public Domain
#

# killAll.sh
# Kill all processes.
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#

PROCS_PID_LIST=$(ps -C serv,servList,select_html --no-headers -o pid,cmd | sed 's/^ *//' | cut -f1 -d ' ')

if [ -z "$PROCS_PID_LIST" ]; then
	echo "There are no processes running."
	exit 1
fi

kill $PROCS_PID_LIST
ALERT_ERROR "Kill $PROCS_PID_LIST"
