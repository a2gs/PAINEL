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

if [ -f "$PAINEL_HOME/bin/select_html" ]; then

	echo '--- Stating select_htmls ------------------------------'
	#Execucao:\n%s <FUNCAO> <SEGUNDOS_RELOAD_GER_HTML> <SEGUNDOS_REFRESH_HTML> <FULL_LOG_PATH> <LOG_LEVEL>

	echo 'Launching select_html OperadorMaquina select_html_OperadorMaquina.log'
	$PAINEL_HOME/bin/select_html OperadorMaquina 4 5 $PAINEL_HOME/log/select_html_OperadorMaquina.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/select_html.log 2>&1
	ALERT_ERROR 'select_html OperadorMaquina select_html_OperadorMaquina.log'
	sleep 1

	echo 'Launching select_html SupervisorMaquina select_html_SupervisorMaquina.log'
	$PAINEL_HOME/bin/select_html SupervisorMaquina 4 5 $PAINEL_HOME/log/select_html_SupervisorMaquina.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/select_html.log 2>&1
	ALERT_ERROR 'select_html SupervisorMaquina select_html_SupervisorMaquina.log'
	sleep 1

	echo 'Launching select_html FornoEletrico select_html_FornoEletrico.log'
	$PAINEL_HOME/bin/select_html FornoEletrico 4 5 $PAINEL_HOME/log/select_html_FornoEletrico.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/select_html.log 2>&1
	ALERT_ERROR 'select_html FornoEletrico select_html_FornoEletrico.log'
	sleep 1

	echo 'Launching select_html All select_html_All.log'
	$PAINEL_HOME/bin/select_html All 4 5 $PAINEL_HOME/log/select_html_All.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/select_html.log 2>&1
	ALERT_ERROR 'select_html All select_html_All.log'
	sleep 1

	sleep 2 # just a delay to all select_htmls create html file

else

	echo "There is no [$PAINEL_HOME/bin/select_html] binary!"

fi
