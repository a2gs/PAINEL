# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Public Domain
#

# BkpLimparLogs.sh
# Log rotate.
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#

#!/bin/bash

LOG_PATH="$PAINEL_HOME/log"

cd "$LOG_PATH"

clear

echo 'Arquivos de logs:'
ls *.log

echo ''

echo 'Gerando backup individuais e truncando (limpando) arquivos:'

for FILES in *.log
do
echo "$FILES"
tar cfz "$FILES".tar.gz "$FILES"
truncate --size 0 "$FILES"
done

cd "$FULL_PATH"
