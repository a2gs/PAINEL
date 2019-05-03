# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Apache License 2.0
#

# runAll.sh
# Runs all processes (default values).
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#

#!/bin/bash

# ------------------------------------------------
runServ.sh

# ------------------------------------------------
runServList.sh

# ------------------------------------------------
runSelectHtml.sh

# ------------------------------------------------
$PAINEL_HOME/scripts/listPIDs.sh
