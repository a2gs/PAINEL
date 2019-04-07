/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* select_html.c
 * Creates a HTML report to a given User Level (office responsibility)
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
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
#include "html.h"
#include "SG_serv.h"
#include "util.h"

#include "log.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define SZ_HTMLFILENAME			(200)
#define SZ_COLUMNSHEADER_DB	(10000)
#define SZ_COLUMNSTABLE_DB		(1000)
#define SZ_PAGETITLE_DB			(500)
#define SZ_DESCRICAO				(80)

typedef struct _pageInfos_t{
	char columnsHeaders[SZ_COLUMNSHEADER_DB + 1];
	char columnsTable[SZ_COLUMNSTABLE_DB + 1];
	char title[SZ_PAGETITLE_DB + 1];
}pageInfos_t;

static char select_NOROW = SQL_NO_ROW;


/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
/* int html_header(htmlFiles_t *htmls, char *title, unsigned int refreshSeg)
 *
 * Writes the HTML Header section.
 *
 * INPUT:
 *  htmls - HTMLs files (static and refresh)
 *  title - HTML title tag
 *  refreshSeg - Seconds for refresh rate (HTML meta tag)
 * OUTPUT:
 *  OK - Ok
 */
int html_header(htmlFiles_t *htmls, char *title, unsigned int refreshSeg)
{
	char auxBufRefresh[25] = {'\0'};

	snprintf(auxBufRefresh, 25, "%d", refreshSeg);

	html_writeDual(htmls, 0, "<!DOCTYPE html>\n");
	html_writeDual(htmls, 0, "<html>\n");

	html_writeDual(htmls, 2, "<meta http-equiv=\"refresh\" content=\"");
	html_writeDual(htmls, 2, auxBufRefresh);
	html_writeDual(htmls, 2, "\">\n");

	html_writeDual(htmls, 0, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/>\n");
	html_writeDual(htmls, 0, "<header>\n");
	html_writeDual(htmls, 0, "<title>");
	html_writeDual(htmls, 0, title);
	html_writeDual(htmls, 0, "</title>\n");
	html_writeDual(htmls, 0, "\t<style>\n");
	html_writeDual(htmls, 0, "\t\ttable{\n");
	html_writeDual(htmls, 0, "\t\t\tfont-family: arial, sans-serif;\n");
	html_writeDual(htmls, 0, "\t\t\tborder-collapse: collapse;\n");
	html_writeDual(htmls, 0, "\t\t\twidth: 100%;\n");
	html_writeDual(htmls, 0, "\t\t}\n");
	html_writeDual(htmls, 0, "\t\tth, td{\n");
	html_writeDual(htmls, 0, "\t\t\tborder: 1px solid #dddddd;\n");
	html_writeDual(htmls, 0, "\t\t\tth.text-align: center;\n");
	html_writeDual(htmls, 0, "\t\t\ttd.text-align: left;\n");
	html_writeDual(htmls, 0, "\t\t\tpadding: 8px;\n");
	html_writeDual(htmls, 0, "\t\t}\n");
	html_writeDual(htmls, 0, "\t\ttr:nth-child(even){\n");
	html_writeDual(htmls, 0, "\t\t\tbackground-color: #dddddd;\n");
	html_writeDual(htmls, 0, "\t\t}\n");
	html_writeDual(htmls, 0, "\t</style>\n");
	/*html_writeDual(htmls, 1, "\t<img src=\"brazil.png\" alt=\"Brasil\" height=\"42\" width=\"48\" align=\"left\">\n");*/
	/*html_writeDual(htmls, 1, "\t<img src=\"france.png\" alt=\"France\" height=\"42\" width=\"48\" align=\"right\">\n");*/

	html_writeDual(htmls, 0, "\t<h1><center><u>");
	html_writeDual(htmls, 0, title);
	html_writeDual(htmls, 0, "</u></center></h1>\n\n");

	html_writeDual(htmls, 0, "</header>\n\n");

	html_fflush(htmls);

	return(OK);
}

/* int html_startTable(htmlFiles_t *htmls, char *columnsHeaders)
 *
 * Writes the HTML body section (table's header).
 *
 * INPUT:
 *  htmls - HTMLs files (static and refresh)
 *  columnsHeaders - the column name
 * OUTPUT:
 *  OK - Ok
 */
int html_startTable(htmlFiles_t *htmls, char *columnsHeaders)
{
	char *p = NULL;
	char descricao[SZ_DESCRICAO + 1] = {'\0'};

	html_writeDual(htmls, 0, "<body>\n");
	html_writeDual(htmls, 0, "\t<table>\n");
	html_writeDual(htmls, 0, "\t\t<tr>\n");

	for(p = columnsHeaders; *p == '\0';){
		cutter(&p, ',', descricao, SZ_DESCRICAO);

		html_writeDual(htmls, 0, "\t\t\t<th>");
		html_writeDual(htmls, 0, descricao);
		html_writeDual(htmls, 0, "</th>\n");
	}

	html_writeDual(htmls, 0, "\t\t</tr>\n");

	html_fflush(htmls);

	return(OK);

	/*
	char *c1 = NULL;
	char *c2 = NULL;
	char descricao[SZ_DESCRICAO + 1] = {'\0'};

	html_writeDual(htmls, 0, "<body>\n");
	html_writeDual(htmls, 0, "\t<table>\n");
	html_writeDual(htmls, 0, "\t\t<tr>\n");

	for(c1 = c2 = columnsHeaders;;){

		c2 = strchr(c1, ',');

		if(c2 != NULL){
			unsigned int szC = ((c2-c1) < SZ_DESCRICAO ? c2-c1 : SZ_DESCRICAO);

			strncpy(descricao, c1, szC);
			descricao[szC] = '\0';
			c1 = c2+1;
		}else
			strcpy(descricao, c1);

		html_writeDual(htmls, 0, "\t\t\t<th>");
		html_writeDual(htmls, 0, descricao);
		html_writeDual(htmls, 0, "</th>\n");

		if(c2 == NULL)
			break;
	}

	html_writeDual(htmls, 0, "\t\t</tr>\n");

	html_fflush(htmls);

	return(OK);
	*/
}

/* int html_endTable(htmlFiles_t *htmls)
 *
 * Closes the HTML table and body sections.
 *
 * INPUT:
 *  htmls - HTMLs files (static and refresh)
 * OUTPUT:
 *  OK - Ok
 */
int html_endTable(htmlFiles_t *htmls)
{
	html_writeDual(htmls, 0, "\t</table>\n");
	html_writeDual(htmls, 0, "</body>\n");
	html_writeDual(htmls, 0, "</html>\n");

	html_fflush(htmls);

	return(OK);
}

/* int hmtl_relat_infos(void *htmlsVoid, int argc, char **argv, char **azColName)
 *
 * Copies database returned data: SQLite callback.
 *
 * INPUT:
 *  argc - number of columns
 *  argv - cell content
 *  azColName - columns name
 * OUTPUT:
 *  htmlsVoid - Database breakdown data
 *  OK - Ok
 */
int hmtl_relat_infos(void *htmlsVoid, int argc, char **argv, char **azColName)
{
	pageInfos_t *data = NULL;

	data = (pageInfos_t *)htmlsVoid;

	select_NOROW = SQL_HAS_ROW;

	strncpy(data->title,          (argv[0] ? argv[0] : "\0"), SZ_PAGETITLE_DB    );
	strncpy(data->columnsTable,   (argv[1] ? argv[1] : "\0"), SZ_COLUMNSTABLE_DB );
	strncpy(data->columnsHeaders, (argv[2] ? argv[2] : "\0"), SZ_COLUMNSHEADER_DB);

	return(0);
}

/* int hmtl_constructTable(void *htmlsVoid, int argc, char **argv, char **azColName)
 *
 * Writes the HTML rows (HTML tr and td tags): SQLite callback.
 *
 * INPUT:
 *  htmlsVoid - HTMLs files to be written
 *  argc - number of columns
 *  argv - cell content
 *  azColName - columns name
 *
 * OUTPUT:
 *  0
 */
int hmtl_constructTable(void *htmlsVoid, int argc, char **argv, char **azColName)
{
	char htmlText[MAXLINE] = {'\0'};
	int i = 0;
	static int writeFlush = 0;
	htmlFiles_t *htmls = NULL;

	htmls = (htmlFiles_t *)htmlsVoid;

	select_NOROW = SQL_HAS_ROW;

	html_writeDual(htmls, 0, "\t\t<tr>\n");
    
	for(i = 0; i < argc; i++){
		snprintf(htmlText, MAXLINE, "\t\t\t<td>%s</td>\n", argv[i] ? argv[i] : "\0");
		html_writeDual(htmls, 0, htmlText);
	}

	html_writeDual(htmls, 0, "\t\t</tr>\n");
    
	if(writeFlush % 10 == 0){
		html_fflush(htmls);
		writeFlush = 0;
	}
    
	return(0);
}

/* int main(int argc, char *argv[])
 *
 * select_html starts.
 *
 * INPUT:
 *  argv[1] - User Level (or office responsibility) HTML report
 *  argv[2] - Seconds to rewrite the HTML file
 *  argv[3] - Seconds to refresh HTML (meta tag)
 * OUTPUT (to OS):
 *  0 - Ok
 *  !0 - Error (see log)
 */
int main(int argc, char *argv[])
{
	char *funcao = NULL;
	char sql[SQL_COMMAND_SZ + 1] = {'\0'};
	char fHtmlStatic[SZ_HTMLFILENAME + 1] = {'\0'};
	char fHtmlRefresh[SZ_HTMLFILENAME + 1] = {'\0'};
	char DBPath[DB_PATHFILE_SZ + 1] = {'\0'};
	pid_t p = (pid_t)0;
	log_t log;
	htmlFiles_t htmls = {
		.htmlStatic = NULL,
		.htmlRefresh = NULL,
		.fdStatic = 0,
		.fdRefresh = 0
	};
	unsigned int segReaload = 0;
	unsigned int segRefresh = 0;
	pageInfos_t pageInfo = {
		.columnsHeaders = {0},
		.columnsTable = {0},
		.title = {0}
	};

	if(argc != 6){
		fprintf(stderr, "[%s %d] Usage:\n%s <FUNCAO> <SEGUNDOS_RELOAD_GER_HTML> <SEGUNDOS_REFRESH_HTML> <LOG_FULL_PATH> <LOG_LEVEL 'WWW|XXX|YYY|ZZZ'>\n\n", time_DDMMYYhhmmss(), getpid(), argv[0]);
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

	if(logCreate(&log, argv[4], argv[5]) == LOG_NOK){
		fprintf(stderr, "[%s %d] Erro criando log! [%s]\n", time_DDMMYYhhmmss(), getpid(), (errno == 0 ? "Level parameters error" : strerror(errno)));

		return(-2);
	}

	p = daemonizeWithoutLock(&log);
	if(p == (pid_t)NOK){
		logWrite(&log, LOGOPALERT, "Cannt daemonize select html!\n");
		logWrite(&log, LOGREDALERT, "Terminating application!\n");

		logClose(&log);
		return(-3);
	}

	funcao = argv[1];
	segReaload = atoi(argv[2]);
	segRefresh = atoi(argv[3]);

	snprintf(fHtmlStatic, SZ_HTMLFILENAME, "%s/%s/%s_Static.html", getPAINELEnvHomeVar(), HTML_PATH, argv[1]);
	snprintf(fHtmlRefresh, SZ_HTMLFILENAME, "%s/%s/%s_Refresh.html", getPAINELEnvHomeVar(), HTML_PATH, argv[1]);
	snprintf(DBPath, DB_PATHFILE_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), DATABASE_PATH, DATABASE_FILE);

	logWrite(&log, LOGMUSTLOGIT, "Select HTML Up! Level: [%s] Generation seconds: [%d] HTML refresh seconds: [%d] PID: [%d] Date: [%s] PAINEL Home: [%s] Files: [%s | %s].\n", funcao, segReaload, segRefresh, p, time_DDMMYYhhmmss(), getPAINELEnvHomeVar(), fHtmlStatic, fHtmlRefresh);

	for(;;){
		memset(&pageInfo, 0, sizeof(pageInfos_t));

		if(dbOpen(NULL, SQLITE_OPEN_READONLY|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE, &log) == NOK){
			logWrite(&log, LOGOPALERT, "Erro em abrir banco de dados!\n");
			logWrite(&log, LOGREDALERT, "Terminating application!\n");

			dbClose();
			logClose(&log);

			return(-5);
		}

		/* Defining report layout by user level (fucao) */

		memset(sql, '\0', sizeof(sql));
		snprintf(sql, SQL_COMMAND_SZ, "SELECT TITULO, CAMPOS, HEADERS FROM %s WHERE FUNCAO = '%s'", DB_REPORTS_TABLE, funcao);

		select_NOROW = SQL_NO_ROW;

		if(dbSelect(sql, hmtl_relat_infos, &pageInfo) == NOK){
			logWrite(&log, LOGOPALERT, "Error at select database to define reports [%s]!\n", sql);
			logWrite(&log, LOGREDALERT, "Terminating application!\n");

			logClose(&log);
			dbClose();
        
			return(-6);
		}
		logWrite(&log, LOGDEV, "Data returned: [%s][%s][%s]\n", pageInfo.title, pageInfo.columnsHeaders, pageInfo.columnsTable);

		if(select_NOROW == SQL_NO_ROW){
			dbClose();

			logWrite(&log, LOGOPALERT, "There is no register to level [%s] waiting [%d] seconds to retry.\n", funcao, segReaload * 3);

			sleep(segReaload * 3);

			continue;
		}

		/* Creating HTML files */

		memset(&htmls, 0, sizeof(htmlFiles_t));
		if(html_fopen(&htmls, fHtmlStatic, fHtmlRefresh) == NOK){
			logWrite(&log, LOGOPALERT, "Falha em abrir/criar arquivos htmls [%s] e [%s].\n", fHtmlStatic, fHtmlRefresh);
			logWrite(&log, LOGREDALERT, "Terminating application!\n");

			logClose(&log);
			return(-7);
		}

		if(html_header(&htmls, pageInfo.title, segRefresh) == NOK){
			logWrite(&log, LOGOPALERT, "Falha em escrever header para arquivos htmls [%s] e [%s].\n", fHtmlStatic, fHtmlRefresh);
			logWrite(&log, LOGREDALERT, "Terminating application!\n");

			logClose(&log);
			return(-8);
		}

		if(html_startTable(&htmls, pageInfo.columnsHeaders) == NOK){
			logWrite(&log, LOGOPALERT, "Falha em escrever tabela para arquivos htmls [%s] e [%s].\n", fHtmlStatic, fHtmlRefresh);
			logWrite(&log, LOGREDALERT, "Terminating application!\n");

			logClose(&log);
			return(-9);
		}

		/* Selecting data by user level (funcao). ALL = all database */

		memset(sql, '\0', sizeof(sql));

		if(strncmp(funcao, "All", 3) == 0) snprintf(sql, SQL_COMMAND_SZ, "SELECT %s FROM %s", pageInfo.columnsTable, DB_MSGS_TABLE);
		else                               snprintf(sql, SQL_COMMAND_SZ, "SELECT %s FROM %s WHERE FUNCAO = '%s'", pageInfo.columnsTable, DB_MSGS_TABLE, funcao);

		select_NOROW = SQL_NO_ROW;

		if(dbSelect(sql, hmtl_constructTable, &htmls) == NOK){
			logWrite(&log, LOGOPALERT, "Error at select database to construct tables [%s]!\n", sql);
			logWrite(&log, LOGREDALERT, "Terminating application!\n");

			logClose(&log);
			dbClose();
        
			return(-10);
		}

		if(select_NOROW == SQL_NO_ROW){
			logWrite(&log, LOGDEV, "NO ROW for select: [%s].\n", sql);
			logWrite(&log, LOGOPMSG, "Zero register for [%s]! Empty report.\n", funcao);
		}
    
		if(html_endTable(&htmls) == NOK){
			logWrite(&log, LOGOPALERT, "Falha em escrever finalizar arquivos htmls [%s] e [%s].\n", fHtmlStatic, fHtmlRefresh);
			logWrite(&log, LOGREDALERT, "Terminating application!\n");

			logClose(&log);
			dbClose();

			return(-11);
		}

		html_fflush(&htmls);
		html_fclose(&htmls);

		dbClose();

		sleep(segReaload);
	}

	logWrite(&log, LOGREDALERT, "Terminating application with sucessfully!\n");
    
	dbClose();
	logClose(&log);
	return(0);
}
