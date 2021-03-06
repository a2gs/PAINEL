# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Apache License 2.0
#

# loop_listPIDs.sh
# Loop listing all processes PIDs (listPIDs.sh).
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#

#!/bin/bash

while true
do
	echo '===[' `date +"%d/%m/%Y %H:%M:%S"` ']========================================================'
	$PAINEL_HOME/scripts/listPIDs.sh
	sleep 3
done
