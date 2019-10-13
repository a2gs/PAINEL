/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
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
#include "BL_serv.h"
#include "util.h"

#include <log.h>
#include <cfgFile.h>


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
 *  PAINEL_OK - Ok
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

	return(PAINEL_OK);
}

/* int html_startTable(htmlFiles_t *htmls, char *columnsHeaders)
 *
 * Writes the HTML body section (table's header).
 *
 * INPUT:
 *  htmls - HTMLs files (static and refresh)
 *  columnsHeaders - the column name
 * OUTPUT:
 *  PAINEL_OK - Ok
 */
int html_startTable(htmlFiles_t *htmls, char *columnsHeaders)
{
	char *p = NULL;
	char descricao[SZ_DESCRICAO + 1] = {'\0'};

	html_writeDual(htmls, 0, "<body>\n");
	html_writeDual(htmls, 0, "\t<table>\n");
	html_writeDual(htmls, 0, "\t\t<tr>\n");

	for(p = columnsHeaders; *p != '\0';){
		cutter(&p, ',', descricao, SZ_DESCRICAO);

		html_writeDual(htmls, 0, "\t\t\t<th>");
		html_writeDual(htmls, 0, descricao);
		html_writeDual(htmls, 0, "</th>\n");
	}

	html_writeDual(htmls, 0, "\t\t</tr>\n");

	html_fflush(htmls);

	return(PAINEL_OK);
}

/* int html_endTable(htmlFiles_t *htmls)
 *
 * Closes the HTML table and body sections.
 *
 * INPUT:
 *  htmls - HTMLs files (static and refresh)
 * OUTPUT:
 *  PAINEL_OK - Ok
 */
