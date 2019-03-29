#/* Andre Augusto Giannotti Scota (a2gs)
# * andre.scota@gmail.com
# *
# * PAINEL
# *
# * Public Domain
# *
# */


#/* makefile
# *
# *  Who     | When       | What
# *  --------+------------+----------------------------
# *   a2gs   | 13/08/2018 | Creation
# *          |            |
# */


CFLAGS_OPTIMIZATION = -g
#CFLAGS_OPTIMIZATION = -O3
CFLAGS_VERSION = -std=c11
CFLAGS_WARNINGS = -Wall -Wextra -Wno-unused-parameter -Wno-unused-but-set-parameter -Wno-unused-result
CFLAGS_DEFINES = -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L -D_POSIX_SOURCE=1 -D_DEFAULT_SOURCE=1 -D_GNU_SOURCE=1
CFLAGS = $(CFLAGS_OPTIMIZATION) $(CFLAGS_VERSION) $(CFLAGS_WARNINGS) $(CFLAGS_DEFINES)

# Specific libraries:
LIB_SHA256=sha-256
LIB_LOG=log
SHA256PATH=$(LOCAL_LIBS)/$(LIB_SHA256)
LOGPATH=$(LOCAL_LIBS)/$(LIB_LOG)
# Libs to ALL modules:
LIBS = -lsqlite3 -llog # Common libs, libs to all modules or system libs

INCLUDEPATH = -I./include -I$(LIBS_BIN_PATH)
SOURCEPATH = ./src
BINPATH = ./bin
LOCAL_LIBS = ./libs
LIBS_BIN_PATH=$(LOCAL_LIBS)/bin

CC = gcc
RM = rm -f
CP = cp
AR = ar
RANLIB = ranlib
CPPCHECK = cppcheck

CPPCHECK_OPTS = --enable=all --std=c11 --platform=unix64 --language=c --check-config --suppress=missingIncludeSystem

all: clean logtag sha256 client serv select_html select_Excel servList create_db userId cppcheck
	@echo "=== ctags ==================="
	ctags -R *
	@echo "=== cscope =================="
	cscope -b -R

cppcheck:
	@echo "=== cppcheck ================"
	$(CPPCHECK) $(CPPCHECK_OPTS) -I ./include -I ./libs/sha-256/ -I./libs/log/ -i ./database/ -i ./database_dataBackup/ -i ./html/ -i ./log/ -i ./ncurses/ --suppress=missingIncludeSystem ./src/

logtag:
	@echo "=== lib LOG ================="
	$(CC) -c -o$(LIBS_BIN_PATH)/log.o $(LOGPATH)/log.c -I$(LOGPATH) $(CFLAGS)
	$(AR) rc $(LIBS_BIN_PATH)/liblog.a $(LIBS_BIN_PATH)/log.o
	$(RANLIB) $(LIBS_BIN_PATH)/liblog.a
	$(CP) $(LOGPATH)/log.h $(LIBS_BIN_PATH)
	-$(RM) $(LIBS_BIN_PATH)/log.o

sha256:
	@echo "=== lib SHA256 =============="
	$(CC) -c -o$(LIBS_BIN_PATH)/sha-256.o $(SHA256PATH)/sha-256.c -I$(SHA256PATH) $(CFLAGS)
	$(AR) rc $(LIBS_BIN_PATH)/libsha-256.a $(LIBS_BIN_PATH)/sha-256.o
	$(RANLIB) $(LIBS_BIN_PATH)/libsha-256.a
	$(CP) $(SHA256PATH)/sha-256.h $(LIBS_BIN_PATH)
	-$(RM) $(LIBS_BIN_PATH)/sha-256.o

client: sha256 logtag
	@echo "=== client =================="
	$(CC) -o $(BINPATH)/client $(SOURCEPATH)/client.c $(SOURCEPATH)/util.c $(SOURCEPATH)/SG_client.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -l$(LIB_SHA256) $(CFLAGS)

serv: logtag
	@echo "=== serv ===================="
	$(CC) -o $(BINPATH)/serv $(SOURCEPATH)/serv.c $(SOURCEPATH)/util.c $(SOURCEPATH)/SG_serv.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) -l$(LIB_LOG) $(LIBS) $(CFLAGS)

select_html: logtag
	@echo "=== select_html ============="
	$(CC) -o $(BINPATH)/select_html $(SOURCEPATH)/select_html.c $(SOURCEPATH)/util.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -l$(LIB_LOG) $(CFLAGS)

select_Excel:
	@echo "=== select_Excel ============"
	$(CC) -o $(BINPATH)/select_Excel $(SOURCEPATH)/select_Excel.c $(SOURCEPATH)/util.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) $(CFLAGS)

userId: sha256
	@echo "=== userId ================="
	$(CC) -o $(BINPATH)/userId $(SOURCEPATH)/userId.c $(SOURCEPATH)/util.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -l$(LIB_SHA256) $(CFLAGS) -Wno-unused-variable

servList: logtag
	@echo "=== servList ================"
	$(CC) -o $(BINPATH)/servList $(SOURCEPATH)/servList.c $(SOURCEPATH)/util.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) -l$(LIB_LOG) $(CFLAGS)

create_db:
	@echo "=== create_db ==============="
	$(CC) -o $(BINPATH)/create_db $(SOURCEPATH)/create_db.c $(SOURCEPATH)/util.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) $(CFLAGS)

clean: clean_html clean_log clean_bin #clean_data
	@echo "=== clean ==================="
	-$(RM) nohup.out tags cscope.out

clean_bin:
	@echo "=== clean_bin ==============="
	-$(RM) $(BINPATH)/* $(LIBS_BIN_PATH)/*

clean_log:
	@echo "=== clean_log ==============="
	-$(RM) ./log/*.log

clean_html:
	@echo "=== clean_html =============="
	-$(RM) html/*.html

clean_data:
	@echo "=== clean_data =============="
	-$(RM) ./database/*.db ./database/*.xls ./database/*.xlsx
