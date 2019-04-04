/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* userId.c
 * Add/remove a user to USERS table
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 08/03/2018 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>

#include "db.h"
#include "SG.h"
#include "SG_client.h"
#include "util.h"
#include "sha-256.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
/* int listUsersAndFuncs(void *htmlsVoid, int argc, char **argv, char **azColName)
 *
 * SQLite3 callback
 *
 * INPUT:
 * OUTPUT:
 */
int listUsersAndFuncs(void *htmlsVoid, int argc, char **argv, char **azColName)
{
	printf("%-*s%s\n",DRT_LEN, argv[0], argv[1]);

	return(0);
}

/* int listUsersAndYoursFunctions(char *DBPath)
 *
 * Print (screen) all users and yours levels (or office responsibility)
 *
 * INPUT:
 *  DBPath - Database full path
 * OUTPUT:
 *  OK - Printed (stdout)
 *  NOK - Error (probability db (stderr)) 
 */
int listUsersAndYoursFunctions(char *DBPath)
{
	sqlite3 *db = NULL;
	char *err_msg = NULL;
	int rc = 0;
	char sql[SQL_COMMAND_SZ + 1] = {'\0'};

	printf("Banco de dados: [%s]\n\n", DBPath);
	printf("Listagem de usuarios e suas funcoes:\n\n");

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
		return(NOK);
	}

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
        
		return(NOK);
	}

	memset(sql, '\0', sizeof(sql));
	snprintf(sql, SQL_COMMAND_SZ, "SELECT ID, FUNCAO FROM %s", DB_USERS_TABLE);

	rc = sqlite3_exec(db, sql, listUsersAndFuncs, 0, &err_msg);

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

		fprintf(stderr, "Failed to select data [%s]: [%s].\n", DB_REPORTS_TABLE, sql);
		fprintf(stderr, "CMD: [%s]\nSQL error: [%s]\n", sql, err_msg);

		sqlite3_free(err_msg);
		sqlite3_close(db);
        
		return(NOK);
	}

	printf("\n");

	sqlite3_close(db);

	return(OK);
}

/* int listFuncs(void *htmlsVoid, int argc, char **argv, char **azColName)
 *
 * SQLite3 callback
 *
 * INPUT:
 * OUTPUT:
 */
int listFuncs(void *htmlsVoid, int argc, char **argv, char **azColName)
{
	if(strcmp(argv[0], "All") != 0)
		printf("%-*s%s\n",VALOR_FUNCAO_LEN, argv[0], argv[1]);

	return(0);
}

/* int listUsersFunctions(char *DBPath)
 *
 * Print (screen) all levels (or office responsibility)
 *
 * INPUT:
 *  DBPath - Database full path
 * OUTPUT:
 *  OK - Printed (stdout)
 *  NOK - Error (probability db (stderr)) 
 */
int listUsersFunctions(char *DBPath)
{
	sqlite3 *db = NULL;
	char *err_msg = NULL;
	int rc = 0;
	char sql[SQL_COMMAND_SZ + 1] = {'\0'};

	printf("Banco de dados: [%s]\n\n", DBPath);
	printf("Listagem de funcoes disponiveis:\n\n");

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
		return(NOK);
	}

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
        
		return(NOK);
	}

	memset(sql, '\0', sizeof(sql));
	snprintf(sql, SQL_COMMAND_SZ, "SELECT FUNCAO, TITULO FROM %s", DB_REPORTS_TABLE);

	rc = sqlite3_exec(db, sql, listFuncs, 0, &err_msg);

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

		fprintf(stderr, "Failed to select data [%s]: [%s].\n", DB_REPORTS_TABLE, sql);
		fprintf(stderr, "CMD: [%s]\nSQL error: [%s]\n", sql, err_msg);

		sqlite3_free(err_msg);
		sqlite3_close(db);
        
		return(NOK);
	}

	printf("\n");

	sqlite3_close(db);

	return(OK);
}

/* int dbAddUser(char *user, char *func, char *pass, char *DBPath)
 *
 * Insert a user (ID), level (or office responsibility) and password into database.
 *
 * INPUT:
 *  user - User ID (DRT)
 *  func - level (or office responsibility)
 *  pass - Textplain password
 *  DBPath - Database full path
 * OUTPUT:
 *  OK - Added
 *  NOK - Error (probability db (stderr)) 
 */
int dbAddUser(char *user, char *func, char *pass, char *DBPath)
{
	sqlite3 *db = NULL;
	char *err_msg = NULL;
	int rc = 0;
	char sql[SQL_COMMAND_SZ + 1] = {'\0'};
	uint8_t hash[32] = {0};
	char passhash[PASS_SHA256_LEN + 1] = {'\0'};

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
		return(NOK);
	}

	rc = sqlite3_open_v2(DBPath, &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE, NULL);
    
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
        
		return(NOK);
	}

	calc_sha_256(hash, pass, strlen(pass));
	hash_to_string(passhash, hash);

	memset(sql, '\0', sizeof(sql));
	snprintf(sql, SQL_COMMAND_SZ, "INSERT INTO %s(ID, FUNCAO, PASSHASH) VALUES('%s', '%s', '%s')", DB_USERS_TABLE, user, func, passhash);

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

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

		fprintf(stderr, "Failed to select data [%s]: [%s].\n", DB_REPORTS_TABLE, sql);
		fprintf(stderr, "CMD: [%s]\nSQL error: [%s]\n", sql, err_msg);

		sqlite3_free(err_msg);
		sqlite3_close(db);
        
		return(NOK);
	}

	sqlite3_close(db);

	return(OK);
}

