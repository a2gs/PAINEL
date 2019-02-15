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


#USAR ESTAS FLAGS PARA DEV PERNICIOSA:
CFLAGS=-Wall -Wextra -g -std=c11 -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L -D_POSIX_SOURCE=1 -D_DEFAULT_SOURCE=1 -D_GNU_SOURCE=1 -Wno-unused-parameter -Wno-unused-but-set-parameter
#USAR ESTAS FLAGS PARA DEV RIGOROSO:
	#CFLAGS=-Wall -Wextra -g -std=c11 -D_XOPEN_SOURCE=700
#USAR ESTAS FLAGS PARA DEPLOY:
	#CFLAGS=-Wall -O3 -std=c11 -D_XOPEN_SOURCE=700 -Wno-unused-result

CC=gcc

INCLUDEPATH=-I./include
SOURCEPATH=./src
BINPATH=./bin
LIBS=./libs

SHA256PATH=$(LIBS)/sha-256

RM = rm -f
CPPCHECK = cppcheck
CPPCHECK_OPTS = --enable=all --std=c11 --platform=unix64 --language=c --check-config --suppress=missingIncludeSystem

all: clean client serv select_html select_Excel servList create_db cppcheck
	@echo "=== ctags ==================="
	ctags -R *
	@echo "=== cscope =================="
	cscope -b -R

cppcheck:
	@echo "=== cppcheck ================"
	$(CPPCHECK) $(CPPCHECK_OPTS) -I ./include -I ./libs/sha-256/ -i ./database/ -i ./database_dataBackup/ -i ./html/ -i ./log/ -i ./ncurses/ --suppress=missingIncludeSystem  ./src/

client:
	@echo "=== client =================="
	$(CC) -o $(BINPATH)/client $(SOURCEPATH)/client.c $(SOURCEPATH)/util.c $(SOURCEPATH)/SG_client.c $(SHA256PATH)/sha-256.c $(INCLUDEPATH) -I$(SHA256PATH) $(CFLAGS)

serv:
	@echo "=== serv ===================="
	$(CC) -o $(BINPATH)/serv $(SOURCEPATH)/serv.c $(SOURCEPATH)/util.c $(SOURCEPATH)/SG_serv.c $(INCLUDEPATH) -lsqlite3 $(CFLAGS)

select_html:
	@echo "=== select_html ============="
	$(CC) -o $(BINPATH)/select_html $(SOURCEPATH)/select_html.c $(SOURCEPATH)/util.c $(INCLUDEPATH) -lsqlite3 $(CFLAGS)

select_Excel:
	@echo "=== select_Excel ============"
	$(CC) -o $(BINPATH)/select_Excel $(SOURCEPATH)/select_Excel.c $(SOURCEPATH)/util.c $(INCLUDEPATH) -lsqlite3 $(CFLAGS)

servList:
	@echo "=== servList ================"
	$(CC) -o $(BINPATH)/servList $(SOURCEPATH)/servList.c $(SOURCEPATH)/util.c $(INCLUDEPATH) $(CFLAGS)

create_db:
	@echo "=== create_db ==============="
	$(CC) -o $(BINPATH)/create_db $(SOURCEPATH)/create_db.c $(SOURCEPATH)/util.c $(INCLUDEPATH) -lsqlite3 $(CFLAGS)

clean: clean_html clean_log clean_bin #clean_data
	@echo "=== clean ==================="
	-$(RM) nohup.out tags cscope.out

clean_bin:
	@echo "=== clean_bin ==============="
	-$(RM) $(BINPATH)/*

clean_log:
	@echo "=== clean_log ==============="
	-$(RM) ./log/*.log

clean_html:
	@echo "=== clean_html =============="
	-$(RM) html/*.html

clean_data:
	@echo "=== clean_data =============="
	-$(RM) ./database/*.db ./database/*.xls ./database/*.xlsx
