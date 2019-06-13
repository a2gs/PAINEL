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
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "util.h"
#include "SG.h"
#include "userId.h"

#include <log.h>
#include <sha-256.h>
#include <wizard_by_return.h>
#include <linkedlist.h>


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

#define USR_IFACE_TOTAL_FIELDS               (10) /* IFACE cmd. At first version, only 10 fields */

#define STR_USR_FIELD_TEXT  ("TEXT")
#define STR_USR_FIELD_NUM   ("NUM")
#define STR_USR_FIELD_DATE  ("DATE")

typedef enum{
	TEXT_USRFIELD = 1,
	NUM_USRFIELD,
	DATE_USRFIELD,
	UNDEFINED_USRFIELD
}usrFieldType_t;

typedef struct _usrField_t{ /* IFACE cmd. Dowloaded from server at correct order to display. Sizes below are fixed at first version. */
#define USR_IFACE_FIELD_SZ      (25)
#define USR_IFACE_FMTFIELD_SZ   (15)
#define USR_IFACE_DESCFIELD_SZ  (40)
	char field[USR_IFACE_FIELD_SZ + 1];
	usrFieldType_t type;
	char fmt[USR_IFACE_FMTFIELD_SZ + 1];
	char desc[USR_IFACE_DESCFIELD_SZ + 1];
}usrField_t;

typedef struct _usrFieldCtrl_t{ /* IFACE cmd. A dynamic list and self-interface (at userId.h) in futere versions. */
	unsigned int totFields;
	usrField_t fields[USR_IFACE_TOTAL_FIELDS];
}usrFieldCtrl_t;

static int sockfd = -1; /* Socket */
static log_t log;
static usrFieldCtrl_t usrIfaceFields;
static char serverAddress[SERVERADDRESS_SZ + 1] = {'\0'};
static char serverPort[SERVERPORT_SZ + 1] = {'\0'};
static char userLogged[USERLOGGED_SZ + 1] = {'\0'};


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */
a2gs_ToolBox_WizardReturnFunc_t screen_menu(void *data);


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */


/* -------------------------------------------------------------------------------------------------------- */

int isConnect(void)
{
	return((sockfd == -1) ? 0 : 1);
}

int disconnectSrvPainel(void)
{
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);

	sockfd = -1;

	return(PAINEL_OK);
}

int connectSrvPainel(char *srvAdd, char *srvPort)
{
	struct addrinfo hints, *res = NULL, *rp = NULL;
	int errGetAddrInfoCode = 0, errConnect = 0;
	void *pAddr = NULL;
	char strAddr[STRADDR_SZ + 1] = {'\0'};

	if(isConnect() == 1)
		disconnectSrvPainel();

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;

	errGetAddrInfoCode = getaddrinfo(srvAdd, srvPort, &hints, &res);
	if(errGetAddrInfoCode != 0){
		logWrite(&log, LOGOPALERT, "ERRO: getaddrinfo() [%s]. Terminating application with ERRO.\n\n", gai_strerror(errGetAddrInfoCode));
		logClose(&log);
		return(PAINEL_NOK);
	}

	for(rp = res; rp != NULL; rp = rp->ai_next){
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1){
			logWrite(&log, LOGOPALERT, "ERRO: socket() [%s].\n", strerror(errno));
			continue;
		}

		if(rp->ai_family == AF_INET)       pAddr = &((struct sockaddr_in *) rp->ai_addr)->sin_addr;
		else if(rp->ai_family == AF_INET6) pAddr = &((struct sockaddr_in6 *) rp->ai_addr)->sin6_addr;
		else                               pAddr = NULL;

		inet_ntop(rp->ai_family, pAddr, strAddr, STRADDR_SZ);
		logWrite(&log, LOGOPMSG, "Trying connect to [%s/%s:%s].\n", rp->ai_canonname, strAddr, srvPort);

		errConnect = connect(sockfd, rp->ai_addr, rp->ai_addrlen);
		if(errConnect == 0)
			break;

		logWrite(&log, LOGOPALERT, "ERRO: connect() to [%s/%s:%s] [%s].\n", rp->ai_canonname, strAddr, srvPort, strerror(errno));

		close(sockfd);
	}

	if(res == NULL || errConnect == -1){ /* End of getaddrinfo() list or connect() returned error */
		logWrite(&log, LOGOPALERT, "ERRO: Unable connect to any address. Terminating application with ERRO.\n\n");
		logClose(&log);
		return(-4);
	}

	freeaddrinfo(res);

	return(PAINEL_OK);
}

int usrIsIfaceFieldsEmpty(void)
{
	return((usrIfaceFields.totFields == 0) ? 0 : 1);
}

void usrIfaceFieldsClear(void)
{
	memset(&usrIfaceFields, 0, sizeof(usrFieldCtrl_t));
	usrIfaceFields.totFields = 0;
}

