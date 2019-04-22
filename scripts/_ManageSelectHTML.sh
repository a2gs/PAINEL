# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Apache License 2.0
#

# _ManageSelectHTML.sh
# TODO
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#

#!/bin/bash

trap '' SIGINT SIGQUIT SIGTSTP

PROC="select_html_"

echo "--------------------------------------------------"
echo "LISTAGEM DE PROCESSOS $PROC*:"
echo "PID	CMD"
ps -e -o pid,cmd | grep "$PROC" | grep -v "grep"
echo "--------------------------------------------------"

if [[ "$#" -ne 0 ]];
then
	echo "$0 nao aceita parametros."
	echo ""
	exit
fi

echo "MENU:"
echo "1) Subir uma nova instancia"
echo "2) Baixar uma instancia"
echo "3) Reiniciar uma instancia"
echo "4) Excluir HTML"
echo ""
echo "Opcao: "
