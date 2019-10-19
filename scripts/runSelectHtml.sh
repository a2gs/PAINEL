# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Apache License 2.0
#

# runSelectHtml.sh
# Runs all ServList process.
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 05/03/2019 | Creation
#          |            |
#

#!/bin/bash

DATE=`date +%Y%m%d`

if [ -f "$PAINEL_HOME/bin/select_html" ]; then

	echo '--- Stating select_htmls ------------------------------'

	echo "Launching select_html OperadorMaquina select_html_OperadorMaquina.log"
	#$PAINEL_HOME/bin/select_html OperadorMaquina 4 5 $PAINEL_HOME/log/select_html_OperadorMaquina_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/select_html.log 2>&1
	$PAINEL_HOME/bin/select_html $PAINEL_HOME/running/CFGs/select_html_OperadorMaquina.cfg 1>>$PAINEL_HOME/log/select_html.log 2>&1
	ALERT_ERROR "select_html OperadorMaquina select_html_OperadorMaquina_$DATE.log"
	sleep 1

	echo "Launching select_html SupervisorMaquina select_html_SupervisorMaquina_$DATE.log"
	#$PAINEL_HOME/bin/select_html SupervisorMaquina 4 5 $PAINEL_HOME/log/select_html_SupervisorMaquina_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/select_html.log 2>&1
	$PAINEL_HOME/bin/select_html $PAINEL_HOME/running/CFGs/select_html_SupervisorMaquina.cfg 1>>$PAINEL_HOME/log/select_html.log 2>&1
	ALERT_ERROR "select_html SupervisorMaquina select_html_SupervisorMaquina_$DATE.log"
	sleep 1

	echo "Launching select_html FornoEletrico select_html_FornoEletrico_$DATE.log"
	#$PAINEL_HOME/bin/select_html FornoEletrico 4 5 $PAINEL_HOME/log/select_html_FornoEletrico_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/select_html.log 2>&1
	$PAINEL_HOME/bin/select_html $PAINEL_HOME/running/CFGs/select_html_FornoEletrico.cfg 1>>$PAINEL_HOME/log/select_html.log 2>&1
	ALERT_ERROR "select_html FornoEletrico select_html_FornoEletrico.log"
	sleep 1

	echo "Launching select_html All select_html_All_$DATE.log"
	#$PAINEL_HOME/bin/select_html All 4 5 $PAINEL_HOME/log/select_html_All_$DATE.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/select_html.log 2>&1
	$PAINEL_HOME/bin/select_html $PAINEL_HOME/running/CFGs/select_html_All.cfg 1>>$PAINEL_HOME/log/select_html.log 2>&1
	ALERT_ERROR "select_html All select_html_All_$DATE.log"
	sleep 1

	sleep 2 # just a delay to all select_htmls create html file

else

	echo "There is no [$PAINEL_HOME/bin/select_html] binary!"

fi
