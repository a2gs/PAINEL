/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* addUser.c
 * Add a user to USERS table
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 08/03/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sqlite3.h>

#include "db.h"
#include "SG.h"
#include "util.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define SZ_SQLCMD					(5000)


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
int listUsersFunctions(char *DBPath)
{
	sqlite3 *db = NULL;
	char *err_msg = NULL;
	int rc = 0;
	char sql[SZ_SQLCMD + 1] = {'\0'};

	printf("Banco de dados: [%s]\n", DBPath);
	printf("Listagem de funcoes disponiveis:\n");
	printf("NULL");

	return(OK);
}

int dBAddUser(char *user, char *func, char *pass, char *dbPath)
{
	sqlite3 *db = NULL;
	char *err_msg = NULL;
	int rc = 0;
	char sql[SZ_SQLCMD + 1] = {'\0'};


	return(OK);
}

int main(int argc, char *argv[])
{
	char DBPath[DB_PATHFILE_SZ + 1] = {'\0'};
	char user[DRT_LEN + 1] = {'\0'};
	char func[VALOR_FUNCAO_LEN + 1] = {'\0'};
	char pass[PASS_SHA256_LEN + 1] = {'\0'};

	snprintf(DBPath, DB_PATHFILE_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), DATABASE_PATH, DATABASE_FILE);

	if((argc == 2) && (strncmp("-f", argv[1], 2) == 0)){
		listUsersFunctions(DBPath);
		return(0);
	}else if(argc == 4){
		strncpy(user,argv[1], DRT_LEN);
		strncpy(func,argv[2], VALOR_FUNCAO_LEN);
		strncpy(pass, argv[3], PASS_SHA256_LEN);

		if(dBAddUser(user, func, pass, DBPath) == NOK){
			fprintf(stderr, "erro...\n");
			return(-2);
		}

	}else if(argc != 1){
		fprintf(stderr, "Execucao:\n%s [-f] [<USER DRT> <FUNC (-f)> <PASSWORD>]\n", argv[0]);
		fprintf(stderr, "\t<USER DRT> <FUNC (-f)> <PASSWORD>\t\tUsuario (DRT), funcao (conforme listada em -f) e senha\n");
		fprintf(stderr, "\t-f\t\tListagem de funcoes\n");
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}

	printf("Usuario (DRT): ");
	fgets(user, DRT_LEN, stdin);

	printf("Funcao ......: ");
	fgets(func, VALOR_FUNCAO_LEN, stdin);

	printf("Senha .......: ");
	fgets(pass, PASS_SHA256_LEN, stdin);


	if(dBAddUser(user, func, pass, DBPath) == NOK){
		return(-3);
	}

