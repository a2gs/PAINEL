# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Apache License 2.0
#

# _ManageServ.sh
# TODO
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#

#!/bin/bash

trap '' SIGINT SIGQUIT SIGTSTP

PROC="serv"

echo "--------------------------------------------------"
echo "LISTAGEM DE PROCESSOS $PROC*:"
echo "PID	CMD"
ps -o pid,cmd -C "$PROC"
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
echo "4) Excluir LOG"
echo "5) RotacionarLOG"
echo "6) Excluir DB"
echo "7) Rotacionar DB"
echo ""
echo "Opcao: "
