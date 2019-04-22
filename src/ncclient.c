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
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */
static int cursor = 0;
static WINDOW *mainWindow = NULL;


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */


/* -------------------------------------------------------------------------------------------------------- */

/* int main(int argc, char *argv[])
 *
 * ncurses client starts.
 *
 * INPUT:
 *  argv[1] - Server IP address
 *  argv[2] - Server port
 * OUTPUT (to SO):
 *  0 - Ok
 *  !0 - Error (see log)
 */
int main(int argc, char *argv[])
{
	mainWindow = initscr();
	keypad(stdscr, TRUE);
	cbreak();
	noecho();

	if(mainWindow == NULL){
		printf("Erro initializating ncurses!\n");
		return(-1);
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

	if(argc != 5){
		/* Call screen to fill <IP_ADDRESS> <PORT> <FULL_LOG_PATH> <LOG_LEVEL 'WWW|XXX|YYY|ZZZ'> */
	}

	cursor = curs_set(0);





	clear();
	mvaddstr(10, 10, "Underconstruction");
	refresh();
	getch();






	curs_set(cursor);
	endwin();

	return(0);
}
