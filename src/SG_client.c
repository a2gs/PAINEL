/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* SG_client.c
 * Client side business-specific logic/stuff: client screen, protocol formatter, etc.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "util.h"
#include "client.h"
#include "SG_client.h"

#include "sha-256.h"
#include "log.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define PASS_LEN							(100)
#define CONFIRMA_ENFIO_LEN				(5)
#define LINE_DRT_FILE_LEN				(200)
#define DRT_FULLFILEPATH_SZ			(300)
#define SUBPATH_RUNNING_DATA_CLI 	SUBPATH_RUNNING_DATA


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */
static char lineToSend[MAXLINE + 1] = {0};
static log_t *log = NULL;


/* *** FUNCTIONS *********************************************************************** */
void getLogSystem(log_t *logClient)
{
	log = logClient;
	return;
}

/* int SG_sendLogin(int sockfd, char *drt, char *passhash, char *funcao)
 *
 * Formats the outgoing login message.
 *
 * INPUT:
 *  sockfd - Socket to server
 *  drt - User ID
 *  passhash - Hashed password
 *  funcao - User level (or office responsibility)
 * OUTPUT:
 *  OK - Ok
 *  NOK - Problem writing to socket
 */
int SG_sendLogin(int sockfd, char *drt, char *passhash, char *funcao)
{
	ssize_t srRet = 0;
	size_t srSz = 0;

	memset(lineToSend, '\0', MAXLINE + 1);

	/* Sending user validation */
	/* COD|DRT|DATAHORA|FUNCAO|PASSHASH */
	srSz = snprintf(lineToSend, MAXLINE, "%d|%s|%s|%s|%s", PROT_COD_LOGIN, drt, time_DDMMYYhhmmss(), funcao, passhash);

	for(srRet = 0; srRet < (ssize_t)srSz; ){
		srRet += send(sockfd, &lineToSend[srRet], srSz - srRet, 0);

		logWrite(log, LOGDEV, "Sending to server: [%*s] [%l]B.\n", srRet, &lineToSend[srRet], srRet);

		if(srRet == -1){
			logWrite(log, LOGOPALERT, "ERRO: wellcome send() [%s] for [%s].\n", strerror(errno), drt);
			return(NOK);
		}
	}

	memset(lineToSend, '\0', MAXLINE + 1);

	/* Receiving user validation response */

	for(srSz = 0; srRet == 0; srSz += srRet){
		srRet = recv(sockfd, &lineToSend[srSz], MAXLINE, 0);

		logWrite(log, LOGDEV, "Receiving from server: [%s] [%l]B.\n", lineToSend, srRet);

		if(srRet == -1){
			logWrite(log, LOGOPALERT, "ERRO: receiving server response [%s] for [%s].\n", strerror(errno), drt);
			return(NOK);
		}
	}

	return(OK);
}

int SG_sendExit(int sockfd, char *drt, char *funcao)
{
	memset(lineToSend, '\0', MAXLINE);

	/* COD|DRT|DATAHORA|FUNCAO */
	snprintf(lineToSend, MAXLINE, "%d|%s|%s|%s", PROT_COD_LOGOUT, drt, time_DDMMYYhhmmss(), funcao);

	if(send(sockfd, lineToSend, strlen(lineToSend), 0) == -1){
		logWrite(log, LOGOPALERT, "ERRO: send() exit [%s].\n", strerror(errno));
		printf("ERRO no envio de exit motivo [%s]!\n", strerror(errno));
		return(NOK);
	}

	return(OK);
}

