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

DATE=`date +%Y%m%d`

# ------------------------------------------------
if [ -f "$PAINEL_HOME/bin/servList" ]; then

	echo '--- Stating servLists ---------------------------------'

	#$PAINEL_HOME/bin/servList 9997 $PAINEL_HOME/html/All_Refresh.html $PAINEL_HOME/log/allRefresh_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	$PAINEL_HOME/bin/servList $PAINEL_HOME/running/CFGs/servList_7.cfg 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR "servList 9997 allRefresh_$DATE.log"

	#$PAINEL_HOME/bin/servList 9996 $PAINEL_HOME/html/All_Static.html $PAINEL_HOME/log/all_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	$PAINEL_HOME/bin/servList $PAINEL_HOME/running/CFGs/servList_6.cfg 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR "servList 9996 all_$DATE.log"

	#$PAINEL_HOME/bin/servList 9995 $PAINEL_HOME/html/FornoEletrico_Refresh.html $PAINEL_HOME/log/FornoEletricoRefresh_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	$PAINEL_HOME/bin/servList $PAINEL_HOME/running/CFGs/servList_5.cfg 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR "servList 9995 FornoEletricoRefresh_$DATE.log"

	#$PAINEL_HOME/bin/servList 9994 $PAINEL_HOME/html/FornoEletrico_Static.html $PAINEL_HOME/log/FornoEletrico_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	$PAINEL_HOME/bin/servList $PAINEL_HOME/running/CFGs/servList_4.cfg 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR "servList 9994 FornoEletrico_$DATE.log"

	#$PAINEL_HOME/bin/servList 9993 $PAINEL_HOME/html/OperadorMaquina_Refresh.html $PAINEL_HOME/log/OperadorMaquinaRefresh_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	$PAINEL_HOME/bin/servList $PAINEL_HOME/running/CFGs/servList_3.cfg 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR "servList 9993 OperadorMaquinaRefresh_$DATE.log"

	#$PAINEL_HOME/bin/servList 9992 $PAINEL_HOME/html/OperadorMaquina_Static.html $PAINEL_HOME/log/OperadorMaquina_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	$PAINEL_HOME/bin/servList $PAINEL_HOME/running/CFGs/servList_2.cfg 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR "servList 9992 OperadorMaquina_$DATE.log"

	#$PAINEL_HOME/bin/servList 9991 $PAINEL_HOME/html/SupervisorMaquina_Refresh.html $PAINEL_HOME/log/SupervisorMaquinaRefresh_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	$PAINEL_HOME/bin/servList $PAINEL_HOME/running/CFGs/servList_1.cfg 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR "servList 9991 SupervisorMaquinaRefresh_$DATE.log"

	#$PAINEL_HOME/bin/servList 9990 $PAINEL_HOME/html/SupervisorMaquina_Static.html $PAINEL_HOME/log/SupervisorMaquina_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/servList.log 2>&1
	$PAINEL_HOME/bin/servList $PAINEL_HOME/running/CFGs/servList_0.cfg 1>>$PAINEL_HOME/log/servList.log 2>&1
	ALERT_ERROR "servList 9990 SupervisorMaquina_$DATE.log"

else

	echo "There is no [$PAINEL_HOME/bin/servList] binary!"

fi
