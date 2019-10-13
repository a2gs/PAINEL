#/* Andre Augusto Giannotti Scota (a2gs)
# * andre.scota@gmail.com
# *
# * PAINEL
# *
# * Apache License 2.0
# *
# */


#/* makefile
# *
# *  Who     | When       | What
# *  --------+------------+----------------------------
# *   a2gs   | 13/08/2018 | Creation
# *          |            |
# */


# C flags:
CFLAGS_OPTIMIZATION = -g
#CFLAGS_OPTIMIZATION = -O3
CFLAGS_VERSION = -std=c11
CFLAGS_WARNINGS = -Wall -Wextra -Wno-unused-parameter -Wno-unused-but-set-parameter
CFLAGS_DEFINES = -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L -D_POSIX_SOURCE=1 -D_DEFAULT_SOURCE=1 -D_GNU_SOURCE=1
CFLAGS = $(CFLAGS_OPTIMIZATION) $(CFLAGS_VERSION) $(CFLAGS_WARNINGS) $(CFLAGS_DEFINES)

# Specific libraries (project libraries ./libs/):
LIB_SHA256 = sha-256
LIB_LOG = log
LIB_LLIST = linkedlist
LIB_WIZPATPATH = wizard_by_return
LIB_CFGFILE = cfgFile

SHA256PATH = $(LOCAL_LIBS)/$(LIB_SHA256)
LOGPATH = $(LOCAL_LIBS)/$(LIB_LOG)
LLISTPATH = $(LOCAL_LIBS)/$(LIB_LLIST)
WIZPATPATH = $(LOCAL_LIBS)/$(LIB_WIZPATPATH)
CFGFILEPATH = $(LOCAL_LIBS)/$(LIB_CFGFILE)

# Libs to ALL modules:
LIBS = -lsqlite3 -lssl -lcrypto -l$(LIB_LOG) # Common libs, libs to all modules or system libs

# Paths normalizes
SOURCEPATH = ./src
BINPATH = ./bin
LOCAL_LIBS = ./libs
LIBS_BIN_PATH = $(LOCAL_LIBS)/bin
INCLUDEPATH = -I./include -I$(LIBS_BIN_PATH)

# System shell utilities
CC = gcc
RM = rm -f
CP = cp
AR = ar
RANLIB = ranlib
CPPCHECK = cppcheck

CPPCHECK_OPTS = --enable=all --std=c11 --platform=unix64 --language=c --check-config --suppress=missingIncludeSystem

all: clean logtag sha256 llist cfgfile wizard_by_return client ncclient sendRecvCmd serv select_html select_Excel servList create_db userIdDB pingServ cppcheck
	@echo
	@echo "=== ctags ==================="
	ctags -R *
	@echo
	@echo "=== cscope =================="
	cscope -b -R

cppcheck:
	@echo
	@echo "=== cppcheck ================"
	$(CPPCHECK) $(CPPCHECK_OPTS) -I ./include -I ./libs/sha-256/ -I./libs/log/ -i ./database/ -i ./database_dataBackup/ -i ./html/ -i ./log/ --suppress=missingIncludeSystem ./src/

wizard_by_return:
	@echo
	@echo "=== lib WIZARDBYRETURN ================="
	$(CC) -c -o$(LIBS_BIN_PATH)/$(LIB_WIZPATPATH).o $(WIZPATPATH)/$(LIB_WIZPATPATH).c -I$(WIZPATPATH) $(CFLAGS) -Wno-int-conversion
	$(AR) rc $(LIBS_BIN_PATH)/lib$(LIB_WIZPATPATH).a $(LIBS_BIN_PATH)/$(LIB_WIZPATPATH).o
	$(RANLIB) $(LIBS_BIN_PATH)/lib$(LIB_WIZPATPATH).a
	$(CP) $(WIZPATPATH)/$(LIB_WIZPATPATH).h $(LIBS_BIN_PATH)
	-$(RM) $(LIBS_BIN_PATH)/$(LIB_WIZPATPATH).o

