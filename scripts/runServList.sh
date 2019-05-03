# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Apache License 2.0
#

# runServList.sh
# Runs all Server List process.
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 05/03/2019 | Creation
#          |            |
#

#!/bin/bash

# ------------------------------------------------
if [ -f "$PAINEL_HOME/bin/servList" ]; then

	echo '--- Stating servLists ---------------------------------'

	$PAINEL_HOME/bin/servList 9997 $PAINEL_HOME/html/All_Refresh.html $PAINEL_HOME/log/allRefresh.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR 'servList 9997 allRefresh.log'

	$PAINEL_HOME/bin/servList 9996 $PAINEL_HOME/html/All_Static.html $PAINEL_HOME/log/all.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR 'servList 9996 all.log'

	$PAINEL_HOME/bin/servList 9995 $PAINEL_HOME/html/FornoEletrico_Refresh.html $PAINEL_HOME/log/FornoEletricoRefresh.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR 'servList 9995 FornoEletricoRefresh.log'

	$PAINEL_HOME/bin/servList 9994 $PAINEL_HOME/html/FornoEletrico_Static.html $PAINEL_HOME/log/FornoEletrico.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR 'servList 9994 FornoEletrico.log'

	$PAINEL_HOME/bin/servList 9993 $PAINEL_HOME/html/OperadorMaquina_Refresh.html $PAINEL_HOME/log/OperadorMaquinaRefresh.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR 'servList 9993 OperadorMaquinaRefresh.log'

	$PAINEL_HOME/bin/servList 9992 $PAINEL_HOME/html/OperadorMaquina_Static.html $PAINEL_HOME/log/OperadorMaquina.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR 'servList 9992 OperadorMaquina.log'

	$PAINEL_HOME/bin/servList 9991 $PAINEL_HOME/html/SupervisorMaquina_Refresh.html $PAINEL_HOME/log/SupervisorMaquinaRefresh.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR 'servList 9991 SupervisorMaquinaRefresh.log'

	$PAINEL_HOME/bin/servList 9990 $PAINEL_HOME/html/SupervisorMaquina_Static.html $PAINEL_HOME/log/SupervisorMaquina.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR 'servList 9990 SupervisorMaquina.log'

else

	echo "There is no [$PAINEL_HOME/bin/servList] binary!"

fi
