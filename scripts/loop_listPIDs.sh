# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Public Domain
#

# loop_listPIDs.sh
# Loop listing all processes PIDs (listPIDs.sh).
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#

#!/bin/sh
while true
do
	echo ---------------
	$PAINEL_HOME/listPIDs.sh
	sleep 3
done