llist:
	@echo
	@echo "=== lib LINKEDLIST ================="
	$(CC) -c -o$(LIBS_BIN_PATH)/$(LIB_LLIST).o $(LLISTPATH)/$(LIB_LLIST).c -I$(LLISTPATH) $(CFLAGS)
	$(AR) rc $(LIBS_BIN_PATH)/lib$(LIB_LLIST).a $(LIBS_BIN_PATH)/$(LIB_LLIST).o
	$(RANLIB) $(LIBS_BIN_PATH)/lib$(LIB_LLIST).a
	$(CP) $(LLISTPATH)/$(LIB_LLIST).h $(LIBS_BIN_PATH)
	-$(RM) $(LIBS_BIN_PATH)/$(LIB_LLIST).o

logtag:
	@echo
	@echo "=== lib LOG ================="
	$(CC) -c -o$(LIBS_BIN_PATH)/$(LIB_LOG).o $(LOGPATH)/$(LIB_LOG).c -I$(LOGPATH) $(CFLAGS)
	$(AR) rc $(LIBS_BIN_PATH)/lib$(LIB_LOG).a $(LIBS_BIN_PATH)/$(LIB_LOG).o
	$(RANLIB) $(LIBS_BIN_PATH)/lib$(LIB_LOG).a
	$(CP) $(LOGPATH)/$(LIB_LOG).h $(LIBS_BIN_PATH)
	-$(RM) $(LIBS_BIN_PATH)/$(LIB_LOG).o

sha256:
	@echo
	@echo "=== lib SHA256 =============="
	$(CC) -c -o$(LIBS_BIN_PATH)/$(LIB_SHA256).o $(SHA256PATH)/$(LIB_SHA256).c -I$(SHA256PATH) $(CFLAGS)
	$(AR) rc $(LIBS_BIN_PATH)/lib$(LIB_SHA256).a $(LIBS_BIN_PATH)/$(LIB_SHA256).o
	$(RANLIB) $(LIBS_BIN_PATH)/lib$(LIB_SHA256).a
	$(CP) $(SHA256PATH)/$(LIB_SHA256).h $(LIBS_BIN_PATH)
	-$(RM) $(LIBS_BIN_PATH)/$(LIB_SHA256).o

cfgfile:
	@echo
	@echo "=== lib CFGFILE ================="
	$(CC) -c -o$(LIBS_BIN_PATH)/$(LIB_CFGFILE).o $(CFGFILEPATH)/$(LIB_CFGFILE).c -I$(CFGFILEPATH) -I$(LLISTPATH) $(CFLAGS) -Wno-unused-value
	$(AR) rc $(LIBS_BIN_PATH)/lib$(LIB_CFGFILE).a $(LIBS_BIN_PATH)/$(LIB_CFGFILE).o
	$(RANLIB) $(LIBS_BIN_PATH)/lib$(LIB_CFGFILE).a
	$(CP) $(CFGFILEPATH)/$(LIB_CFGFILE).h $(LIBS_BIN_PATH)
	-$(RM) $(LIBS_BIN_PATH)/$(LIB_CFGFILE).o

pingServ: logtag
	@echo
	@echo "=== pingServ =================="
	$(CC) -o $(BINPATH)/pingServ $(SOURCEPATH)/pingServ.c $(SOURCEPATH)/util.c $(SOURCEPATH)/util_network.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -l$(LIB_SHA256) $(CFLAGS)

client: sha256 logtag
	@echo
	@echo "=== client =================="
	$(CC) -o $(BINPATH)/client $(SOURCEPATH)/client.c $(SOURCEPATH)/util.c $(SOURCEPATH)/util_network.c $(SOURCEPATH)/BL_client.c $(SOURCEPATH)/userId.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -l$(LIB_LLIST) -l$(LIB_SHA256) $(CFLAGS)