/* int dbRemoveUser(char *user, char *func, char *pass, char *DBPath)
 *
 * Remove a user (ID) from database.
 *
 * INPUT:
 *  user - User ID (DRT)
 *  DBPath - Database full path
 * OUTPUT:
 *  OK - Removed
 *  NOK - Error (probability db (stderr)) 
 */
int dbRemoveUser(char *user, char *DBPath)
{
	sqlite3 *db = NULL;
	char *err_msg = NULL;
	int rc = 0;
	char sql[SQL_COMMAND_SZ + 1] = {'\0'};

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
		return(NOK);
	}

	rc = sqlite3_open_v2(DBPath, &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE, NULL);
    
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
        
		return(NOK);
	}

	memset(sql, '\0', sizeof(sql));
	snprintf(sql, SQL_COMMAND_SZ, "DELETE FROM %s WHERE ID = '%s'", DB_USERS_TABLE, user);

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

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

		fprintf(stderr, "Failed to select data [%s]: [%s].\n", DB_REPORTS_TABLE, sql);
		fprintf(stderr, "CMD: [%s]\nSQL error: [%s]\n", sql, err_msg);

		sqlite3_free(err_msg);
		sqlite3_close(db);
        
		return(NOK);
	}

	sqlite3_close(db);

	return(OK);
}

int main(int argc, char *argv[])
{
	char DBPath[DB_PATHFILE_SZ + 1] = {'\0'};
	char user[DRT_LEN + 1] = {'\0'};
	char func[VALOR_FUNCAO_LEN + 1] = {'\0'};
	char pass[PASS_LEN + 1] = {'\0'};

	snprintf(DBPath, DB_PATHFILE_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), DATABASE_PATH, DATABASE_FILE);

	if(argc == 2){

		if(strncmp(argv[1], "-f", 2) == 0){
			/* form to list funcoes (user level or office responsibility) */

			listUsersFunctions(DBPath);

		}else if(strncmp(argv[1], "-i", 2) == 0){
			/* form to include user */

			printf("Usuario (DRT): ");
			fgets(user, DRT_LEN, stdin);
			changeCharByChar(user, '\n', '\0');
			/*
			c = strchr(user, '\n');
			if(c != NULL) *c = '\0';
			*/

			printf("Funcao ......: ");
			fgets(func, VALOR_FUNCAO_LEN, stdin);
			changeCharByChar(func, '\n', '\0');
			/*
			c = strchr(func, '\n');
			if(c != NULL) *c = '\0';
			*/

			printf("Senha .......: ");
			fgets(pass, PASS_LEN, stdin);
			changeCharByChar(pass, '\n', '\0');
			/*
			c = strchr(pass, '\n');
			if(c != NULL) *c = '\0';
			*/

			if(dbAddUser(user, func, pass, DBPath) == NOK){
				fprintf(stderr, "Erro inserindo formulario de usuario!\n");
				return(-3);
			}

			printf("Usuario inserido com sucesso!\nNao esquecer de adicionar a linha \"%s-%s\" no(s) arquivo(s) %s, no(s) computador(es) de trabalho deste usuario.\n", user, func, DRT_FILE);

		}else if(strncmp(argv[1], "-r", 2) == 0){
			/* form to remove user */

			printf("Usuario (DRT): ");
			fgets(user, DRT_LEN, stdin);
			changeCharByChar(user, '\n', '\0');
			/*
			c = strchr(user, '\n');
			if(c != NULL) *c = '\0';
			*/

			if(dbRemoveUser(user, DBPath) == NOK){
				fprintf(stderr, "Erro removendo formulario de usuario!\n");
				return(-3);
			}

			printf("Usuario removido com sucesso!\nNao esquecer de remover a linha do usuario \"%s\" no(s) arquivo(s) %s, no(s) computador(es) de trabalho deste usuario.\n", user, DRT_FILE);

		}else if(strncmp(argv[1], "-l", 2) == 0){
			/* list users */

			listUsersAndYoursFunctions(DBPath);
		}

		return(0);
	}

	if(argc == 5 && strncmp(argv[1], "-i", 2) == 0){
		/* include user */
		strncpy(user, argv[2], DRT_LEN);
		strncpy(func, argv[3], VALOR_FUNCAO_LEN);
		strncpy(pass, argv[4], PASS_LEN);

		if(dbAddUser(user, func, pass, DBPath) == NOK){
			fprintf(stderr, "Erro inserindo usuario!\n");
			return(-2);
		}

		return(0);
	}

	if(argc == 3 && strncmp(argv[1], "-r", 2) == 0){
		/* remove user */
		strncpy(user, argv[2], DRT_LEN);

		if(dbRemoveUser(user, DBPath) == NOK){
			fprintf(stderr, "Erro removendo usuario!\n");
			return(-2);
		}

		return(0);
	}

	/* argc and argv wrong! */
	fprintf(stderr, "Execucao:\n%s [-f] [<USER DRT> <FUNC (-f)> <PASSWORD>] [-r <USER DRT>]\n", argv[0]);
	fprintf(stderr, "\t-i <USER DRT> <FUNC (-f)> <PASSWORD>\t\tUsuario (DRT), funcao (conforme listada em -f) e senha\n");
	fprintf(stderr, "\t-i\t\tFormulario para inserir usuario (DRT), funcao (conforme listada em -f) e senha\n");
	fprintf(stderr, "\t-r <USER DRT>\t\tUsuario (DRT) a ser excluido\n");
	fprintf(stderr, "\t-r\t\tFormulario para excluir usuario (DRT)\n");
	fprintf(stderr, "\t-f\t\tListagem de funcoes\n");
	fprintf(stderr, "\t-l\t\tListagem de usuarios e suas funcoes\n");
	fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());

	return(-1);
}
