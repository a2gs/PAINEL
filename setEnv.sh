# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Public Domain
#

# setEnv.sh
# Set envirounment variables, alias and etc.. for PAINEL
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#

# Usage:
# . ./setEnv.sh
# or
# source ./setEnv.sh

#!/bin/sh

export PAINEL_HOME=`pwd`

export PATH=$PATH:$PAINEL_HOME/scripts