ncclient: sha256 logtag wizard_by_return llist cfgfile
	@echo
	@echo "=== ncclient =================="
	$(CC) -o $(BINPATH)/ncclient $(SOURCEPATH)/ncclient.c $(SOURCEPATH)/ncclient_util.c $(SOURCEPATH)/util.c $(SOURCEPATH)/util_network.c $(SOURCEPATH)/userId.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -lncurses -lform -l$(LIB_SHA256) -l$(LIB_LLIST) -l$(LIB_WIZPATPATH) -l$(LIB_CFGFILE) $(CFLAGS) -Wno-incompatible-pointer-types

sendRecvCmd:
	@echo
	@echo "=== sendRecvCmd =================="
	$(CC) -o $(BINPATH)/sendRecvCmd $(SOURCEPATH)/sendRecvCmd.c $(SOURCEPATH)/util.c $(SOURCEPATH)/util_network.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -l$(LIB_SHA256) -l$(LIB_CFGFILE) -l$(LIB_LLIST) $(CFLAGS)

serv: logtag
	@echo
	@echo "=== serv ===================="
	$(CC) -o $(BINPATH)/serv $(SOURCEPATH)/serv.c $(SOURCEPATH)/util.c $(SOURCEPATH)/util_network.c $(SOURCEPATH)/BL_serv.c $(SOURCEPATH)/db.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -l$(LIB_CFGFILE) -l$(LIB_LLIST) -l$(LIB_SHA256) $(CFLAGS)

select_html: logtag
	@echo
	@echo "=== select_html ============="
	$(CC) -o $(BINPATH)/select_html $(SOURCEPATH)/select_html.c $(SOURCEPATH)/util.c $(SOURCEPATH)/db.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -l$(LIB_SHA256) -l$(LIB_CFGFILE) -l$(LIB_LLIST) $(CFLAGS)

select_Excel:
	@echo
	@echo "=== select_Excel ============"
	$(CC) -o $(BINPATH)/select_Excel $(SOURCEPATH)/select_Excel.c $(SOURCEPATH)/util.c $(SOURCEPATH)/db.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -l$(LIB_SHA256) $(CFLAGS)

userIdDB: sha256
	@echo
	@echo "=== userIdDB ================="
	$(CC) -o $(BINPATH)/userIdDB $(SOURCEPATH)/userIdDB.c $(SOURCEPATH)/util.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -l$(LIB_SHA256) $(CFLAGS) -Wno-unused-variable

servList: logtag
	@echo
	@echo "=== servList ================"
	$(CC) -o $(BINPATH)/servList $(SOURCEPATH)/servList.c $(SOURCEPATH)/util.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -l$(LIB_LOG) -l$(LIB_SHA256) -l$(LIB_CFGFILE) -l$(LIB_LLIST) $(CFLAGS)

create_db: logtag
	@echo
	@echo "=== create_db ==============="
	$(CC) -o $(BINPATH)/create_db $(SOURCEPATH)/create_db.c $(SOURCEPATH)/util.c $(SOURCEPATH)/db.c $(INCLUDEPATH) -L$(LIBS_BIN_PATH) $(LIBS) -l$(LIB_SHA256) $(CFLAGS)

clean: clean_html clean_log clean_bin #clean_data
	@echo
	@echo "=== clean ==================="
	-$(RM) nohup.out tags cscope.out

clean_bin:
	@echo
	@echo "=== clean_bin ==============="
	-$(RM) $(BINPATH)/* $(LIBS_BIN_PATH)/*

clean_log:
	@echo
	@echo "=== clean_log ==============="
	-$(RM) ./log/*.log

clean_html:
	@echo
	@echo "=== clean_html =============="
	-$(RM) html/*.html

clean_data:
	@echo
	@echo "=== clean_data =============="
	-$(RM) ./database/*.db ./database/*.xls ./database/*.xlsx
