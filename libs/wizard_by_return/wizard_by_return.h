/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * MIT License
 *
 */


/* wizard_by_return.h
 *
 * A pattern to follow a list of function, in sequence until a error.
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 2005 06 30 | Creation v0.1
 *          |            |
 */


#ifndef __A2GS_TOOLBOX_WIZARDRETURN_H__
#define __A2GS_TOOLBOX_WIZARDRETURN_H__


/* *** INCLUDES ************************************************************************ */
#include <stdint.h>


/* *** DEFINES ************************************************************************* */


/* *** DATA TYPES ********************************************************************** */
typedef uintptr_t (*a2gs_ToolBox_WizardReturnFunc_t) (void *);


/* *** INTERFACES / PROTOTYPES ********************************************************* */
/*
 *
 * INPUT:
 *  data - Data passed through functions in list.
 * OUTPUT:
 */
int a2gs_ToolBox_WizardReturn(a2gs_ToolBox_WizardReturnFunc_t init, void *data);


/* *** EXAMPLE ************************************************************************* */
#if 0

	#include <stdio.h>
	#include "wizard_by_return.h"

	a2gs_ToolBox_WizardReturnFunc_t func1(void *data);
	a2gs_ToolBox_WizardReturnFunc_t func2(void *data);
	a2gs_ToolBox_WizardReturnFunc_t func3(void *data);
	a2gs_ToolBox_WizardReturnFunc_t func4(void *data);

	a2gs_ToolBox_WizardReturnFunc_t func1(void *data)
	{
		a2gs_ToolBox_WizardReturnFunc_t ret = NULL;
		int x = *((int *) data);

		printf("func 1: [%d]\n", x);
		return(ret);
	}

	a2gs_ToolBox_WizardReturnFunc_t func2(void *data)
	{
		a2gs_ToolBox_WizardReturnFunc_t ret = NULL;
		int x = *((int *) data);

		printf("func 2: [%d]\n", x);
		return(ret);
	}

	a2gs_ToolBox_WizardReturnFunc_t func3(void *data)
	{
		a2gs_ToolBox_WizardReturnFunc_t ret = func4;
		int x = *((int *) data);

		printf("func 3: [%d]\n", x);
		return(ret);
	}

	a2gs_ToolBox_WizardReturnFunc_t func4(void *data)
	{
		a2gs_ToolBox_WizardReturnFunc_t ret = func1;
		int x = *((int *) data);

		printf("func 4: [%d]\n", x);
		return(ret);
	}

	int main(int argc, char *argv[])
	{
		int x = 13;

		a2gs_ToolBox_WizardReturn(func3, (void *)&x);

		return(0);
	}

#endif

#endif
