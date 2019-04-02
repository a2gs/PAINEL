# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Public Domain
#

# runAll.sh
# Runs all processes (default values).
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#


# ------------------------------------------------
echo '--- Stating serv --------------------------------------'
if [ ! -f "$PAINEL_HOME/database/database.db" ]; then
	echo "Creating DB..."
	sleep 4
fi
$PAINEL_HOME/bin/serv 9998 $PAINEL_HOME/log/serv9998.log 'REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV' 1>>$PAINEL_HOME/log/serv.log 2>&1
ALERT_ERROR 'serv 9998'


# ------------------------------------------------
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


# ------------------------------------------------
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


# ------------------------------------------------
$PAINEL_HOME/scripts/listPIDs.sh
