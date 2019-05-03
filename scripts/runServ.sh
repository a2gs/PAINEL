# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Apache License 2.0
#

# runServ.sh
# Runs PAINEL server.
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 03/05/2019 | Creation
#          |            |
#

#!/bin/bash

DATE=`date +%Y%m%d_%H%M%S`

# ------------------------------------------------
if [ -f "$PAINEL_HOME/bin/serv" ]; then

	echo '--- Stating serv --------------------------------------'

	$PAINEL_HOME/bin/serv 9998 $PAINEL_HOME/log/serv9998_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/serv.log 2>&1
	ALERT_ERROR "serv 9998 $PAINEL_HOME/log/serv9998_$DATE.log"
	if [ ! -f "$PAINEL_HOME/database/database.db" ]; then
		echo 'Creating DB...'
		sleep 4
	fi

else

	echo "There is no [$PAINEL_HOME/bin/serv] binary!"

fi