int usrIfaceFieldAdd(char *ffield, char *ftype, char *ffmt, char *fdesc)
{
	if(usrIfaceFields.totFields >= USR_IFACE_TOTAL_FIELDS)
		return(PAINEL_NOK);

	strncpy(usrIfaceFields.fields[usrIfaceFields.totFields].field, ffield, USR_IFACE_FIELD_SZ);
	strncpy(usrIfaceFields.fields[usrIfaceFields.totFields].fmt,   ffmt,   USR_IFACE_FMTFIELD_SZ);
	strncpy(usrIfaceFields.fields[usrIfaceFields.totFields].desc,  fdesc,  USR_IFACE_DESCFIELD_SZ);

	if     (strcmp(ftype, STR_USR_FIELD_TEXT) == 0) usrIfaceFields.fields[usrIfaceFields.totFields].type = TEXT_USRFIELD;
	else if(strcmp(ftype, STR_USR_FIELD_NUM ) == 0) usrIfaceFields.fields[usrIfaceFields.totFields].type = NUM_USRFIELD;
	else if(strcmp(ftype, STR_USR_FIELD_DATE) == 0) usrIfaceFields.fields[usrIfaceFields.totFields].type = DATE_USRFIELD;
	else                                usrIfaceFields.fields[usrIfaceFields.totFields].type = UNDEFINED_USRFIELD;

	usrIfaceFields.totFields++;

	return(PAINEL_OK);
}

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

	for(walker = head; walker != NULL; walker = walker->next){
		if(strncmp(drt, ((userId_t *)(walker->data))->userId, drtSz) == 0)
			return(walker);
	}

	return(NULL);
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
	}

	if(sendToNet(sockfd, msg, msgSzOut, &srError) == PAINEL_NOK){
	}

	if(recvFromNet(sockfd, msg, MAXLINE, &msgSzOut, &srError) == PAINEL_NOK){
	}

	return(PAINEL_OK);
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

	logWrite(&log, LOGDEV, "Login screen.\n");

	if(screen_drawDefaultTheme(&thisScreen, SRC_LOGIN_MAX_LINES, SRC_LOGIN_MAX_COLS, "User Login (DRT)") == PAINEL_NOK){
		return(NULL);
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
		goto CLEANUP_SCREEN_LOGIN;
	}

	while(1){
		post_form(formLogin);

		curs_set(1);

		wrefresh(thisScreen);
		wrefresh(formLoginScreen);

		ch = getch();

		if(ch == KEY_ENTER || ch == 10){
			char auxLogin[DRT_LEN          + 1] = {'\0'};
			char auxPass[PASS_LEN          + 1] = {'\0'};

			curs_set(0);

			alltrim(field_buffer(dtrLogin[1], 0), auxLogin, DRT_LEN);
			alltrim(field_buffer(dtrLogin[3], 0), auxPass,  PASS_LEN);

			walker = searchLLUserDRT(head, auxLogin, DRT_LEN);

			/* DRT found inside DRT_FILE */
			if(walker != NULL){
				char auxLevel[VALOR_FUNCAO_LEN + 1] = {'\0'};

				strncpy(auxLevel, userType_t_2_String(((userId_t *)walker->data)->level), VALOR_FUNCAO_LEN);

				if(sendLoginCmd(auxLogin, auxPass, auxLevel) == PAINEL_NOK){
				}

				/* TODO ------------------------------------------------------ */

			 /* if(Check if user are registred into DRTs.text) == OK{
			 *    if(check login to server == OK){
			 * 		User ok, get user IFACE cmd
			 * 		GO TO USER (defined) DYNAMIC SCREEN
			 * 	}
			 * }else{
			 *
			 * }
			 *
			 */

			}


			break;
		}

		if(ch == ESC_KEY) break;

		formDriver(formLoginScreen, formLogin, ch);
	}


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

	return(screen_menu);
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
	WINDOW *thisScreen = NULL;

	if(screen_drawDefaultTheme(&thisScreen, 40, 120, "Add DRT") == PAINEL_NOK){
		return(NULL);
	}


	/* ... */
	/* TODO ------------------------------------------------------ */


	getch();

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
	a2gs_ToolBox_WizardReturnFunc_t initFunc = NULL;

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

	if(logCreate(&log, argv[3], argv[4]) == LOG_NOK){                                                         
		fprintf(stderr, "[%s %d] Erro criando log! [%s]. Terminating application with ERRO.\n", time_DDMMYYhhmmss(), getpid(), (errno == 0 ? "Level parameters error" : strerror(errno)));
		return(-2);
	}

	strncpy(userLogged, "Not Logged", USERLOGGED_SZ);

	getLogSystem_Util(&log);

	strncpy(serverAddress, argv[1], SERVERADDRESS_SZ);
	strncpy(serverPort,    argv[2], SERVERPORT_SZ);

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
		return(-3);
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
		return(-4);
	}

	if(has_colors() == FALSE){
		endwin();
		printf("Terminal nao suporta cores (has_colors() = FALSE).\n");
		logClose(&log);
		return(-5);
	}

	if(can_change_color() == FALSE){
		endwin();
		printf("Terminal nao suporta mudar as cores (can_change_colors() = FALSE).\n");
		logClose(&log);
		return(-6);
	}

	if(start_color() != OK){
		endwin();
		printf("Erro em iniciar cores (start_colors() = FALSE).\n");
		logClose(&log);
		return(-7);
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
