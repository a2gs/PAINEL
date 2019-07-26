/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* ncclient.c
 * ncurses client interface.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 25/04/2019 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ncurses.h>
#include <form.h>

#include "ncclient_util.h"
#include "util.h"
#include "util_network.h"
#include "SG.h"
#include "userId.h"

#include <log.h>
#include <sha-256.h>
#include <wizard_by_return.h>
#include <linkedlist.h>
#include <cfgFile.h>


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define STATUS_BAR_1                         "Connected [%s:%s] User [%s]"

#define STATUS_BAR_1_LINE_N                  (LINES - 1)
#define STATUS_BAR_1_COL_N(__fmt_bar1_srt__) ((COLS - strlen(__fmt_bar1_srt__))/2)

#define STATUS_BAR_2_LINE_N                  (LINES - 2)

#define FMT_STATUS_BAR_1_SZ                  (100)

#define ESC_KEY                              (27)

#define SERVERADDRESS_SZ                     (100)
#define SERVERPORT_SZ                        (7)
#define USERLOGGED_SZ                        (DRT_LEN)

#define SUBPATH_RUNNING_DATA_NCCLI           SUBPATH_RUNNING_DATA


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */
a2gs_ToolBox_WizardReturnFunc_t screen_menu(void *data);


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */
static log_t log;
static char serverAddress[SERVERADDRESS_SZ + 1] = {'\0'};
static char serverPort[SERVERPORT_SZ + 1] = {'\0'};
static char userLogged[USERLOGGED_SZ + 1] = {'\0'};


/* *** FUNCTIONS *********************************************************************** */
size_t formatTitle(char *titleOut, size_t titleOutSz, char *msg)
{
	size_t msgSz = 0;
	size_t midTitle = 0;
	size_t midMsg = 0;

	msgSz = strlen(msg);

	if(titleOutSz < msgSz) return(0);

	midTitle = titleOutSz / 2;
	midMsg   = msgSz / 2;

	memset(titleOut, ' ', titleOutSz);
	memcpy(titleOut + midTitle - midMsg, msg, msgSz);

	return(1);
}

void drawDefaultStatusBar(void)
{
	char fmt_STATUS_BAR_1[FMT_STATUS_BAR_1_SZ +1] = {0};

	snprintf(fmt_STATUS_BAR_1, FMT_STATUS_BAR_1_SZ, STATUS_BAR_1, serverAddress, serverPort, userLogged);
	mvwprintw(stdscr, STATUS_BAR_1_LINE_N, STATUS_BAR_1_COL_N(fmt_STATUS_BAR_1), fmt_STATUS_BAR_1);

	refresh();
}

void drawKeyBar(char *keyBar)
{
	mvwprintw(stdscr, STATUS_BAR_2_LINE_N, ((COLS - strlen(keyBar))/2), keyBar);

	refresh();
}

void signalHandle(int sig)
{
	switch(sig){
		case SIGWINCH:
			break;
		default:
			break;
	}
}

int screen_drawDefaultTheme(WINDOW **screen, int totLines, int totCols, char *title)
{
	char screenTitle[200] = {0};

	clear();
	drawDefaultStatusBar();

	*screen = newwin(totLines, totCols, (LINES/2)-(totLines/2), (COLS/2)-(totCols/2));
	box(*screen, 0, 0);

	formatTitle(screenTitle, totCols-2, title);
	mvwaddch(*screen, 2, 0, ACS_LTEE);
	mvwaddch(*screen, 2, totCols-1, ACS_RTEE);
	mvwhline(*screen , 2, 1, ACS_HLINE, totCols-2);
	wattron(*screen, A_REVERSE);
	mvwprintw(*screen, 1, 1, screenTitle);
	wattroff(*screen, A_REVERSE);

	wrefresh(*screen);

	return(PAINEL_OK);
}

int formDriver(WINDOW *screen, FORM *formScreen, int ch)
{
	static char insertFlag = 0;

	switch(ch){
		case KEY_DOWN:
			form_driver(formScreen, REQ_NEXT_FIELD);
			form_driver(formScreen, REQ_END_LINE);
			break;

		case KEY_UP:
			form_driver(formScreen, REQ_PREV_FIELD);
			form_driver(formScreen, REQ_END_LINE);
			break;

		case KEY_LEFT:
			form_driver(formScreen, REQ_PREV_CHAR);
			break;

		case KEY_RIGHT:
			form_driver(formScreen, REQ_NEXT_CHAR);
			break;

		case KEY_IC:
			if(insertFlag == 0) form_driver(formScreen, REQ_OVL_MODE);
			else                form_driver(formScreen, REQ_INS_MODE);
			insertFlag = ~insertFlag;
			break;

		case KEY_HOME:
			form_driver(formScreen, REQ_BEG_FIELD);
			break;

		case KEY_END:
			form_driver(formScreen, REQ_END_FIELD);
			break;

		case KEY_BACKSPACE:
		case 127:
			form_driver(formScreen, REQ_DEL_PREV);
			break;

		case KEY_DC:
			form_driver(formScreen, REQ_DEL_CHAR);
			break;

		default:
			form_driver(formScreen, ch);
			break;
	}

	form_driver(formScreen, REQ_VALIDATION);

	wrefresh(screen);

	return(PAINEL_OK);
}