int html_endTable(htmlFiles_t *htmls)
{
	html_writeDual(htmls, 0, "\t</table>\n");
	html_writeDual(htmls, 0, "</body>\n");
	html_writeDual(htmls, 0, "</html>\n");

	html_fflush(htmls);

	return(PAINEL_OK);
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
 *  PAINEL_OK - Ok
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

int hmtl_constructEmptyTable(htmlFiles_t *htmls)
{
	char htmlText[MAXLINE] = {'\0'};
	snprintf(htmlText, MAXLINE, "<center style=\"color:rgb(255, 0, 0); border:2px solid rgb(255, 0, 0); background-color:rgb(0, 0, 0);\"><b>THERE ARE NO DATA SAVED FOR THIS FUNCTION!</b></center><br>\n");
	html_writeDual(htmls, 0, htmlText);

	return(PAINEL_OK);
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
	char *cfgLogFile     = NULL;
	char *cfgLogLevel    = NULL;
	char *cfgLevel       = NULL;
	char *cfgRegHTMLFile = NULL;
	char *cfgHTMLReload  = NULL;
	char funcao[VALOR_FUNCAO_LEN      + 1] = {'\0'};
	char sql[SQL_COMMAND_SZ           + 1] = {'\0'};
	char fHtmlStatic[SZ_HTMLFILENAME  + 1] = {'\0'};
	char fHtmlRefresh[SZ_HTMLFILENAME + 1] = {'\0'};
	char DBPath[DB_PATHFILE_SZ        + 1] = {'\0'};
	unsigned int segReaload   = 0;
	unsigned int segRefresh   = 0;
	unsigned int cfgLineError = 0;
	pid_t p = (pid_t)0;
	log_t log;
	cfgFile_t selHtmlCfg;
	htmlFiles_t htmls = {
		.htmlStatic = NULL,
		.htmlRefresh = NULL,
		.fdStatic = 0,
		.fdRefresh = 0
	};
	pageInfos_t pageInfo = {
		.columnsHeaders = {0},
		.columnsTable = {0},
		.title = {0}
	};

	if(argc != 2){
		fprintf(stderr, "[%s %d] Usage:\n%s <CONFIG_FILE>\n\n", time_DDMMYYhhmmss(), getpid(), argv[0]);
		fprintf(stderr, "CONFIG_FILE sample variables:\n");
		fprintf(stderr, "\tLEVEL = <FUNCAO>\n");
		fprintf(stderr, "\tREGENERATING_HTML_FILE_SECS = 5\n");
		fprintf(stderr, "\tHTML_RELOAD = 10\n");
		fprintf(stderr, "\tLOG_FILE = ncclient.log\n");
		fprintf(stderr, "\t#Log levels:\n");
		fprintf(stderr, "\t#REDALERT = Red alert\n");
		fprintf(stderr, "\t#DBALERT = Database alert\n");
		fprintf(stderr, "\t#DBMSG = Database message\n");
		fprintf(stderr, "\t#OPALERT = Operation alert\n");
		fprintf(stderr, "\t#OPMSG = Operation message\n");
		fprintf(stderr, "\t#MSG = Just a message\n");
		fprintf(stderr, "\t#DEV = Developer (DEBUG) message\n");
		fprintf(stderr, "\tLOG_LEVEL = REDALERT|DBALERT|DBMSG|OPALERT|OPMSG|MSG|DEV\n");
		fprintf(stderr, "PAINEL Home: [%s]\n", getPAINELEnvHomeVar());
		return(-1);
	}

	if(cfgFileLoad(&selHtmlCfg, argv[1], &cfgLineError) == CFGFILE_NOK){
		fprintf(stderr, "Error open/loading (at line: [%d]) configuration file [%s]: [%s].\n", cfgLineError, argv[1], strerror(errno));
		return(-2);
	}

	if(cfgFileOpt(&selHtmlCfg, "LEVEL", &cfgLevel) == CFGFILE_NOK){
		fprintf(stderr, "Config with label LEVEL not found into file [%s]! Exit.\n", argv[1]);
		return(-3);
	}

	if(cfgFileOpt(&selHtmlCfg, "REGENERATING_HTML_FILE_SECS", &cfgRegHTMLFile) == CFGFILE_NOK){
		fprintf(stderr, "Config with label REGENERATING_HTML_FILE_SECS not found into file [%s]! Exit.\n", argv[1]);
		return(-4);
	}

	if(cfgFileOpt(&selHtmlCfg, "HTML_RELOAD", &cfgHTMLReload) == CFGFILE_NOK){
		fprintf(stderr, "Config with label HTML_RELOAD not found into file [%s]! Exit.\n", argv[1]);
		return(-5);
	}

	if(cfgFileOpt(&selHtmlCfg, "LOG_FILE", &cfgLogFile) == CFGFILE_NOK){
		fprintf(stderr, "Config with label LOG_FILE not found into file [%s]! Exit.\n", argv[1]);
		return(-6);
	}

	if(cfgFileOpt(&selHtmlCfg, "LOG_LEVEL", &cfgLogLevel) == CFGFILE_NOK){
		fprintf(stderr, "Config with label LOG_LEVEL not found into file [%s]! Exit.\n", argv[1]);
		return(-7);
	}

	if(logCreate(&log, cfgLogFile, cfgLogLevel) == LOG_NOK){
		fprintf(stderr, "[%s %d] Erro criando log! [%s]\n", time_DDMMYYhhmmss(), getpid(), (errno == 0 ? "Level parameters error" : strerror(errno)));
		return(-8);
	}

	segReaload = atoi(cfgHTMLReload);
	segRefresh = atoi(cfgRegHTMLFile);
	strncpy(funcao, cfgLevel, VALOR_FUNCAO_LEN);

	if(cfgFileFree(&selHtmlCfg) == CFGFILE_NOK){
		printf("Error at cfgFileFree().\n");
		return(-9);
	}

	getLogSystem_Util(&log); /* Loading log to util functions */
	getLogSystem_DB(&log); /* Loading log to DB functions */

	p = daemonizeWithoutLock(/*&log*/);
	if(p == (pid_t)PAINEL_NOK){
		logWrite(&log, LOGOPALERT, "Cannt daemonize select html!\n");
		logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

		logClose(&log);
		return(-10);
	}

	snprintf(fHtmlStatic, SZ_HTMLFILENAME, "%s/%s/%s_Static.html", getPAINELEnvHomeVar(), HTML_PATH, argv[1]);
	snprintf(fHtmlRefresh, SZ_HTMLFILENAME, "%s/%s/%s_Refresh.html", getPAINELEnvHomeVar(), HTML_PATH, argv[1]);
	snprintf(DBPath, DB_PATHFILE_SZ, "%s/%s/%s", getPAINELEnvHomeVar(), DATABASE_PATH, DATABASE_FILE);

	logWrite(&log, LOGMUSTLOGIT, "Select HTML Up! Level: [%s] Generation seconds: [%d] HTML refresh seconds: [%d] PID: [%d] Date: [%s] PAINEL Home: [%s] Files: [%s | %s].\n", funcao, segReaload, segRefresh, p, time_DDMMYYhhmmss(), getPAINELEnvHomeVar(), fHtmlStatic, fHtmlRefresh);

	for(;;){
		memset(&pageInfo, 0, sizeof(pageInfos_t));

		if(dbOpen(NULL, SQLITE_OPEN_READONLY|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE/*, &log*/) == PAINEL_NOK){
			logWrite(&log, LOGOPALERT, "Erro em abrir banco de dados!\n");
			logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

			dbClose();
			logClose(&log);

			return(-11);
		}

		/* Defining report layout by user level (fucao) */

		memset(sql, '\0', sizeof(sql));
		snprintf(sql, SQL_COMMAND_SZ, "SELECT TITULO, CAMPOS, HEADERS FROM %s WHERE FUNCAO = '%s'", DB_REPORTS_TABLE, funcao);

		select_NOROW = SQL_NO_ROW;

		if(dbSelect(sql, hmtl_relat_infos, &pageInfo) == PAINEL_NOK){
			logWrite(&log, LOGOPALERT, "Error at select database to define reports [%s]!\n", sql);
			logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

			logClose(&log);
			dbClose();
        
			return(-12);
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
		if(html_fopen(&htmls, fHtmlStatic, fHtmlRefresh) == PAINEL_NOK){
			logWrite(&log, LOGOPALERT, "Falha em abrir/criar arquivos htmls [%s] e [%s].\n", fHtmlStatic, fHtmlRefresh);
			logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

			logClose(&log);
			return(-13);
		}

		if(html_header(&htmls, pageInfo.title, segRefresh) == PAINEL_NOK){
			logWrite(&log, LOGOPALERT, "Falha em escrever header para arquivos htmls [%s] e [%s].\n", fHtmlStatic, fHtmlRefresh);
			logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

			logClose(&log);
			return(-14);
		}

		if(html_startTable(&htmls, pageInfo.columnsHeaders) == PAINEL_NOK){
			logWrite(&log, LOGOPALERT, "Falha em escrever tabela para arquivos htmls [%s] e [%s].\n", fHtmlStatic, fHtmlRefresh);
			logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

			logClose(&log);
			return(-15);
		}

		/* Selecting data by user level (funcao). ALL = all database */

		memset(sql, '\0', sizeof(sql));

		if(strncmp(funcao, "All", 3) == 0) snprintf(sql, SQL_COMMAND_SZ, "SELECT %s FROM %s", pageInfo.columnsTable, DB_MSGS_TABLE);
		else                               snprintf(sql, SQL_COMMAND_SZ, "SELECT %s FROM %s WHERE FUNCAO = '%s'", pageInfo.columnsTable, DB_MSGS_TABLE, funcao);

		select_NOROW = SQL_NO_ROW;

		if(dbSelect(sql, hmtl_constructTable, &htmls) == PAINEL_NOK){
			logWrite(&log, LOGOPALERT, "Error at select database to construct tables [%s]!\n", sql);
			logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

			logClose(&log);
			dbClose();
        
			return(-16);
		}

		if(select_NOROW == SQL_NO_ROW){
			logWrite(&log, LOGDEV, "NO ROW for select: [%s].\n", sql);
			logWrite(&log, LOGOPMSG, "Zero register for [%s]! Empty report.\n", funcao);
			hmtl_constructEmptyTable(&htmls);
		}
    
		if(html_endTable(&htmls) == PAINEL_NOK){
			logWrite(&log, LOGOPALERT, "Falha em escrever finalizar arquivos htmls [%s] e [%s].\n", fHtmlStatic, fHtmlRefresh);
			logWrite(&log, LOGREDALERT, "Terminating application!\n\n");

			logClose(&log);
			dbClose();

			return(-17);
		}

		html_fflush(&htmls);
		html_fclose(&htmls);

		dbClose();

		sleep(segReaload);
	}

	logWrite(&log, LOGREDALERT, "Terminating application with sucessfully!\n\n");
    
	dbClose();
	logClose(&log);
	return(0);
}
