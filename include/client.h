/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * PAINEL
 *
 * Public Domain
 *
 */


/* client.h
 * Client interfaces.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 13/08/2018 | Creation
 *          |            |
 */


#ifndef __CLIENT_H__
#define __CLIENT_H__


/* *** INCLUDES ****************************************************** */


/* *** DEFINES ******************************************************* */
#define CORRIGIR_CMD		"corrigir"


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES *************************** */


/* *** DATA TYPES **************************************************** */
typedef enum{
	OKPC = 1,
	ERROPC,
	EXITPC,
	CORRIGIRPC
}tipoPreenchimento_t;


/* *** INTERFACES / PROTOTYPES *************************************** */
/* tipoPreenchimento_t preencherCampo(const char *pergunta, char *campo, size_t szCampo)
 *
 * Funcao que faz uma pergunta e captura um dado do teclado.
 *
 * pergunta - Pergunta para usuario
 * campo - Variavel que sera armazenada a digitacao
 * szCampo - Tamanho maximo de campo (limite USER_INPUT_LEN)
 *
 * Return:
 * OKPC - Usuario digitou um valor armazenado em campo
 * ERROPC - Erro na captura
 * EXITPC - Usuario digitou "exit"
 * CORRIGIRPC - Usuario digitou "corrigir"
 */
tipoPreenchimento_t preencherCampo(const char *pergunta, char *campo, size_t szCampo);

#endif