a2gs_ToolBox_WizardReturnFunc_t screen_config(void *data)
{
#define SRC_CFG_MAX_LINES (8)
#define SRC_CFG_MAX_COLS  (60)
	int ch = 0;
	WINDOW *thisScreen = NULL;
	WINDOW *formCfgScreen = NULL;
	FORM *formCfg = NULL;
	FIELD *drtCfg[5] = {NULL, NULL, NULL, NULL, NULL};

	logWrite(&log, LOGDEV, "Cfg screen.\n");

	if(screen_drawDefaultTheme(&thisScreen, SRC_CFG_MAX_LINES, SRC_CFG_MAX_COLS, "Client Configuration") == PAINEL_NOK){
		return(NULL);
	}

	drtCfg[0] = new_field(1, 25, 1, 1,  0, 0);
	drtCfg[1] = new_field(1, 30, 1, 27, 0, 0);
	drtCfg[2] = new_field(1, 25, 2, 1,  0, 0);
	drtCfg[3] = new_field(1, 5,  2, 27, 0, 0);
	drtCfg[4] = NULL;

	set_field_buffer(drtCfg[0], 0, "Server DNS or IP address:");
	set_field_opts(drtCfg[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

	set_field_buffer(drtCfg[1], 0, serverAddress);
	set_field_back(drtCfg[1], A_UNDERLINE);
	set_field_opts(drtCfg[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	set_field_buffer(drtCfg[2], 0, "Port....................:");
	set_field_opts(drtCfg[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

	set_field_buffer(drtCfg[3], 0, serverPort);
	set_field_back(drtCfg[3], A_UNDERLINE);
	set_field_opts(drtCfg[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	formCfg = new_form(drtCfg);
	formCfgScreen = derwin(thisScreen, SRC_CFG_MAX_LINES - 4, SRC_CFG_MAX_COLS - 2, 3, 1);

	set_form_win(formCfg, thisScreen);
	set_form_sub(formCfg, formCfgScreen);

	while(1){
		post_form(formCfg);

		curs_set(1);

		wrefresh(thisScreen);
		wrefresh(formCfgScreen);

		ch = getch();

		if(ch == KEY_ENTER || ch == 10){
			char auxSrvAdd[SERVERADDRESS_SZ + 1] = {'\0'};
			char auxSrvPrt[SERVERPORT_SZ    + 1] = {'\0'};

			alltrim(field_buffer(drtCfg[1], 0), auxSrvAdd, SERVERADDRESS_SZ);
			alltrim(field_buffer(drtCfg[3], 0), auxSrvPrt, SERVERPORT_SZ);

			curs_set(0);

			if(pingServer(auxSrvAdd, auxSrvPrt) == PAINEL_NOK){
				mvwprintw(formCfgScreen, 3, 1, "Erro em tentar conexao. Corrigir ou sair? (c/S)");
				wrefresh(formCfgScreen);

				ch = getch();

				if(ch == 'C' || ch == 'c'){
					unpost_form(formCfg);
					continue;
				}else break;
			}

			mvwprintw(formCfgScreen, 3, 1, "Salvar alteracao? (s/N)");
			wrefresh(formCfgScreen);

			ch = getch();

			if(ch == 'S' || ch == 's'){
				strncpy(serverAddress, auxSrvAdd, SERVERADDRESS_SZ);
				strncpy(serverPort,    auxSrvPrt, SERVERPORT_SZ);
			}

			break;
		}

		if(ch == ESC_KEY) break;

		formDriver(formCfgScreen, formCfg, ch);
	}

	curs_set(0);

	unpost_form(formCfg);
	free_form(formCfg);
	free_field(drtCfg[0]);
	free_field(drtCfg[1]);
	free_field(drtCfg[2]);
	free_field(drtCfg[3]);
	delwin(formCfgScreen);
	delwin(thisScreen);

	return(screen_menu);
}

ll_node_t *searchLLUserDRT(ll_node_t *head, char *drt, size_t drtSz)
{
	ll_node_t *walker = NULL;

	/* TODO: change code below to ll_node_t * ll_search(ll_node_t **head, ll_node_t *search, int (*searchNodeFunc)(void *e1, void *e2)) */

	for(walker = head; walker != NULL; walker = walker->next){
		if(strncmp(drt, ((userId_t *)(walker->data))->userId, drtSz) == 0)
			return(walker);
	}

	return(NULL);
}

int getUserIFace(char *level)
{
	int srError = 0;
	size_t msgSzOut = 0;
	char msgIFace[MAXLINE + 1] = {'\0'}, *msgWalker = NULL;
	char bufAuxFType[FTYPE_MAX_SZ + 1] = {'\0'};
	usrField_t bufAux;
	protoData_t data;

	if(formatProtocol(&data, PROT_COD_IFACE, msgIFace, MAXLINE, &msgSzOut) == PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Erro formatando protocol PROT_COD_IFACE.\n");
		return(PAINEL_NOK);
	}

	if(sendToNet(getSocket(), msgIFace, msgSzOut, &srError) == PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Erro enviando requisicao PROT_COD_IFACE: [%s].\n", strerror(srError));
		return(PAINEL_NOK);
	}

	if(recvFromNet(getSocket(), msgIFace, MAXLINE, &msgSzOut, &srError) == PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Erro recebendo requisicao PROT_COD_IFACE: [%s].\n", ((srError == 0) ? "PAINEL_NOK" : strerror(srError)));
		return(PAINEL_NOK);
	}

	msgWalker = strchr(msgIFace, '|'); /* jumping the first '|' from protocol: Protocol code */
	if(msgWalker == NULL)
		return(PAINEL_NOK); /* bad formatted protocol */

	msgWalker++;

	usrIfaceFieldsClean();

	for(;;){
		memset(&bufAux, 0, sizeof(usrField_t));

		cutter(&msgWalker, ':', bufAux.field, USR_IFACE_FIELD_SZ);
		cutter(&msgWalker, ':', bufAuxFType,  FTYPE_MAX_SZ);
		cutter(&msgWalker, ':', bufAux.fmt, USR_IFACE_FMTFIELD_SZ);
		cutter(&msgWalker, '|', bufAux.desc, USR_IFACE_DESCFIELD_SZ); /* at least */

		if(usrIfaceFieldAdd(bufAux.field, bufAuxFType, bufAux.fmt, bufAux.desc) == PAINEL_NOK){
			logWrite(&log, LOGOPALERT, "Erro inserindo registro de IFace na lista:  [%s].\n", msgIFace);
			return(PAINEL_NOK);
		}
	}

	return(PAINEL_OK);
}

int sendLoginCmd(char *login, char *pass, char *level)
{
	int srError = 0;
	uint8_t hash[PASS_SHA256_BIN_LEN + 1] = {0};
	size_t msgSzOut = 0;
	char msg[MAXLINE + 1] = {0};
	protoData_t data;

	memset(&data, 0, sizeof(protoData_t));

	calc_sha_256(hash, pass, strlen(pass));
	hash_to_string(data.passhash, hash);

	strncpy(data.drt, login, DRT_LEN);
	strncpy(data.funcao, level, VALOR_FUNCAO_LEN);

	if(formatProtocol(&data, PROT_COD_LOGIN, msg, MAXLINE, &msgSzOut) == PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Erro formatando protocol PROT_COD_LOGIN.\n");
		return(PAINEL_NOK);
	}

	if(sendToNet(getSocket(), msg, msgSzOut, &srError) == PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Erro enviando requisicao PROT_COD_LOGIN: [%s].\n", strerror(srError));
		return(PAINEL_NOK);
	}

	if(recvFromNet(getSocket(), msg, MAXLINE, &msgSzOut, &srError) == PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Erro recebendo requisicao PROT_COD_LOGIN: [%s].\n", ((srError == 0) ? "PAINEL_NOK" : strerror(srError)));
		return(PAINEL_NOK);
	}

	return(PAINEL_OK);
}

a2gs_ToolBox_WizardReturnFunc_t screen_dynamicUserScreen(void *data) /* TODO <<<<<<<<<<<<<<<<<<<<<<<<< */
{
#define SRC_DYNSCREEN_MAX_LINES (120)
#define SRC_DYNSCREEN_MAX_COLS  (40)
	WINDOW *thisScreen = NULL;

	if(screen_drawDefaultTheme(&thisScreen, SRC_DYNSCREEN_MAX_LINES, SRC_DYNSCREEN_MAX_COLS, "REGISTER") == PAINEL_NOK){
		return(screen_menu);
	}

	delwin(thisScreen);

	return(screen_menu);
}

a2gs_ToolBox_WizardReturnFunc_t screen_login(void *data)
{
#define SRC_LOGIN_MAX_LINES (8)
#define SRC_LOGIN_MAX_COLS  (24)
	int ch = 0;
	char drtFullFilePath[DRT_FULLFILEPATH_SZ + 1] = {'\0'};
	ll_node_t *head = NULL;
	ll_node_t *walker = NULL;
	WINDOW *thisScreen = NULL;
	WINDOW *formLoginScreen = NULL;
	FORM *formLogin = NULL;
	FIELD *dtrLogin[5] = {NULL, NULL, NULL, NULL, NULL};
	a2gs_ToolBox_WizardReturnFunc_t ret = NULL;

	logWrite(&log, LOGDEV, "Login screen.\n");

	if(isConnect() == 0){
		if(connectSrvPainel(serverAddress, serverPort) == PAINEL_NOK){
			logWrite(&log, LOGOPALERT, "Erro conetando ao servidor PAINEL [%s:%s].\n", serverAddress, serverPort);
			return(screen_menu);
		}
	}

	if(screen_drawDefaultTheme(&thisScreen, SRC_LOGIN_MAX_LINES, SRC_LOGIN_MAX_COLS, "User Login (DRT)") == PAINEL_NOK){
		return(screen_menu);
	}

	dtrLogin[0] = new_field(1, 9,  1, 1,  0, 0);
	dtrLogin[1] = new_field(1, 7,  1, 11, 0, 0);
	dtrLogin[2] = new_field(1, 9,  2, 1,  0, 0);
	dtrLogin[3] = new_field(1, 10, 2, 11, 0, 0);
	dtrLogin[4] = NULL;

	set_field_buffer(dtrLogin[0], 0, "DRT.....:");
	set_field_opts(dtrLogin[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

	set_field_back(dtrLogin[1], A_UNDERLINE);
	set_field_opts(dtrLogin[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	set_field_buffer(dtrLogin[2], 0, "Password:");
	set_field_opts(dtrLogin[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

	set_field_back(dtrLogin[3], A_UNDERLINE);
	set_field_opts(dtrLogin[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	formLogin = new_form(dtrLogin);
	formLoginScreen = derwin(thisScreen, SRC_LOGIN_MAX_LINES - 4, SRC_LOGIN_MAX_COLS - 2, 3, 1);

	set_form_win(formLogin, thisScreen);
	set_form_sub(formLogin, formLoginScreen);

	post_form(formLogin);

	curs_set(1);

	wrefresh(thisScreen);
	wrefresh(formLoginScreen);

	snprintf(drtFullFilePath, DRT_FULLFILEPATH_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), SUBPATH_RUNNING_DATA_NCCLI, DRT_FILE);
	logWrite(&log, LOGDEV, "Opening UserId (DRT) file to delete: [%s].\n", drtFullFilePath);

	if(loadUserIdFileToMemory(&head, drtFullFilePath) == PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Erro carregando lista do arquivo de DRT para login.\n");
		ret = screen_menu;
		goto CLEANUP_SCREEN_LOGIN;
	}

	while(1){
		post_form(formLogin);

		curs_set(1);

		wrefresh(thisScreen);
		wrefresh(formLoginScreen);

		ch = getch();

		if(ch == KEY_ENTER || ch == 10){
			char auxLogin[DRT_LEN + 1] = {'\0'};
			char auxPass[PASS_LEN + 1] = {'\0'};

			curs_set(0);

			alltrim(field_buffer(dtrLogin[1], 0), auxLogin, DRT_LEN);
			alltrim(field_buffer(dtrLogin[3], 0), auxPass,  PASS_LEN);

			/* TODO: it is looking only for DRT/User, looking with Level too. */
			/* TODO: change code below to ll_node_t * ll_search(ll_node_t **head, ll_node_t *search, int (*searchNodeFunc)(void *e1, void *e2)) and ll_getData() */
			walker = searchLLUserDRT(head, auxLogin, DRT_LEN);

			/* DRT found inside DRT_FILE */
			if(walker != NULL){
				char auxLevel[VALOR_FUNCAO_LEN + 1] = {'\0'};

				strncpy(auxLevel, userType_t_2_String(((userId_t *)walker->data)->level), VALOR_FUNCAO_LEN);

				if(sendLoginCmd(auxLogin, auxPass, auxLevel) == PAINEL_NOK){
					logWrite(&log, LOGOPALERT, "Erro enviando comando de login: [%s/%s].\n", auxLogin, auxLevel);
					ret = screen_menu;
					goto CLEANUP_SCREEN_LOGIN;
				}

				if(getUserIFace(userType_t_2_String(((userId_t *)walker->data)->level)) == PAINEL_NOK){
					logWrite(&log, LOGOPALERT, "Erro baixando interface do usuario: [%s].\n", auxLevel);
					ret = screen_menu;
					goto CLEANUP_SCREEN_LOGIN;
				}

				strncpy(userLogged, auxLogin, DRT_LEN);

				ret = screen_dynamicUserScreen;
			}else{
				/* User not registred into DRTs.text */
				logWrite(&log, LOGOPALERT, "Usuario [%s] nao registrado no arquivo de DRT [%s].\n", auxLogin, drtFullFilePath);
				ret = screen_menu;
			}

			goto CLEANUP_SCREEN_LOGIN;
		}

		if(ch == ESC_KEY){
			ret = screen_menu;
			goto CLEANUP_SCREEN_LOGIN;
		}

		formDriver(formLoginScreen, formLogin, ch);
	}

	ret = NULL; /* must never falls here... */

CLEANUP_SCREEN_LOGIN:

	curs_set(0);

	ll_destroyList(&head, 1);
	unpost_form(formLogin);
	free_form(formLogin);
	free_field(dtrLogin[0]);
	free_field(dtrLogin[1]);
	free_field(dtrLogin[2]);
	free_field(dtrLogin[3]);
	delwin(formLoginScreen);
	delwin(thisScreen);

	return(ret);
}

a2gs_ToolBox_WizardReturnFunc_t screen_listDRT(void *data)
{
#define SRC_LISTDRT_MAX_LINES (40)
#define SRC_LISTDRT_MAX_COLS  (60)
	int i = 0;
	unsigned int j = 0;
	ll_node_t *head = NULL;
	ll_node_t *walker = NULL;
	WINDOW *thisScreen = NULL, *listScreen = NULL;
	char drtFullFilePath[DRT_FULLFILEPATH_SZ + 1] = {'\0'};

	logWrite(&log, LOGDEV, "List DRT screen.\n");

	if(screen_drawDefaultTheme(&thisScreen, SRC_LISTDRT_MAX_LINES, SRC_LISTDRT_MAX_COLS, "List DRTs") == PAINEL_NOK)
		return(NULL);

	drawKeyBar("[LEFT] Back, [RIGHT] Forward and [SPACE] Quit");

	ll_create(&head);

	snprintf(drtFullFilePath, DRT_FULLFILEPATH_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), SUBPATH_RUNNING_DATA_NCCLI, DRT_FILE);

	logWrite(&log, LOGDEV, "Opening UserId (DRT) file to list: [%s].\n", drtFullFilePath);

	if(loadUserIdFileToMemory(&head, drtFullFilePath) == PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Erro carregando lista do arquivo de DRT.\n");
		ll_destroyList(&head, 1);
		return(NULL);
	}

	listScreen = derwin(thisScreen, SRC_LISTDRT_MAX_LINES - 4, SRC_LISTDRT_MAX_COLS - 2, 3, 1);

	/* TODO: change code below to ll_node_t * ll_search(ll_node_t **head, ll_node_t *search, int (*searchNodeFunc)(void *e1, void *e2)) and ll_getData() */
	for(i = 0, j = 1, walker = head; walker != NULL; walker = walker->next, i++, j++){

		mvwprintw(listScreen, i+1, 1, "%03d) %s - %s", j, ((userId_t *)(walker->data))->userId, userType_t_2_String(((userId_t *)walker->data)->level));

		if(i == 31){
			mvwprintw(listScreen, i+3, 1, "[ENTER] to next page...");
			wrefresh(listScreen);
			getch();
			wclear(listScreen);
			i = -1;
		}

	}

	mvwprintw(listScreen, 2+i, 1, "Pause. [ENTER] return to menu.");
	wrefresh(listScreen);

	getch();

	delwin(listScreen);
	delwin(thisScreen);

	ll_destroyList(&head, 1);

	return(screen_menu);
}

a2gs_ToolBox_WizardReturnFunc_t screen_addDRT(void *data)
{
#define SRC_ADDDRT_MAX_LINES (40)
#define SRC_ADDDRT_MAX_COLS  (120)
	int ch = 0;
	char drtToAdd[DRT_LEN + 1] = {'\0'};
	WINDOW *thisScreen = NULL;
	WINDOW *formScreen = NULL;
	FORM *formAddDRT = NULL;
	FIELD *dtrToAdd[3] = {NULL, NULL, NULL};

	if(screen_drawDefaultTheme(&thisScreen, 40, 120, "Add DRT") == PAINEL_NOK){
		return(NULL);
	}

	dtrToAdd[0] = new_field(1, 4, 1, 1, 0, 0);
	dtrToAdd[1] = new_field(1, 10, 1, 6, 0, 0);
	dtrToAdd[2] = NULL;


	/* TODO <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

	set_field_buffer(dtrToAdd[0], 0, "DRT:");
	set_field_opts(dtrToAdd[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

	set_field_back(dtrToAdd[1], A_UNDERLINE);
	set_field_opts(dtrToAdd[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	formAddDRT = new_form(dtrToAdd);
	formScreen = derwin(thisScreen, SRC_ADDDRT_MAX_LINES - 4, SRC_ADDDRT_MAX_COLS - 2, 3, 1);

	set_form_win(formAddDRT, thisScreen);
	set_form_sub(formAddDRT, formScreen);

	post_form(formAddDRT);

	curs_set(1);

	wrefresh(thisScreen);
	wrefresh(formScreen);

	while((ch = getch()) != ESC_KEY){

		switch(ch){
			case KEY_ENTER:
			case 10: /* ENTER */
				strncpy(drtToAdd, field_buffer(dtrToAdd[1], 0), DRT_LEN);
				break;

			case KEY_BACKSPACE:
			case 127:
				form_driver(formAddDRT, REQ_DEL_PREV);
				break;

			case KEY_DC:
				form_driver(formAddDRT, REQ_DEL_CHAR);
				break;

			case KEY_LEFT:
				form_driver(formAddDRT, REQ_PREV_CHAR);
				break;

			case KEY_RIGHT:
				form_driver(formAddDRT, REQ_NEXT_CHAR);
				break;

			default:
				form_driver(formAddDRT, ch);
				break;
		}

		wrefresh(formScreen);
	}

	curs_set(0);

	if(ch == ESC_KEY){
		/* Cleanup... */
		goto CLEANUP_SCREEN_DELDRT;
	}

	getch();
	/* LEVELS MENU */
	/* static levelMenu_t *menuLevelLabel[TOT_MENU_LEVELS_LABEL] = {NULL}; */

CLEANUP_SCREEN_DELDRT:
	unpost_form(formAddDRT);
	free_form(formAddDRT);
	free_field(dtrToAdd[0]);
	free_field(dtrToAdd[1]);
	delwin(formScreen);
	delwin(thisScreen);

	return(screen_menu);
}

a2gs_ToolBox_WizardReturnFunc_t screen_delDRT(void *data)
{
#define SRC_DELDRT_MAX_LINES (40)
#define SRC_DELDRT_MAX_COLS  (120)
	int ch = 0;
	char drtToDelete[DRT_LEN + 1] = {'\0'};
	char drtToDeleteInput[DRT_LEN + 1] = {'\0'};
	char drtFullFilePath[DRT_FULLFILEPATH_SZ + 1] = {'\0'};
	ll_node_t *head = NULL;
	ll_node_t *walker = NULL;
	FORM *formDelDRT = NULL;
	WINDOW *thisScreen = NULL;
	WINDOW *formScreen = NULL;
	FIELD *dtrToDelete[3] = {NULL, NULL, NULL};

	if(screen_drawDefaultTheme(&thisScreen, SRC_DELDRT_MAX_LINES, SRC_DELDRT_MAX_COLS, "Delete DRT") == PAINEL_NOK){
		return(NULL);
	}

	dtrToDelete[0] = new_field(1, 4, 1, 1, 0, 0);
	dtrToDelete[1] = new_field(1, 10, 1, 6, 0, 0);
	dtrToDelete[2] = NULL;

	set_field_buffer(dtrToDelete[0], 0, "DRT:");
	set_field_opts(dtrToDelete[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

	set_field_back(dtrToDelete[1], A_UNDERLINE);
	set_field_opts(dtrToDelete[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	formDelDRT = new_form(dtrToDelete);
	formScreen = derwin(thisScreen, SRC_DELDRT_MAX_LINES - 4, SRC_DELDRT_MAX_COLS - 2, 3, 1);

	set_form_win(formDelDRT, thisScreen);
	set_form_sub(formDelDRT, formScreen);

	post_form(formDelDRT);

	curs_set(1);

	wrefresh(thisScreen);
	wrefresh(formScreen);

	while((ch = getch()) != ESC_KEY){

		switch(ch){
			case KEY_ENTER:
			case 10: /* ENTER */
				strncpy(drtToDelete, field_buffer(dtrToDelete[1], 0), DRT_LEN);
				break;

			case KEY_BACKSPACE:
			case 127:
				form_driver(formDelDRT, REQ_DEL_PREV);
				break;

			case KEY_DC:
				form_driver(formDelDRT, REQ_DEL_CHAR);
				break;

			case KEY_LEFT:
				form_driver(formDelDRT, REQ_PREV_CHAR);
				break;

			case KEY_RIGHT:
				form_driver(formDelDRT, REQ_NEXT_CHAR);
				break;

			default:
				form_driver(formDelDRT, ch);
				break;
		}

		wrefresh(formScreen);
	}

	curs_set(0);

	if(ch == ESC_KEY){
		/* Cleanup... */
		goto CLEANUP_SCREEN_DELDRT;
	}

	snprintf(drtFullFilePath, DRT_FULLFILEPATH_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), SUBPATH_RUNNING_DATA_NCCLI, DRT_FILE);
	logWrite(&log, LOGDEV, "Opening UserId (DRT) file to delete: [%s].\n", drtFullFilePath);

	if(loadUserIdFileToMemory(&head, drtFullFilePath) == PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Erro carregando lista do arquivo de DRT.\n");
		ll_destroyList(&head, 1);
		return(NULL);
	}

	alltrim(drtToDeleteInput, drtToDelete, DRT_LEN);

	/* TODO: change code below to ll_node_t * ll_search(ll_node_t **head, ll_node_t *search, int (*searchNodeFunc)(void *e1, void *e2)) and ll_getData() */
	walker = searchLLUserDRT(head, drtToDelete, DRT_LEN);

	/* DRT found */
	if(walker != NULL){
		char userIdNewFullPath[DRT_FULLFILEPATH_SZ        + 1] = {'\0'};
		char userIdTempBkpNewFullPath[DRT_FULLFILEPATH_SZ + 1] = {'\0'};

		memset(userIdNewFullPath,        '\0', DRT_FULLFILEPATH_SZ + 1);
		memset(userIdTempBkpNewFullPath, '\0', DRT_FULLFILEPATH_SZ + 1);

		mvwprintw(formScreen, 3, 1, "Excluir DRT: [%s] funcao: [%s]? (s/N)", ((userId_t *)(walker->data))->userId, ((userId_t *)(walker->data))->level);
		ch = getch();

		if(ch == 'S' || ch == 's'){
			logWrite(&log, LOGDEV, "Deleting DRT [%s].\n", drtToDelete);
			ll_delete(&head, walker, 1);

			snprintf(userIdNewFullPath, DRT_FULLFILEPATH_SZ, "%s/%s/%s_TEMP_NEW", getPAINELEnvHomeVar(), SUBPATH_RUNNING_DATA_NCCLI, DRT_FILE);
			logWrite(&log, LOGDEV, "Dumping new temp UserId (DRT) file: [%s].\n", userIdNewFullPath);

			if(dumpUserIdMemoryFromFile(&head, userIdNewFullPath) == PAINEL_NOK){
				logWrite(&log, LOGOPALERT, "Erro dumping new userId list to file! Aborting (DRT [%s] didnt delete)!\n", drtToDelete);

				mvwprintw(formScreen, 5, 1, "Problema em gravar nova DRT [%s] para arquivo temp: [%s]. Pausa.", drtToDelete, userIdNewFullPath);
				getch();

				/* Cleanup... */
				goto CLEANUP_SCREEN_DELDRT;
			}

			/* TODO do test ------------------------------------------------------ */

			snprintf(userIdTempBkpNewFullPath, DRT_FULLFILEPATH_SZ, "%s/%s/%s_%s", getPAINELEnvHomeVar(), SUBPATH_RUNNING_DATA_NCCLI, DRT_FILE, time_DDMMYYhhmmss());
			logWrite(&log, LOGDEV, "Backuping current UserId (DRT) file [%s] to [%s].\n", userIdNewFullPath, userIdTempBkpNewFullPath);

			if(rename(drtFullFilePath, userIdNewFullPath) == -1){
				mvwprintw(formScreen, 5, 1, "Pausa 1."); /* TODO */
				logWrite(&log, LOGOPALERT, "[%s]\n", strerror(errno)); /* TODO */
				getch();

				/* Cleanup... */
				goto CLEANUP_SCREEN_DELDRT;
			}

			logWrite(&log, LOGDEV, "Renaming new temp file [%s] to default name [%s].\n", userIdNewFullPath, drtFullFilePath);

			if(rename(userIdTempBkpNewFullPath, drtFullFilePath) == -1){
				mvwprintw(formScreen, 5, 1, "Pausa 2."); /* TODO */
				logWrite(&log, LOGOPALERT, "[%s]\n", strerror(errno)); /* TODO */
				getch();

				/* Cleanup... */
				goto CLEANUP_SCREEN_DELDRT;
			}
		}
	}else{
		mvwprintw(formScreen, 3, 1, "DRT [%s] nao localizada! Pausa.", ((userId_t *)(walker->data))->userId);
		getch();
	}

	/* Cleanup... I will not create a function with 5 parameters ... Yes, that is a GOTO, but used with love and correctness */
CLEANUP_SCREEN_DELDRT:
	ll_destroyList(&head, 1);
	unpost_form(formDelDRT);
	free_form(formDelDRT);
	free_field(dtrToDelete[0]);
	free_field(dtrToDelete[1]);
	delwin(formScreen);
	delwin(thisScreen);

	return(screen_menu);
}

a2gs_ToolBox_WizardReturnFunc_t screen_menu(void *data)
{
#define SCREEN_MENU_TOTAL_OPTS (9)
	WINDOW *thisScreen = NULL;
	unsigned int i = 0;
	int opt = 0;
	a2gs_ToolBox_WizardReturnFunc_t nextScreen = NULL;
	char *menus[SCREEN_MENU_TOTAL_OPTS] = {"1) Fazer login e entrar registros", "", "2) Listar DRTs cadastradas nesta estacao", "3) Adicionar uma nova DRT nesta estacao", "4) Remover DRT nesta estacao", "", "5) Client config", "", "0) EXIT"};

	if(screen_drawDefaultTheme(&thisScreen, 18, 46, "MENU") == PAINEL_NOK){
		return(NULL);
	}

	drawKeyBar("Use keys [UP] or [DOWN] and [ENTER] to select the option");

	while(1){

		for(i = 0; i < SCREEN_MENU_TOTAL_OPTS; i++){
			mvwprintw(thisScreen, 4+i, 2, menus[i]);
		}

		break;

	}

	mvwprintw(thisScreen, 15, 2, "Press option number (menu not enabled yet)");

	wrefresh(thisScreen);

	do{
		opt = getch();

		switch(opt){

			case '1':
				nextScreen = screen_login;
				break;

			case '2':
				nextScreen = screen_listDRT;
				break;

			case '3':
				nextScreen = screen_addDRT;
				break;

			case '4':
				nextScreen = screen_delDRT;
				break;

			case '5':
				nextScreen = screen_config;
				break;

			case '0':
				nextScreen = NULL;
				break;

			default:
				opt = ERR;
				break;
		}
	}while(opt == ERR);

	delwin(thisScreen);

	return(nextScreen);
}

/* int main(int argc, char *argv[])
 *
 * INPUT:
 * OUTPUT (to SO):
 */
int main(int argc, char *argv[])
{
	int cursor = 0;
	unsigned int cfgLineError = 0;
	char *cfgServerAddress = NULL;
	char *cfgServerPort    = NULL;
	char *cfgLogFile       = NULL;
	char *cfgLogLevel      = NULL;
	cfgFile_t nccCfg;
	a2gs_ToolBox_WizardReturnFunc_t initFunc = NULL;

/*
	if(argc != 5){
		fprintf(stderr, "[%s %d] Usage:\n%s <IP_ADDRESS> <PORT> <FULL_LOG_PATH> <LOG_LEVEL 'WWW|XXX|YYY|ZZZ'>\n\n", time_DDMMYYhhmmss(), getpid(), argv[0]);
		fprintf(stderr, "Where WWW, XXX, YYY and ZZZ are a combination (surrounded by \"'\" and separated by \"|\") of: REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\n");
		fprintf(stderr, "\tREDALERT = Red alert\n");
		fprintf(stderr, "\tDBALERT = Database alert\n");
		fprintf(stderr, "\tDBMSG = Database message\n");
		fprintf(stderr, "\tOPALERT = Operation alert\n");
		fprintf(stderr, "\tOPMSG = Operation message\n");
		fprintf(stderr, "\tMSG = Just a message\n");
		fprintf(stderr, "\tDEV = Developer (DEBUG) message\n\n");
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}
*/
	if(argc != 2){
		fprintf(stderr, "[%s %d] Usage:\n%s <CONFIG_FILE>\n\n", time_DDMMYYhhmmss(), getpid(), argv[0]);
		fprintf(stderr, "CONFIG_FILE sample variables:\n");
		fprintf(stderr, "\t#PAINE_SERVER_ADDRESS = 123.123.123.123\n");
		fprintf(stderr, "\tPAINE_SERVER_ADDRESS = painel.server\n");
		fprintf(stderr, "\tPAINE_SERVER_PORT = 9998\n");
		fprintf(stderr, "\tLOG_FILE = ncclient.log\n");
		fprintf(stderr, "\t#Log levels:\n");
		fprintf(stderr, "\t#REDALERT = Red alert\n");
		fprintf(stderr, "\t#DBALERT = Database alert\n");
		fprintf(stderr, "\t#DBMSG = Database message\n");
		fprintf(stderr, "\t#OPALERT = Operation alert\n");
		fprintf(stderr, "\t#OPMSG = Operation message\n");
		fprintf(stderr, "\t#MSG = Just a message\n");
		fprintf(stderr, "\t#DEV = Developer (DEBUG) message\n");
		fprintf(stderr, "\tLOG_LEVEL = REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\n");
		fprintf(stderr, "\tPAINEL_PASSPHRASE = abcdefghijlmnopqrstuvxz\n\n");
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}

	if(cfgFileLoad(&nccCfg, argv[1], &cfgLineError) == CFGFILE_NOK){
		fprintf(stderr, "Error open/loading (at line: [%d]) configuration file [%s]: [%s].\n", cfgLineError, argv[1], strerror(errno));
		return(-2);
	}

	if(cfgFileOpt(&nccCfg, "PAINE_SERVER_ADDRESS", &cfgServerAddress) == CFGFILE_NOK){
		printf("Config with label PAINE_SERVER_ADDRESS not found into file [%s]! Exit.\n", argv[1]);
		return(-3);
	}

	if(cfgFileOpt(&nccCfg, "PAINE_SERVER_PORT", &cfgServerPort) == CFGFILE_NOK){
		printf("Config with label PAINE_SERVER_PORT not found into file [%s]! Exit.\n", argv[1]);
		return(-4);
	}

	if(cfgFileOpt(&nccCfg, "LOG_FILE", &cfgLogFile) == CFGFILE_NOK){
		printf("Config with label LOG_FILE not found into file [%s]! Exit.\n", argv[1]);
		return(-5);
	}

	if(cfgFileOpt(&nccCfg, "LOG_LEVEL", &cfgLogLevel) == CFGFILE_NOK){
		printf("Config with label LOG_LEVEL not found into file [%s]! Exit.\n", argv[1]);
		return(-6);
	}

	if(logCreate(&log, cfgLogFile, cfgLogLevel) == LOG_NOK){                                                         
		fprintf(stderr, "[%s %d] Erro criando log! [%s]. Terminating application with ERRO.\n", time_DDMMYYhhmmss(), getpid(), (errno == 0 ? "Level parameters error" : strerror(errno)));
		return(-7);
	}

	strncpy(userLogged, "Not Logged", USERLOGGED_SZ);

	getLogSystem_Util(&log);
	getLogSystem_UtilNetwork(&log);

	strncpy(serverAddress, cfgServerAddress, SERVERADDRESS_SZ);
	strncpy(serverPort,    cfgServerPort,    SERVERPORT_SZ);

	if(cfgFileFree(&nccCfg) == CFGFILE_NOK){
		printf("Error at cfgFileFree().\n");
		return(-8);
	}

	logWrite(&log, LOGMUSTLOGIT, "StartUp nClient [%s]! Server: [%s] Port: [%s] PAINEL Home: [%s].\n", time_DDMMYYhhmmss(), serverAddress, serverPort, getPAINELEnvHomeVar());

	if(pingServer(serverAddress, serverPort) == PAINEL_OK){
		logWrite(&log, LOGOPMSG, "Ping response from [%s:%s] ok. Going to main menu screen.\n", serverAddress, serverPort);
		initFunc = screen_menu;
	}else{
		logWrite(&log, LOGOPALERT, "Unable to ping server [%s:%s]. Going to config screen.\n", serverAddress, serverPort);
		initFunc = screen_config;
	}

	if(initscr() == NULL){;
		printf("Erro initializating ncurses!\n");
		logClose(&log);
		return(-9);
	}

	keypad(stdscr, TRUE);
	cbreak();
	noecho();
	set_escdelay(0);

	/* https://invisible-island.net/ncurses/ncurses-intro.html */
	/* signal(SIGWINCH, signalHandle); */
	/* TODO: configurar o SCREEN RESIZE para nao ser recebido como user input: KEY_RESIZE */

	if((LINES < 24) || (COLS < 80)){
		endwin();
		printf("O terminal precisa ter no minimo 80x24");
		logClose(&log);
		return(-10);
	}

	if(has_colors() == FALSE){
		endwin();
		printf("Terminal nao suporta cores (has_colors() = FALSE).\n");
		logClose(&log);
		return(-11);
	}

	if(can_change_color() == FALSE){
		endwin();
		printf("Terminal nao suporta mudar as cores (can_change_colors() = FALSE).\n");
		logClose(&log);
		return(-12);
	}

	if(start_color() != OK){
		endwin();
		printf("Erro em iniciar cores (start_colors() = FALSE).\n");
		logClose(&log);
		return(-13);
	}

	set_escdelay(0);

	cursor = curs_set(0);

	a2gs_ToolBox_WizardReturn(initFunc, NULL);

	curs_set(cursor);

	endwin();
	delwin(stdscr);

	logClose(&log);

	return(0);
}
