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

#kill `cat servlock`
kill `ps -o pid -C servList | grep -v PID`
ALERT_ERROR 'Kill servList'

kill `ps aux | grep select| grep -v vim | grep -v grep | awk '{print $2}'`
ALERT_ERROR 'Kill select'

kill `ps -o pid -C serv | grep -v PID`
ALERT_ERROR 'Kill serv'
