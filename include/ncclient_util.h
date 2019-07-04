/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* sc.h
 * Project's auxiliary functions (client/server).
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


#ifndef __NCCLIENT_UTIL_H__
#define __NCCLIENT_UTIL_H__


/* *** INCLUDES ****************************************************** */


/* *** DEFINES ******************************************************* */
#define USR_IFACE_FIELD_SZ     (25)
#define USR_IFACE_FMTFIELD_SZ  (15)
#define USR_IFACE_DESCFIELD_SZ (40)
#define USR_IFACE_TOTAL_FIELDS (10) /* IFACE cmd. At first version, only 10 fields */

#define FTYPE_MAX_SZ           (20) /* Max size of field type (below) */

#define TOT_MENU_LEVELS_LABEL  (40)
#define LEVEL_DESCRIPTION_SZ   (20) /* TODO: send to db.h or where else */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */
typedef enum{
	TEXT_USRFIELD = 1,
	NUM_USRFIELD,
	DATE_USRFIELD,
	UNDEFINED_USRFIELD
}usrFieldType_t;

typedef struct _levelMenu_t{
	char levelDesc[LEVEL_DESCRIPTION_SZ + 1];
	usrFieldType_t type;
}levelMenu_t;

typedef struct _usrField_t{ /* IFACE cmd. Dowloaded from server at correct order to display. Sizes below are fixed at first version. */
	char field[USR_IFACE_FIELD_SZ + 1];
	usrFieldType_t type;
	char fmt[USR_IFACE_FMTFIELD_SZ + 1];
	char desc[USR_IFACE_DESCFIELD_SZ + 1];
}usrField_t;

typedef struct _usrFieldCtrl_t{ /* IFACE cmd. A dynamic list and self-interface (at userId.h) in futere versions. */
	unsigned int totFields;
	usrField_t fields[USR_IFACE_TOTAL_FIELDS];
}usrFieldCtrl_t;


/* *** INTERFACES / PROTOTYPES *************************************** */

/* 
 *
 * 
 *
 * INPUT:
 *  none
 * OUTPUT:
 *  none
 */
void usrIfaceFieldsClean(void);
int usrIsIfaceFieldsEmpty(void);
int usrIfaceFieldAdd(char *ffield, char *ftype, char *ffmt, char *fdesc);

#endif
