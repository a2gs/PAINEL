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
#include <string.h>
#include <signal.h>
#include <ncurses.h>

#include <util.h>

#include "wizard_by_return.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define STATUS_BAR_1                         "Connected [%s] User [%s]"

#define STATUS_BAR_1_LINE_N                  (LINES - 1)
#define STATUS_BAR_1_COL_N(__fmt_bar1_srt__) ((COLS - strlen(__fmt_bar1_srt__))/2)

#define STATUS_BAR_2_LINE_N                  (LINES - 2)

#define FMT_STATUS_BAR_1_SZ                  (100)

#define ESC_KEY                              (27)


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

	snprintf(fmt_STATUS_BAR_1, FMT_STATUS_BAR_1_SZ, STATUS_BAR_1, "localhost:666", "Unknow");
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

a2gs_ToolBox_WizardReturnFunc_t screen_config(void *data)
{
	WINDOW *thisScreen = NULL;
	char screenTitle[200] = {0};
	/* int thisScreen_maxx = 0, thisScreen_maxy = 0; */

	clear();
	drawDefaultStatusBar();
	/* drawKeyBar(""); */

	thisScreen = newwin(20+20, 60+60, (LINES/2)-20, (COLS/2)-60);
	box(thisScreen, 0, 0);

	/*
	thisScreen_maxx = getmaxx(thisScreen);
	thisScreen_maxy = getmaxy(thisScreen);
	*/

	formatTitle(screenTitle, 120-2, "Client Configuration");
	mvwaddch(thisScreen, 2, 0, ACS_LTEE);
	mvwaddch(thisScreen, 2, 120-1, ACS_RTEE);
	mvwhline(thisScreen , 2, 1, ACS_HLINE, 120-2);
	wattron(thisScreen, A_REVERSE);
	mvwprintw(thisScreen, 1, 1, screenTitle);
	wattroff(thisScreen, A_REVERSE);

	/* ... */


	wrefresh(thisScreen);

	getch();

	delwin(thisScreen);

	return(NULL);
}

a2gs_ToolBox_WizardReturnFunc_t screen_login(void *data)
{
	WINDOW *thisScreen = NULL;
	char screenTitle[200] = {0};
	/* int thisScreen_maxx = 0, thisScreen_maxy = 0; */

	clear();
	drawDefaultStatusBar();
	/* drawKeyBar(""); */

	thisScreen = newwin(20+20, 60+60, (LINES/2)-20, (COLS/2)-60);
	box(thisScreen, 0, 0);

	/*
	thisScreen_maxx = getmaxx(thisScreen);
	thisScreen_maxy = getmaxy(thisScreen);
	*/

	formatTitle(screenTitle, 120-2, "User Login (DRT)");
	mvwaddch(thisScreen, 2, 0, ACS_LTEE);
	mvwaddch(thisScreen, 2, 120-1, ACS_RTEE);
	mvwhline(thisScreen , 2, 1, ACS_HLINE, 120-2);
	wattron(thisScreen, A_REVERSE);
	mvwprintw(thisScreen, 1, 1, screenTitle);
	wattroff(thisScreen, A_REVERSE);

	/* ... */


	wrefresh(thisScreen);

	getch();

	delwin(thisScreen);

	return(NULL);
}

a2gs_ToolBox_WizardReturnFunc_t screen_listDRT(void *data)
{
	WINDOW *thisScreen = NULL;
	char screenTitle[200] = {0};
	/* int thisScreen_maxx = 0, thisScreen_maxy = 0; */

	clear();
	drawDefaultStatusBar();
	/* drawKeyBar(""); */

	thisScreen = newwin(20+20, 60+60, (LINES/2)-20, (COLS/2)-60);
	box(thisScreen, 0, 0);

	/*
	thisScreen_maxx = getmaxx(thisScreen);
	thisScreen_maxy = getmaxy(thisScreen);
	*/

	formatTitle(screenTitle, 120-2, "List DRTs");
	mvwaddch(thisScreen, 2, 0, ACS_LTEE);
	mvwaddch(thisScreen, 2, 120-1, ACS_RTEE);
	mvwhline(thisScreen , 2, 1, ACS_HLINE, 120-2);
	wattron(thisScreen, A_REVERSE);
	mvwprintw(thisScreen, 1, 1, screenTitle);
	wattroff(thisScreen, A_REVERSE);

	/* ... */


	wrefresh(thisScreen);

	getch();

	delwin(thisScreen);

	return(NULL);
}

