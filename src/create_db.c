/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* create_db.c
 * Just to create SQLite databases (an auxiliary tool).
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <sqlite3.h>

#include "util.h"
#include "db.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
/* int main(int argc, char *argv[])
 *
 * creadte_db starts.
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  OK - Ok
 *  NOK - Error
 */
int main(int argc, char *argv[])
{
	char DBPath[DB_PATHFILE_SZ + 1] = {'\0'};

	snprintf(DBPath, DB_PATHFILE_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), DATABASE_PATH, DATABASE_FILE);
	fprintf(stderr, "Criando banco: [%s]\n", DBPath);


	if(createAllTables(DBPath) == NOK){
		fprintf(stderr, "ERRO criando banco de dados (tabelas) SQLite!\n");
		return(-1);
	}

	fprintf(stderr, "Banco de dados (tabelas) SQLite criado!\n");

	return(0);
}
