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
#include <sys/types.h>


#include <util.h>
#include <SG.h>

#include <log.h>
#include "wizard_by_return.h"


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

static log_t log;
static char serverAddress[SERVERADDRESS_SZ + 1] = {'\0'};
static char serverPort[SERVERPORT_SZ + 1] = {'\0'};
static char userLogged[USERLOGGED_SZ + 1] = {'\0'};


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */


/* -------------------------------------------------------------------------------------------------------- */

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

	return(NULL);
}

a2gs_ToolBox_WizardReturnFunc_t screen_login(void *data)
{
	WINDOW *thisScreen = NULL;

	if(screen_drawDefaultTheme(&thisScreen, 40, 120, "User Login (DRT)") == PAINEL_NOK){
		return(NULL);
	}


	/* ... */


	getch();

	delwin(thisScreen);

	return(NULL);
}

a2gs_ToolBox_WizardReturnFunc_t screen_listDRT(void *data)
{
	WINDOW *thisScreen = NULL;

	if(screen_drawDefaultTheme(&thisScreen, 40, 120, "List DRTs") == PAINEL_NOK){
		return(NULL);
	}


	/* ... */


	getch();

	delwin(thisScreen);

	return(NULL);
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

	return(NULL);
}

a2gs_ToolBox_WizardReturnFunc_t screen_delDRT(void *data)
{
	WINDOW *thisScreen = NULL;

	if(screen_drawDefaultTheme(&thisScreen, 40, 120, "Delete DRT") == PAINEL_NOK){
		return(NULL);
	}


	/* ... */


	getch();

	delwin(thisScreen);

	return(NULL);
}

a2gs_ToolBox_WizardReturnFunc_t screen_menu(void *data)
{
#define SCREEN_MENU_TOTAL_OPTS (9)
	WINDOW *thisScreen = NULL;
	unsigned int i = 0;
	int opt = 0;
	a2gs_ToolBox_WizardReturnFunc_t nextScreen = NULL;
	char *menus[SCREEN_MENU_TOTAL_OPTS] = {"1) Fazer login", "", "2) Listar DRTs cadastradas nesta estacao", "3) Adicionar uma nova DRT nesta estacao", "4) Remover DRT nesta estacao", "", "5) Client config", "", "0) EXIT"};

	if(screen_drawDefaultTheme(&thisScreen, 40, 120, "MENU") == PAINEL_NOK){
		return(NULL);
	}

	drawKeyBar("Use keys [UP] or [DOWN] and [ENTER] to select the option");


	/* ... */


	while(1){

		for(i = 0; i < SCREEN_MENU_TOTAL_OPTS; i++){
			mvwprintw(thisScreen, 4+i, 2, menus[i]);
		}

		break;

	}

	mvwprintw(thisScreen, 18, 2, "Press option number (menu not enabled yet)");

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
