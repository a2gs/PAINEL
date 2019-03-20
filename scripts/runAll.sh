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
$PAINEL_HOME/bin/serv 9998
ALERT_ERROR 'serv 9998'


# ------------------------------------------------
echo '--- Stating select_htmls ------------------------------'
#Execucao:\n%s <FUNCAO> <SEGUNDOS_RELOAD_GER_HTML> <SEGUNDOS_REFRESH_HTML>
$PAINEL_HOME/bin/select_html OperadorMaquina 1 5   2> $PAINEL_HOME/log/select_html_OperadorMaquina.log &
ALERT_ERROR 'select_html OperadorMaquina select_html_OperadorMaquina.log'

$PAINEL_HOME/bin/select_html SupervisorMaquina 1 5 2> $PAINEL_HOME/log/select_html_SupervisorMaquina.log &
ALERT_ERROR 'select_html SupervisorMaquina select_html_SupervisorMaquina.log'

$PAINEL_HOME/bin/select_html FornoEletrico 1 5     2> $PAINEL_HOME/log/select_html_FornoEletrico.log &
ALERT_ERROR 'select_html FornoEletrico select_html_FornoEletrico.log'

$PAINEL_HOME/bin/select_html All 1 5               2> $PAINEL_HOME/log/select_html_All.log &
ALERT_ERROR 'select_html All select_html_All.log'


# ------------------------------------------------
echo '--- Stating servLists ---------------------------------'
$PAINEL_HOME/bin/servList 9997 $PAINEL_HOME/html/All_Refresh.html               2> $PAINEL_HOME/log/allRefresh.log
ALERT_ERROR 'servList 9997 allRefresh.log'

$PAINEL_HOME/bin/servList 9996 $PAINEL_HOME/html/All_Static.html                2> $PAINEL_HOME/log/all.log
ALERT_ERROR 'servList 9996 all.log'

$PAINEL_HOME/bin/servList 9995 $PAINEL_HOME/html/FornoEletrico_Refresh.html     2> $PAINEL_HOME/log/FornoEletricoRefresh.log
ALERT_ERROR 'servList 9995 FornoEletricoRefresh.log'

$PAINEL_HOME/bin/servList 9994 $PAINEL_HOME/html/FornoEletrico_Static.html      2> $PAINEL_HOME/log/FornoEletrico.log
ALERT_ERROR 'servList 9994 FornoEletrico.log'

$PAINEL_HOME/bin/servList 9993 $PAINEL_HOME/html/OperadorMaquina_Refresh.html   2> $PAINEL_HOME/log/OperadorMaquinaRefresh.log
ALERT_ERROR 'servList 9993 OperadorMaquinaRefresh.log'

$PAINEL_HOME/bin/servList 9992 $PAINEL_HOME/html/OperadorMaquina_Static.html    2> $PAINEL_HOME/log/OperadorMaquina.log
ALERT_ERROR 'servList 9992 OperadorMaquina.log'

$PAINEL_HOME/bin/servList 9991 $PAINEL_HOME/html/SupervisorMaquina_Refresh.html 2> $PAINEL_HOME/log/SupervisorMaquinaRefresh.log
ALERT_ERROR 'servList 9991 SupervisorMaquinaRefresh.log'

$PAINEL_HOME/bin/servList 9990 $PAINEL_HOME/html/SupervisorMaquina_Static.html  2> $PAINEL_HOME/log/SupervisorMaquina.log
ALERT_ERROR 'servList 9990 SupervisorMaquina.log'


# ------------------------------------------------
echo '--- PROCESSOS NO AR -----------------------------------'
ps -C serv,servList,select_html -o pid,cmd | sed 's/^ *//' | column  -t

echo '--- PORTAS EM LISTNING --------------------------------'
PROCS_PID_LIST=$(ps -C serv,servList,select_html --no-headers -o pid,cmd | sed 's/^ *//' | cut -f1 -d ' ')
netstat -nap --tcp --listening 2>/dev/null | grep "$PROCS_PID_LIST"