int SG_interfaceFornoEletrico(char *drt, int socket)
{
	char confirmaEnvio[CONFIRMA_ENFIO_LEN + 1] = {'\0'};
	tipoPreenchimento_t retCampo = OK;
	fornoElet_t fornElet = {
		.perguntas = {"Porcentagem de FeSi (xxx,yy): ",
		              "Porcentagem de S (xxx,yy): ",
		              "Porcentagem de Mg (xxx,yy): ",
		              "Panela (xx): ",
		              "Temperatura (xxxxx,yyy): ",
		              "Numero Forno Eletrico (AAxxx): "
		},
		.percFeSi  = {'\0'},
		.percS     = {'\0'},
		.percMg    = {'\0'},
		.panela    = {'\0'},
		.temp      = {'\0'},
		.fornEletr = {'\0'}
	};

	while(1){
		printf("\n\n---[%s]------------------------------------------------------------------\n", time_DDMMYYhhmmss());
		printf("Preenchimento de registro (digite '%s' para sair da sua DRT [%s] ou '%s' para descartar o registr atual e iniciar um novo)\n\n", LOGOUT_CMD, CORRIGIR_CMD, drt);

		retCampo = preencherCampo(fornElet.perguntas[0], fornElet.percFeSi, FORNELET_PERC_FESI_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(fornElet.perguntas[1], fornElet.percS, FORNELET_PERC_S_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(fornElet.perguntas[2], fornElet.percMg, FORNELET_PERC_MG_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(fornElet.perguntas[3], fornElet.panela, FORNELET_PAMELA_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(fornElet.perguntas[4], fornElet.temp, FORNELET_TEMP_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(fornElet.perguntas[5], fornElet.fornEletr, FORNELET_NUMFORELE_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		do{
			memset(confirmaEnvio, '\0', sizeof(confirmaEnvio));
			printf("\nConfirmar envio do registro acima (s/n)? ");
			fgets(confirmaEnvio, CONFIRMA_ENFIO_LEN, stdin);
		}while(confirmaEnvio[0] != 's' && confirmaEnvio[0] != 'S' && confirmaEnvio[0] != 'n' && confirmaEnvio[0] != 'N');

		if(confirmaEnvio[0] == 's' || confirmaEnvio[0] == 'S'){
			memset(lineToSend, '\0', MAXLINE);
			/* COD|DRT|DATAHORA||FUNCAO|PANELA||FORNELETR||||TEMP|PERCFESI|PERCMG||PERCS||||||| */
			snprintf(lineToSend, MAXLINE, "%d|%s|%s||%s|%s||%s||||%s|%s|%s||%s|||||||", PROT_COD_INSREG, drt, time_DDMMYYhhmmss(), STR_FORNOELETRICO, fornElet.panela, fornElet.fornEletr, fornElet.temp, fornElet.percFeSi, fornElet.percMg, fornElet.percS);

			logWrite(log, LOGOPMSG, "Mensagem [%s] enviada.\n", lineToSend);
							 
			if(send(socket, lineToSend, strlen(lineToSend), 0) == -1){
				logWrite(log, LOGOPALERT, "ERRO: send() [%s]: [%s].\n", lineToSend, strerror(errno));
				printf("ERRO no envio desta mensagem [%s] motivo [%s]!\n", lineToSend, strerror(errno));
				break;
			}
		}else
			printf("REGISTRO NAO ENVIADO!\n");
	}

	return(OK);
}

int SG_interfaceOperadorMaquina(char *drt, int socket)
{
	char confirmaEnvio[CONFIRMA_ENFIO_LEN + 1] = {'\0'};
	tipoPreenchimento_t retCampo = OK;
	operMaquina_t operMaquina = {
		.perguntas = {"Porcentagem de FeSi (xxx,yy): ",
		              "Porcentagem de Inoculante (xxx,yy): ",
		              "Aspecto (200 texto): ",
		              "Panela (xx): ",
		              "WS (xx,yy): ",
		              "Diametro Nominal (xxxxxx): ",
		              "Classe (xxxxxx): ",
		              "Temperatura (xxxxx,yyy): ",
		              "Numero da Maquina (x): "
		},
		.percFeSi       = {'\0'},
		.percInoculante = {'\0'},
		.aspecto        = {'\0'},
		.panela         = {'\0'},
		.ws             = {'\0'},
		.diamNom        = {'\0'},
		.classe         = {'\0'},
		.temp           = {'\0'},
		.numMaquina     = {'\0'}
	};

	while(1){
		printf("\n\n---[%s]------------------------------------------------------------------\n", time_DDMMYYhhmmss());
		printf("Preenchimento de registro (digite '%s' para sair da sua DRT [%s] ou '%s' para descartar o registr atual e iniciar um novo)\n\n", LOGOUT_CMD, CORRIGIR_CMD, drt);

		retCampo = preencherCampo(operMaquina.perguntas[0], operMaquina.percFeSi, OPEMAQ_PERC_FESI_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(operMaquina.perguntas[1], operMaquina.percInoculante, OPEMAQ_PERC_INOC_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(operMaquina.perguntas[2], operMaquina.aspecto, OPEMAQ_ASPEC_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(operMaquina.perguntas[3], operMaquina.panela, OPEMAQ_PANELA_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(operMaquina.perguntas[4], operMaquina.ws, OPEMAQ_WS_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(operMaquina.perguntas[5], operMaquina.temp, OPEMAQ_DIAMNOM_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(operMaquina.perguntas[6], operMaquina.classe, OPEMAQ_CLASSE_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(operMaquina.perguntas[7], operMaquina.temp, OPEMAQ_TEMP_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		retCampo = preencherCampo(operMaquina.perguntas[8], operMaquina.numMaquina, OPEMAQ_NUMMAQ_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC){
			printf("REGISTRO CANCELADO!\n");
			continue;
		}

		do{
			memset(confirmaEnvio, '\0', sizeof(confirmaEnvio));
			printf("\nConfirmar envio do registro acima (s/n)? ");
			fgets(confirmaEnvio, CONFIRMA_ENFIO_LEN, stdin);
		}while(confirmaEnvio[0] != 's' && confirmaEnvio[0] != 'S' && confirmaEnvio[0] != 'n' && confirmaEnvio[0] != 'N');

		if(confirmaEnvio[0] == 's' || confirmaEnvio[0] == 'S'){
			memset(lineToSend, '\0', MAXLINE);
			/* COD|DRT|DATAHORA||FUNCAO|PANELA|WS||NUMMAQUINA|||TEMP|PERCFESI|||||INOCULANTE||||ASPECTUBO| */
			snprintf(lineToSend, MAXLINE, "%d|%s|%s||%s|%s|%s||%s|||%s|%s|||||%s||||%s|", PROT_COD_INSREG, drt, time_DDMMYYhhmmss(), STR_OPERMAQUINA, operMaquina.panela, operMaquina.ws, operMaquina.numMaquina, operMaquina.temp, operMaquina.percFeSi, operMaquina.percInoculante, operMaquina.aspecto);

			logWrite(log, LOGOPMSG, "Mensagem [%s] enviada.\n", lineToSend);

			if(send(socket, lineToSend, strlen(lineToSend), 0) == -1){
				logWrite(log, LOGOPALERT, "ERRO: send() [%s]: [%s]\n", lineToSend, strerror(errno));
				printf("ERRO no envio desta mensagem [%s] motivo [%s]!\n", lineToSend, strerror(errno));
				break;
			}  
		}else
			printf("REGISTRO NAO ENVIADO!\n");
	}

	return(OK);
}

int SG_interfaceSupervisorMaquina(char *drt, int socket)
{
	char confirmaEnvio[CONFIRMA_ENFIO_LEN + 1] = {'\0'};
	tipoPreenchimento_t retCampo = OK;
	supMaquina_t supMaquina = {
		.perguntas = {"Aspecto (200 texto): ",
		              "Refugo (200 texto): ",
		              "Cadencia (xxx,yy): ",
		              "OEE (xxx,yy): "
		},
		.aspecto = {'\0'},
		.refugo = {'\0'},
		.cadencia = {'\0'},
		.oee = {'\0'}
	};

	while(1){
		printf("\n\n---[%s]------------------------------------------------------------------\n", time_DDMMYYhhmmss());
		printf("Preenchimento de registro (digite '%s' para sair da sua DRT [%s] ou '%s' para descartar o registr atual e iniciar um novo)\n\n", LOGOUT_CMD, CORRIGIR_CMD, drt);

		retCampo = preencherCampo(supMaquina.perguntas[0], supMaquina.aspecto, SUPMAQ_ASPEC_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC) continue;

		retCampo = preencherCampo(supMaquina.perguntas[1], supMaquina.refugo, SUPMAG_REFUGO_LEN);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC) continue;

		retCampo = preencherCampo(supMaquina.perguntas[2], supMaquina.cadencia, SUPMAQ_CADENCIA);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC) continue;

		retCampo = preencherCampo(supMaquina.perguntas[3], supMaquina.oee, SUPMAQ_OEE);
		if(retCampo == EXITPC){
			return(NOK);
			break;
		}else if(retCampo == CORRIGIRPC) continue;

		do{
			memset(confirmaEnvio, '\0', sizeof(confirmaEnvio));
			printf("\nConfirmar envio do registro acima (s/n)? ");
			fgets(confirmaEnvio, CONFIRMA_ENFIO_LEN, stdin);
		}while(confirmaEnvio[0] != 's' && confirmaEnvio[0] != 'S' && confirmaEnvio[0] != 'n' && confirmaEnvio[0] != 'N');

		if(confirmaEnvio[0] == 's' || confirmaEnvio[0] == 'S'){
			memset(lineToSend, '\0', MAXLINE);
			/* COD|DRT|DATAHORA||FUNCAO|||||||||||||||CADENCIA|OEE|ASPECTUBO|REFUGO */
			snprintf(lineToSend, MAXLINE, "%d|%s|%s||%s|||||||||||||||%s|%s|%s|%s", PROT_COD_INSREG, drt, time_DDMMYYhhmmss(), STR_SUPMAQUINA, supMaquina.cadencia, supMaquina.oee, supMaquina.aspecto, supMaquina.refugo);

			logWrite(log, LOGOPMSG, "Mensagem [%s] enviada.\n", lineToSend);

			if(send(socket, lineToSend, strlen(lineToSend), 0) == -1){
				logWrite(log, LOGOPALERT, "ERRO: send() [%s]: [%s]\n", lineToSend, strerror(errno));
				printf("ERRO no envio desta mensagem [%s] motivo [%s]!\n", lineToSend, strerror(errno));
				break;
			}  
		}else
			printf("REGISTRO NAO ENVIADO!\n");
	}

	return(OK);
}

int geraArqDRTs(void)
{
	FILE *f = NULL;

	if((f = fopen("./EXEMPLO_DRTs.text", "w")) == NULL){
		fprintf(stderr, "Unable to open/create SAQMPLE_DRTs.text! [%s]\n", strerror(errno));
		return(NOK);
	}

	fprintf(f, "11111-%s\n", STR_FORNOELETRICO);
	fprintf(f, "22222-%s\n", STR_OPERMAQUINA);
	fprintf(f, "33333-%s\n", STR_SUPMAQUINA);
	fprintf(f, "\n\nLembrar: O arquivo de DRTs deve obrigatoriamente ter o nome [%s/%s/%s] e a funcao nao deve ter mais de [%d] caracteres!", getPAINELEnvHomeVar(), SUBPATH_RUNNING_DATA_CLI, DRT_FILE, VALOR_FUNCAO_LEN);

	fflush(f);
	fclose(f);

	return(OK);
}

/* int SG_relacionaDRTTipoUsuario(char *drt, char *funcao, tipoUsuario_t *usrType)
 *
 * Search the User Level (office responsability) from a DRT (User ID) reading a local file.
 *
 * INPUT:
 *  drt - User ID
 * OUTPUT:
 *  funcao - User Level (office responsability) to a given User ID (DRT)
 *  usrType - User level type (tipoUsuario_t)
 *  NOK - User Id not located into file.
 *  OK - User identified
 */
int SG_relacionaDRTTipoUsuario(char *drt, char *funcao, tipoUsuario_t *usrType)
{
	FILE *fDRT = NULL;
	char line[LINE_DRT_FILE_LEN]={'\0'};
	char *c = NULL;
	char drtReaded[DRT_LEN] = {'\0'};
	char drtFullFilePath[DRT_FULLFILEPATH_SZ] = {'\0'};

	snprintf(drtFullFilePath, DRT_FULLFILEPATH_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), SUBPATH_RUNNING_DATA_CLI, DRT_FILE);

	if((fDRT = fopen(drtFullFilePath, "r")) == NULL){
		logWrite(log, LOGOPALERT, "Unable to open/read [%s]! [%s].\n", drtFullFilePath, strerror(errno));
		return(NOK);
	}

	while(!feof(fDRT)){
		memset(line,      '\0', LINE_DRT_FILE_LEN);
		memset(drtReaded, '\0', DRT_LEN);
		memset(funcao,    '\0', VALOR_FUNCAO_LEN);

		if(fgets(line, LINE_DRT_FILE_LEN, fDRT) == NULL) break;

		changeCharByChar(line, '\n', '\0');
		/*
		c = strchr(line, '\n');
		if(c != NULL) *c = '\0';
		*/

		c = strchr(line, '-');
		if(c == NULL) continue; /* linha mal formatada, sem '-' */

		strncpy(drtReaded, line, c-line);

		if(strncmp(drtReaded, drt, DRT_LEN) == 0){
			strncpy(funcao, c+1, VALOR_FUNCAO_LEN);

			if(strcmp(funcao, STR_FORNOELETRICO) == 0){
				*usrType = FORNO_ELETRICO;
				break;
			}else if (strcmp(funcao, STR_OPERMAQUINA) == 0){
				*usrType = OPERADOR_MAQUINA;
				break;
			}else if (strcmp(funcao, STR_SUPMAQUINA) == 0){
				*usrType = SUPERVISOR_MAQUINA;
				break;
			}else{
				*usrType = UNDEFINED_USER;
				logWrite(log, LOGOPALERT, "DRT [%s] cadastrada mas funcao nao definida!\n", drt);
				fclose(fDRT);
				return(NOK);
			}
		}
	}

	if(feof(fDRT)){
		logWrite(log, LOGOPALERT, "DRT [%s] nao cadastrada neste sistema!\n", drt);
		fclose(fDRT);
		return(NOK);
	}

	fclose(fDRT);
	return(OK);
}

int SG_fazerLogin(char *drt, char *passhash, char *funcao, tipoUsuario_t *userType)
{
	char pass[PASS_LEN + 1] = {'\0'};
	uint8_t hash[32] = {0};

	drt[0] = '\0';

	/* DRT */
	do{
		memset(drt, '\0', DRT_LEN);
		printf("\n\n---[%s]------------------------------------------------------------------\n", time_DDMMYYhhmmss());
		printf("Digite sua DRT ('%s' para sair): ", LOGOUT_CMD);
		fgets(drt, DRT_LEN, stdin);


		changeCharByChar(drt, '\n', '\0');
		/*
		c = strchr(drt, '\n');
		if(c != NULL) *c = '\0';
		*/
	}while(drt[0] == '\0');

	if(strncmp(drt, LOGOUT_CMD, sizeof(LOGOUT_CMD)-1) == 0){
		return(NOK);
	}

	if(SG_relacionaDRTTipoUsuario(drt, funcao, userType) == NOK)
		logWrite(log, LOGOPALERT, "Tentativa de acesso com DRT [%s] nao reconhecida ou com funcao cadastrada invalida as [%s].\n", drt, time_DDMMYYhhmmss());

	/* PASS */
	printf("Digite sua senha: ");
	fgets(pass, PASS_LEN, stdin);

	changeCharByChar(pass, '\n', '\0');
	/*
	c = strchr(pass, '\n');
	if(c != NULL) *c = '\0';
	*/

	/* HASH PASS */
	calc_sha_256(hash, pass, strlen(pass));
	hash_to_string(passhash, hash);

	return(OK);
}

int SG_clientScreen(int sockfd, char *drt, char *funcao, tipoUsuario_t userType)
{
	switch(userType){

		case FORNO_ELETRICO:
			if(SG_interfaceFornoEletrico(drt, sockfd) == NOK)
				return(NOK);
			break;

		case OPERADOR_MAQUINA:
			if(SG_interfaceOperadorMaquina(drt, sockfd) == NOK)
				return(NOK);
			break;

		case SUPERVISOR_MAQUINA:
			if(SG_interfaceSupervisorMaquina(drt, sockfd) == NOK)
				return(NOK);
			break;

		default:
			break;
	}

	return(OK);
}
