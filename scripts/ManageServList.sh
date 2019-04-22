# Andre Augusto Giannotti Scota (a2gs)
# andre.scota@gmail.com
#
# PAINEL
#
# Apache License 2.0
#

# ManageServList.sh
# Operator's ServList processes tool.
#
#  Who     | When       | What
#  --------+------------+----------------------------
#   a2gs   | 13/08/2018 | Creation
#          |            |
#

#!/bin/bash
set +v

trap '' SIGINT SIGQUIT SIGTSTP

PROC='servList'

LOG_PATH='./log/'
HTML_PATH='./html/'

FULL_PATH=$PAINEL_HOME

printListProcs()
{
	echo '---['`date '+%Y-%m-%d %H:%M:%S'`']------------'
	echo 'LISTAGEM DE PROCESSOS servList:'
	ps -o pid,cmd -C "$PROC"
	echo '------------------------------------'
}

#------------------------------------------------------------------------
StartInst(){
	local PORT=''
	local FILE_HTML=''
	local PROCRUNNING=''
	local RUNNING_PID=''
	local LOGNAME=''
	local PORTINUSE=''

	echo '=================================================================='
	echo 'MENU: 1) INICIANDO NOVA INSTANCIA'
	echo ''

	printListProcs

	until [ -f "$HTML_PATH""$FILE_HTML" ];
	do
		echo ''
		echo 'Arquivos HTML disponiveis:'
		ls "$HTML_PATH"
		echo ''

		read -p 'Qual arquivo deseja iniciar o server (em branco para cancelar): ' FILE_HTML
		if [ -z "$FILE_HTML" ];
		then
			echo 'Operacao cancelada!'
			return
		fi

		if [ ! -f "$HTML_PATH""$FILE_HTML" ];
		then
			echo ''
			echo "ERRO: Arquivo $FILE_HTML nao existe!"
			echo ''
		fi
	done

	until [ -n "$PORT" ]
	do
		read -p 'Qual porta deseja iniciar o server (em branco para cancelar): ' PORT
		if [ -z "$PORT" ];
		then
		echo "Operacao cancelada!"
			return
		fi

		PORTINUSE=`netstat -ntl | grep "[0-9]*\.[0-9]*\.[0-9]*.[0-9]*:$PORT"`
		if [ -n "$PORTINUSE" ];
		then
			echo ''
			echo 'Esta porta ja esta em uso por outro processo!'
			echo ''
			PORT=''
		fi
	done

	#Verificando se ja existe processo nesta porta e arquivo
	PROCRUNNING=`ps --no-headers -o pid,cmd -C "$PROC" | grep "$FILE_HTML" | grep "$PORT" | grep -v grep`

	if [ -z "$PROCRUNNING" ];
	then
		LOGNAME="$LOG_PATH"`date '+%Y%m%d_%H%M%S'`'_'$PROC'_'$PORT'_'$FILE_HTML'.log'
		"$FULL_PATH"/"$PROC" "$PORT" "$FULL_PATH"/"$HTML_PATH""$FILE_HTML" 2> "$FULL_PATH"/"$LOGNAME"

		echo ''
		echo "Executando: $FULL_PATH/$PROC $PORT $FULL_PATH/$HTML_PATH$FILE_HTML 2> $FULL_PATH/$LOGNAME"
		echo ''
	else
		RUNNING_PID=`echo "$PROCRUNNING" | cut -d' ' -f1`
		echo ''
		echo "Este processo ja esta rodando com o PID: $RUNNING_PID"
		echo 'Nao sera levantada outra instancia com a mesma configuracao!'
		echo ''
		return
	fi

	return
}

DownInst(){
	local PID_TO_KILL

	echo '=================================================================='
	echo 'MENU: 2) BAIXANDO UMA INSTANCIA'
	echo ''
	printListProcs
	echo ''

	read -p 'Digite o PID do processo que deseja parar (em branco para cancelar): ' PID_TO_KILL
	if [ -z "$PID_TO_KILL" ];
	then
		echo 'Operacao cancelada!'
		return
	fi

	kill "$PID_TO_KILL"
}

main(){
	clear
	local choice

	while :
	do
		echo ''
		echo '=================================================================='
		echo 'MENU: PRINCIPAL'
		echo ''

		printListProcs

		echo ""
		echo "MENU:"
		echo "1) Subir uma nova instancia"
		echo "2) Baixar uma instancia"
		echo "3) Sair"
		echo ""

		read -p "Opcao [1 - 3]: " choice

		echo ""

		case $choice in
			1)
				clear
				StartInst ;;
			2)
				clear
				DownInst ;;
			3) exit 0;;
			*) echo -e "Opcao invalida" && sleep 2
		esac
	done
}

#------------------------------------------------------------------------

if [[ "$#" -ne 0 ]];
then
	echo "$0 nao aceita parametros."
	echo ""
	exit
fi

main
