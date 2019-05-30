BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS `USRIFACE` (
	`FUNCAO`	TEXT,
	`FIELD`	TEXT,
	`FTYPE`	TEXT,
	`FFMT`	TEXT,
	`FDESC`	TEXT,
	`FORDER`	TEXT,
	FOREIGN KEY(`FUNCAO`) REFERENCES `USERLEVEL`(`FUNCAO`)
);
CREATE TABLE IF NOT EXISTS `USERS` (
	`ID`	TEXT NOT NULL,
	`FUNCAO`	TEXT NOT NULL,
	`PASSHASH`	TEXT,
	FOREIGN KEY(`FUNCAO`) REFERENCES `USERLEVEL`(`FUNCAO`),
	PRIMARY KEY(`ID`)
);
CREATE TABLE IF NOT EXISTS `USERLEVEL` (
	`FUNCAO`	TEXT,
	PRIMARY KEY(`FUNCAO`)
);
CREATE TABLE IF NOT EXISTS `RELATS` (
	`FUNCAO`	TEXT,
	`TITULO`	TEXT,
	`CAMPOS`	TEXT,
	`HEADERS`	TEXT,
	FOREIGN KEY(`FUNCAO`) REFERENCES `USERLEVEL`(`FUNCAO`),
	PRIMARY KEY(`FUNCAO`)
);
CREATE TABLE IF NOT EXISTS `MSGS` (
	`DRT`	TEXT NOT NULL,
	`DATAHORA`	TEXT NOT NULL,
	`LOGINOUT`	TEXT,
	`FUNCAO`	TEXT NOT NULL,
	`PANELA`	TEXT,
	`WS`	TEXT,
	`FORNELETR`	TEXT,
	`NUMMAQUINA`	TEXT,
	`DIAMETRO`	TEXT,
	`CLASSE`	TEXT,
	`TEMP`	TEXT,
	`PERCFESI`	TEXT,
	`PERCMG`	TEXT,
	`PERCC`	TEXT,
	`PERCS`	TEXT,
	`PERCP`	TEXT,
	`PERCINOCLNT`	TEXT,
	`ENELETTON`	TEXT,
	`CADENCIA`	TEXT,
	`OEE`	TEXT,
	`ASPECTUBO`	TEXT,
	`REFUGO`	TEXT,
	`IPPORT`	TEXT,
	PRIMARY KEY(`DATAHORA`,`DRT`)
);
CREATE INDEX IF NOT EXISTS `ID_INDX` ON `USERS` (
	`ID`
);
CREATE INDEX IF NOT EXISTS `FUNC_INDX` ON `USERLEVEL` (
	`FUNCAO`
);
CREATE INDEX IF NOT EXISTS `FUNCIF_INDX` ON `USRIFACE` (
	`FUNCAO`
);
CREATE INDEX IF NOT EXISTS `FUNCAO_INDX` ON `MSGS` (
	`FUNCAO`
);
CREATE INDEX IF NOT EXISTS `DATAHORA_INDX` ON `MSGS` (
	`DATAHORA`
);
COMMIT;