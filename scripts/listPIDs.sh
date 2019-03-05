# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Public Domain
#

# listPIDs.sh
# List all processes PIDs.
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#

echo "1) servList: " `ps -o pid -C servList | grep -v PID`
netstat -nap --tcp --listening 2>/dev/null | grep 999
echo "2) select: " `ps aux | grep select| grep -v vim | grep -v grep | awk '{print $2}'`
echo "3) serv: " `ps -o pid -C serv | grep -v PID`