#if 0


	for(;;){
		memset(&pageInfo, 0, sizeof(pageInfos_t));

		rc = sqlite3_enable_shared_cache(1);
		if(rc != SQLITE_OK){
			if(rc == SQLITE_BUSY){
				fprintf(stderr, "SQLITE_BUSY [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else if(rc == SQLITE_LOCKED){
				fprintf(stderr, "SQLITE_LOCKED [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
				fprintf(stderr, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else{
				fprintf(stderr, "Another error [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}

			fprintf(stderr, "Cannot enable shared cache database [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			return(1);
		}

		/*rc = sqlite3_open_v2(DBPath, &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX, NULL);*/
		/*rc = sqlite3_open_v2(DBPath, &db, SQLITE_OPEN_READONLY|SQLITE_OPEN_FULLMUTEX, NULL);*/
		rc = sqlite3_open_v2(DBPath, &db, SQLITE_OPEN_READONLY|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE, NULL);
    
		if(rc != SQLITE_OK){
			if(rc == SQLITE_BUSY){
				fprintf(stderr, "SQLITE_BUSY [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else if(rc == SQLITE_LOCKED){
				fprintf(stderr, "SQLITE_LOCKED [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
				fprintf(stderr, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else{
				fprintf(stderr, "Another error [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}

			fprintf(stderr, "Cannot open database [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			sqlite3_close(db);
        
			return(-2);
		}

		memset(sql, '\0', sizeof(sql));
		snprintf(sql, SZ_SQLCMD, "SELECT TITULO, CAMPOS, HEADERS FROM RELATS WHERE FUNCAO = '%s'", funcao);

		rc = sqlite3_exec(db, sql, hmtl_relat_infos, &pageInfo, &err_msg);
    
		if(rc != SQLITE_OK){
			if(rc == SQLITE_BUSY){
				fprintf(stderr, "SQLITE_BUSY [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else if(rc == SQLITE_LOCKED){
				fprintf(stderr, "SQLITE_LOCKED [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
				fprintf(stderr, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else{
				fprintf(stderr, "Another error [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}

			fprintf(stderr, "Failed to select data RELAT [%s].\n", sql);
			fprintf(stderr, "CMD: [%s]\nSQL error: [%s]\n", sql, err_msg);

			sqlite3_free(err_msg);
			sqlite3_close(db);
        
			return(-3);
		} 

		memset(&htmls, 0, sizeof(htmlFiles_t));
		if(html_fopen(&htmls, fHtmlStatic, fHtmlRefresh) == NOK){
			fprintf(stderr, "Falha em abrir/criar arquivos htmls [%s] e [%s].\n", fHtmlStatic, fHtmlRefresh);
			return(-4);
		}

		if(html_header(&htmls, pageInfo.title, segRefresh) == NOK){
			fprintf(stderr, "Falha em escrever header para arquivos htmls [%s] e [%s].\n", fHtmlStatic, fHtmlRefresh);
			return(-5);
		}

		if(html_startTable(&htmls, pageInfo.columnsHeaders) == NOK){
			fprintf(stderr, "Falha em escrever tabela para arquivos htmls [%s] e [%s].\n", fHtmlStatic, fHtmlRefresh);
			return(-6);
		}

		memset(sql, '\0', sizeof(sql));

		if(strncmp(funcao, "All", 3) == 0) snprintf(sql, SZ_SQLCMD, "SELECT %s FROM MSGS", pageInfo.columnsTable);
		else                               snprintf(sql, SZ_SQLCMD, "SELECT %s FROM MSGS WHERE FUNCAO = '%s'", pageInfo.columnsTable, funcao);

		rc = sqlite3_exec(db, sql, hmtl_constructTable, &htmls, &err_msg);
    
		if(rc != SQLITE_OK){
			if(rc == SQLITE_BUSY){
				fprintf(stderr, "SQLITE_BUSY [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else if(rc == SQLITE_LOCKED){
				fprintf(stderr, "SQLITE_LOCKED [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
				fprintf(stderr, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else{
				fprintf(stderr, "Another error [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}

			fprintf(stderr, "Failed to select data FUNCAO [%s]\n", funcao);
			fprintf(stderr, "CMD: [%s]\nSQL error: [%s]\n", sql, err_msg);

			sqlite3_free(err_msg);
			sqlite3_close(db);
        
			return(-7);
		} 
    
		if(html_endTable(&htmls) == NOK){
			fprintf(stderr, "Falha em escrever finalizar arquivos htmls [%s] e [%s].\n", fHtmlStatic, fHtmlRefresh);
			return(-9);
		}

		html_fflush(&htmls);
		html_fclose(&htmls);

		if(sqlite3_close_v2(db) != SQLITE_OK){
			if(rc == SQLITE_BUSY){
				fprintf(stderr, "SQLITE_BUSY [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else if(rc == SQLITE_LOCKED){
				fprintf(stderr, "SQLITE_LOCKED [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else if(rc == SQLITE_LOCKED_SHAREDCACHE){
				fprintf(stderr, "SQLITE_LOCKED_SHAREDCACHE [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}else{
				fprintf(stderr, "Another error [%s]: [%s]\n", DBPath, sqlite3_errmsg(db));
			}

			fprintf(stderr, "SQL close error!\nCMD: [%s]\nSQL error: [%s]\n", sql, err_msg);
			return(-10);
		}

		sleep(segReaload);
	}
#endif

	return(0);
}