a2gs_ToolBox_WizardReturnFunc_t screen_addDRT(void *data)
{
	WINDOW *thisScreen = NULL;
	char screenTitle[200] = {0};
	/* int thisScreen_maxx = 0, thisScreen_maxy = 0; */

	clear();
	drawDefaultStatusBar();
	/* drawKeyBar(""); */

	thisScreen = newwin(20+20, 60+60, (LINES/2)-20, (COLS/2)-60);
	box(thisScreen, 0, 0);

	/*
	thisScreen_maxx = getmaxx(thisScreen);
	thisScreen_maxy = getmaxy(thisScreen);
	*/

	formatTitle(screenTitle, 120-2, "Add DRT");
	mvwaddch(thisScreen, 2, 0, ACS_LTEE);
	mvwaddch(thisScreen, 2, 120-1, ACS_RTEE);
	mvwhline(thisScreen , 2, 1, ACS_HLINE, 120-2);
	wattron(thisScreen, A_REVERSE);
	mvwprintw(thisScreen, 1, 1, screenTitle);
	wattroff(thisScreen, A_REVERSE);

	/* ... */


	wrefresh(thisScreen);

	getch();

	delwin(thisScreen);

	return(NULL);
}

a2gs_ToolBox_WizardReturnFunc_t screen_delDRT(void *data)
{
	WINDOW *thisScreen = NULL;
	char screenTitle[200] = {0};
	/* int thisScreen_maxx = 0, thisScreen_maxy = 0; */

	clear();
	drawDefaultStatusBar();
	/* drawKeyBar(""); */

	thisScreen = newwin(20+20, 60+60, (LINES/2)-20, (COLS/2)-60);
	box(thisScreen, 0, 0);

	/*
	thisScreen_maxx = getmaxx(thisScreen);
	thisScreen_maxy = getmaxy(thisScreen);
	*/

	formatTitle(screenTitle, 120-2, "Delete DRT");
	mvwaddch(thisScreen, 2, 0, ACS_LTEE);
	mvwaddch(thisScreen, 2, 120-1, ACS_RTEE);
	mvwhline(thisScreen , 2, 1, ACS_HLINE, 120-2);
	wattron(thisScreen, A_REVERSE);
	mvwprintw(thisScreen, 1, 1, screenTitle);
	wattroff(thisScreen, A_REVERSE);

	/* ... */


	wrefresh(thisScreen);

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
	char screenTitle[200] = {0};
	a2gs_ToolBox_WizardReturnFunc_t nextScreen = NULL;
	char *menus[SCREEN_MENU_TOTAL_OPTS] = {"1) Fazer login", "", "2) Listar DRTs cadastradas nesta estacao", "3) Adicionar uma nova DRT nesta estacao", "4) Remover DRT nesta estacao", "", "5) Client config", "", "0) EXIT"};
	/* int thisScreen_maxx = 0, thisScreen_maxy = 0; */

	clear();
	drawDefaultStatusBar();
	drawKeyBar("Use keys [UP] or [DOWN] and [ENTER] to select the option");

	thisScreen = newwin(20+20, 60+60, (LINES/2)-20, (COLS/2)-60);
	box(thisScreen, 0, 0);

	/*
	thisScreen_maxx = getmaxx(thisScreen);
	thisScreen_maxy = getmaxy(thisScreen);
	*/

	/* Windows title */
	formatTitle(screenTitle, 120-2, "MENU");
	mvwaddch(thisScreen, 2, 0, ACS_LTEE);
	mvwaddch(thisScreen, 2, 120-1, ACS_RTEE);
	mvwhline(thisScreen , 2, 1, ACS_HLINE, 120-2);
	wattron(thisScreen, A_REVERSE);
	mvwprintw(thisScreen, 1, 1, screenTitle);
	wattroff(thisScreen, A_REVERSE);


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

	if(argc != 3){
		initFunc = screen_config;
	}else{
		if(pingServer(argv[1], argv[2]) == PAINEL_OK)
			initFunc = screen_menu;
		else
			initFunc = screen_config;
	}

	if(initscr() == NULL){;
		printf("Erro initializating ncurses!\n");
		return(-1);
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
		return(-2);
	}

	if(has_colors() == FALSE){
		endwin();
		printf("Terminal nao suporta cores (has_colors() = FALSE).\n");
		return(-2);
	}

	if(can_change_color() == FALSE){
		endwin();
		printf("Terminal nao suporta mudar as cores (can_change_colors() = FALSE).\n");
		return(-3);
	}

	if(start_color() != OK){
		endwin();
		printf("Erro em iniciar cores (start_colors() = FALSE).\n");
		return(-4);
	}

	set_escdelay(0);

	cursor = curs_set(0);

	a2gs_ToolBox_WizardReturn(initFunc, NULL);

	curs_set(cursor);

	endwin();
	delwin(stdscr);

	return(0);
}
