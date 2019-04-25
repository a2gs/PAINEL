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
#define STATUS_BAR_2                         "[TAB] Next field | [ESC] Exit"

#define STATUS_BAR_1_LINE_N                  (LINES - 1)
#define STATUS_BAR_1_COL_N(__fmt_bar1_srt__) ((COLS - strlen(__fmt_bar1_srt__))/2)

#define STATUS_BAR_2_LINE_N                  (LINES - 2)
#define STATUS_BAR_2_COL_N                   ((COLS - sizeof(STATUS_BAR_2))/2)

#define FMT_STATUS_BAR_1_SZ                  (100)

#define ESC_KEY                              (27)

a2gs_ToolBox_WizardReturnFunc_t screen1(void *data);
a2gs_ToolBox_WizardReturnFunc_t screen2(void *data);
a2gs_ToolBox_WizardReturnFunc_t screen3(void *data);
a2gs_ToolBox_WizardReturnFunc_t screen4(void *data);


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
	mvwprintw(stdscr, STATUS_BAR_2_LINE_N, STATUS_BAR_2_COL_N, STATUS_BAR_2);

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

a2gs_ToolBox_WizardReturnFunc_t screen4(void *data)
{
	WINDOW *thisScreen = NULL;
	char screenTitle[200] = {0};
	/* int thisScreen_maxx = 0, thisScreen_maxy = 0; */

	clear();
	drawDefaultStatusBar();

	thisScreen = newwin(2+2, 5+5, (LINES/2)-2, (COLS/2)-5);
	box(thisScreen, 0, 0);

	/*
	thisScreen_maxx = getmaxx(thisScreen);
	thisScreen_maxy = getmaxy(thisScreen);
	*/

	formatTitle(screenTitle, 10-2, "Title 4");
	wattron(thisScreen, A_REVERSE);
	mvwprintw(thisScreen, 1, 1, screenTitle);
	wattroff(thisScreen, A_REVERSE);


	/* ... */


	wrefresh(thisScreen);

	getch();

	delwin(thisScreen);

	return(NULL);
}

a2gs_ToolBox_WizardReturnFunc_t screen3(void *data)
{
	WINDOW *thisScreen = NULL;
	char screenTitle[200] = {0};
	/* int thisScreen_maxx = 0, thisScreen_maxy = 0; */

	clear();
	drawDefaultStatusBar();

	thisScreen = newwin(5+5, 10+10, (LINES/2)-5, (COLS/2)-10);
	box(thisScreen, 0, 0);

	/*
	thisScreen_maxx = getmaxx(thisScreen);
	thisScreen_maxy = getmaxy(thisScreen);
	*/

	formatTitle(screenTitle, 20-2, "Title 3");
	wattron(thisScreen, A_REVERSE);
	mvwprintw(thisScreen, 1, 1, screenTitle);
	wattroff(thisScreen, A_REVERSE);


	/* ... */


	wrefresh(thisScreen);

	getch();

	delwin(thisScreen);

	return(screen4);
}

a2gs_ToolBox_WizardReturnFunc_t screen2(void *data)
{
	WINDOW *thisScreen = NULL;
	char screenTitle[200] = {0};
	/* int thisScreen_maxx = 0, thisScreen_maxy = 0; */

	clear();
	drawDefaultStatusBar();

	thisScreen = newwin(10+10, 40+40, (LINES/2)-10, (COLS/2)-40);
	box(thisScreen, 0, 0);

	/*
	thisScreen_maxx = getmaxx(thisScreen);
	thisScreen_maxy = getmaxy(thisScreen);
	*/

	formatTitle(screenTitle, 80-2, "Title 2");
	wattron(thisScreen, A_REVERSE);
	mvwprintw(thisScreen, 1, 1, screenTitle);
	wattroff(thisScreen, A_REVERSE);


	/* ... */


	wrefresh(thisScreen);

	getch();

	delwin(thisScreen);

	return(screen3);
}

a2gs_ToolBox_WizardReturnFunc_t screen1(void *data)
{
	WINDOW *thisScreen = NULL;
	char screenTitle[200] = {0};
	/* int thisScreen_maxx = 0, thisScreen_maxy = 0; */

	clear();
	drawDefaultStatusBar();

	thisScreen = newwin(20+20, 60+60, (LINES/2)-20, (COLS/2)-60);
	box(thisScreen, 0, 0);

	/*
	thisScreen_maxx = getmaxx(thisScreen);
	thisScreen_maxy = getmaxy(thisScreen);
	*/

	formatTitle(screenTitle, 120-2, "Title 1");
	wattron(thisScreen, A_REVERSE);
	mvwprintw(thisScreen, 1, 1, screenTitle);
	wattroff(thisScreen, A_REVERSE);


	/* ... */


	wrefresh(thisScreen);

	getch();

	delwin(thisScreen);

	return(screen2);
}

int pingServer(char ip, int port)
{
	return(OK);
}

a2gs_ToolBox_WizardReturnFunc_t screen_config(void *data)
{
	return(NULL);
}

a2gs_ToolBox_WizardReturnFunc_t screen_menu(void *data)
{
	return(NULL);
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
		if(pingServer == OK)
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
