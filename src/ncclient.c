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
#include <sys/types.h>

#include "util.h"
#include "SG.h"
#include "userId.h"

#include <log.h>
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

	if     (strcmp(ftype, "TEXT") == 0) usrIfaceFields.fields[usrIfaceFields.totFields].type = TEXT_USRFIELD;
	else if(strcmp(ftype, "NUM" ) == 0) usrIfaceFields.fields[usrIfaceFields.totFields].type = NUM_USRFIELD;
	else if(strcmp(ftype, "DATE") == 0) usrIfaceFields.fields[usrIfaceFields.totFields].type = DATE_USRFIELD;
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

a2gs_ToolBox_WizardReturnFunc_t screen_config(void *data)
{
	WINDOW *thisScreen = NULL;

	if(screen_drawDefaultTheme(&thisScreen, 40, 120, "Client Configuration") == PAINEL_NOK){
		return(NULL);
	}


	/* ... */


	getch();

	delwin(thisScreen);

	return(screen_menu);
}

a2gs_ToolBox_WizardReturnFunc_t screen_login(void *data)
{
#define SRC_LOGIN_MAX_LINES (40)
#define SRC_LOGIN_MAX_COLS  (120)
	int ch = 0;
	WINDOW *thisScreen = NULL;
	WINDOW *formLoginScreen = NULL;
	FORM *formLoginDRT = NULL;
	FIELD *dtrLogin[5] = {NULL, NULL, NULL};

	logWrite(&log, LOGDEV, "Login screen.\n");

	if(screen_drawDefaultTheme(&thisScreen, SRC_LOGIN_MAX_LINES, SRC_LOGIN_MAX_COLS, "User Login (DRT)") == PAINEL_NOK){
		return(NULL);
	}

	dtrLogin[0] = new_field(1, 4, 1, 1, 0, 0);
	dtrLogin[1] = new_field(1, 10, 1, 6, 0, 0);
	dtrLogin[2] = new_field(2, 4, 1, 6, 0, 0);
	dtrLogin[3] = new_field(2, 10, 1, 6, 0, 0);
	dtrLogin[4] = NULL;

	set_field_buffer(dtrLogin[0], 0, "DRT:");
	set_field_opts(dtrLogin[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

	set_field_back(dtrLogin[1], A_UNDERLINE);
	set_field_opts(dtrLogin[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	set_field_buffer(dtrLogin[2], 0, "Password:");
	set_field_opts(dtrLogin[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

	set_field_back(dtrLogin[3], A_UNDERLINE);
	set_field_opts(dtrLogin[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

	formLoginDRT = new_form(dtrLogin);
	formLoginScreen = derwin(thisScreen, SRC_LOGIN_MAX_LINES - 4, SRC_LOGIN_MAX_COLS - 2, 3, 1);

	set_form_win(formLoginDRT, thisScreen);
	set_form_sub(formLoginDRT, formLoginScreen);

	post_form(formLoginDRT);

	curs_set(1);

	wrefresh(thisScreen);
	wrefresh(formLoginScreen);

	while((ch = getch()) != ESC_KEY){
	}


	getch();

	delwin(thisScreen);

	/* User ok, get user IFACE cmd */

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

	for(walker = head; walker != NULL; walker = walker->next){
		if(strncmp(drtToDelete, ((userId_t *)(walker->data))->userId, DRT_LEN) == 0)
			break;
	}

	/* DRT found */
	if(walker != NULL){
		char userIdNewFullPath[DRT_FULLFILEPATH_SZ + 1] = {'\0'};
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

			snprintf(userIdTempBkpNewFullPath, DRT_FULLFILEPATH_SZ, "%s/%s/%s_%s", getPAINELEnvHomeVar(), SUBPATH_RUNNING_DATA_NCCLI, DRT_FILE, time_DDMMYYhhmmss());
			logWrite(&log, LOGDEV, "Backuping current UserId (DRT) file [%s] to [%s].\n", userIdNewFullPath, userIdTempBkpNewFullPath);

			if(rename(drtFullFilePath, userIdNewFullPath) == -1){
				mvwprintw(formScreen, 5, 1, "Pausa."); /* TODO */
				logWrite(&log, LOGOPALERT, "[%s]\n", strerror(errno)); /* TODO */
				getch();

				/* Cleanup... */
				goto CLEANUP_SCREEN_DELDRT;
			}

			logWrite(&log, LOGDEV, "Renaming new temp file [%s] to default name [%s].\n", userIdNewFullPath, drtFullFilePath);

			if(rename(userIdTempBkpNewFullPath, drtFullFilePath) == -1){
				mvwprintw(formScreen, 5, 1, "Pausa."); /* TODO */
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

	strncpy(serverAddress, "no_server", SERVERADDRESS_SZ);
	strncpy(serverPort, "0000", SERVERPORT_SZ);
	strncpy(userLogged, "Not Logged", USERLOGGED_SZ);

	getLogSystem_Util(&log);

	logWrite(&log, LOGMUSTLOGIT, "StartUp nClient [%s]! Server: [%s] Port: [%s] PAINEL Home: [%s].\n", time_DDMMYYhhmmss(), argv[1], argv[2], getPAINELEnvHomeVar());

	if(pingServer(argv[1], argv[2]) == PAINEL_OK){
		logWrite(&log, LOGOPMSG, "Ping response from [%s:%s] ok. Going to main menu screen.\n", argv[1], argv[2]);
		strncpy(serverAddress, argv[1], SERVERADDRESS_SZ);
		strncpy(serverPort, argv[2], SERVERPORT_SZ);
		initFunc = screen_menu;
	}else{
		logWrite(&log, LOGOPALERT, "Unable to ping server [%s:%s]. Going to config screen.\n", argv[1], argv[2]);
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
