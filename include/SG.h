/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Apache License 2.0
 *
 */


/* SG.h
 * <File description>
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


#ifndef __SG_H__
#define __SG_H__


/* *** INCLUDES ****************************************************** */


/* *** DEFINES ******************************************************* */
/* PROTOCOLO "PADRAO" (para comando PROT_COD_INSREG):
   COD|DRT|DATAHORA|LOGINOUT|PANELA|WS|FORNELETR|NUMMAQUINA|DIAMETRO|CLASSE|TEMP|PERCFESI|PERCMG|PERCC|PERCS|PERCP|PERCINOCLNT|ENELETTON|CADENCIA|OEE|ASPECTUBO|REFUGO
*/

#define DRT_LEN               (25)
#define PASS_LEN              (100)
#define PASS_SHA256_BIN_LEN   (32)
#define PASS_SHA256_ASCII_LEN (64)
#define IV_SHA256_LEN         (32)

#define VALOR_PANELA_LEN			(3)
#define VALOR_PORCENTAGEM_LEN		(sizeof("xxx,yy"))
#define VALOR_TEMPERATURA_LEN		(10)
#define VALOR_ASPECTO_LEN			(200)
#define VALOR_FUNCAO_LEN			(35)

#define VALOR_IPPORT_LEN			(sizeof("aaa.bbb.ccc.ddd:pppppp"))

#define TOT_PERGUNTAS_FORNO_ELET		(6)
#define FORNELET_PERC_FESI_LEN		(VALOR_PORCENTAGEM_LEN)
#define FORNELET_PERC_S_LEN			(VALOR_PORCENTAGEM_LEN)
#define FORNELET_PERC_MG_LEN			(VALOR_PORCENTAGEM_LEN)
#define FORNELET_PAMELA_LEN 			(VALOR_PANELA_LEN)
#define FORNELET_TEMP_LEN				(VALOR_TEMPERATURA_LEN)
#define FORNELET_NUMFORELE_LEN		(5)
#define FORNELET_PERC_C_LEN			(VALOR_PORCENTAGEM_LEN) /* TODO: VERIFICAR O TAMANO DESTE DADO */
#define FORNELET_PERC_P_LEN			(VALOR_PORCENTAGEM_LEN) /* TODO: VERIFICAR O TAMANO DESTE DADO */

#define TOT_PERGUNTAS_OPE_MAQUINA	(9)
#define OPEMAQ_PERC_FESI_LEN			(VALOR_PORCENTAGEM_LEN)
#define OPEMAQ_PERC_INOC_LEN			(VALOR_PORCENTAGEM_LEN)
#define OPEMAQ_ASPEC_LEN				(VALOR_ASPECTO_LEN)
#define OPEMAQ_PANELA_LEN				(VALOR_PANELA_LEN)
#define OPEMAQ_DIAMNOM_LEN				(6)
#define OPEMAQ_CLASSE_LEN           (6)
#define OPEMAQ_WS_LEN					(6)
#define OPEMAQ_TEMP_LEN					(VALOR_TEMPERATURA_LEN)
#define OPEMAQ_NUMMAQ_LEN				(2)
#define OPEMAQ_ENEELETON_LEN			(9)

#define TOT_PERGUNTAS_SUP_MAQUINA	(4)
#define SUPMAQ_ASPEC_LEN				(VALOR_ASPECTO_LEN)
#define SUPMAG_REFUGO_LEN				(200)
#define SUPMAQ_CADENCIA					(6)
#define SUPMAQ_OEE						(6)


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */


/* *** INTERFACES / PROTOTYPES *************************************** */
/* <header function description included by another sources> void function(void)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */

#endif
