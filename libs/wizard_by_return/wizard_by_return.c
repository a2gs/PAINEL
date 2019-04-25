/* Andre Augusto Giannotti Scota Tool Box Library.
 * andre.scota@gmail.com
 *
 * MIT License
 *
 */


/* wizard_by_return.c
 * This file implement the 'run_functions_in_a_list' facillity.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 2005 06 30 | Creation v0.1
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include "wizard_by_return.h"


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */


/* *** LOCAL PROTOTYPES **************************************************************** */


/* *** EXTERNS / LOCAL GLOBALS ********************************************************* */



/* *** INTERFACES / PROTOTYPES ********************************************************* */
int a2gs_ToolBox_WizardReturn(a2gs_ToolBox_WizardReturnFunc_t init, void *data)
{
	a2gs_ToolBox_WizardReturnFunc_t ret;

	ret = init;

	for(ret = init; ret != NULL; ret = ret(data));

	return(0);
}
