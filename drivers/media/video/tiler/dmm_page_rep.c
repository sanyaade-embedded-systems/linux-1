/*
 * dmm_page_rep.c
 *
 * DMM driver support functions for TI OMAP processors.
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <linux/vmalloc.h>
#include <linux/mm.h>      /* vmalloc_to_page */
#include <linux/mmzone.h>  /* __page_to_phys */
#include "dmm_prv.h"

#define tilerdump(x) /*printk(KERN_NOTICE "%s::%s():%d: %lx\n",\
			__FILE__, __func__, __LINE__, (unsigned long)x); */
#ifdef CHECK_STACK
#define lajosdump(x) printk(KERN_NOTICE "%s::%s():%d: %s=%p\n",\
					__FILE__, __func__, __LINE__, #x, x);
#else
#define lajosdump(x)
#endif

unsigned long freePageCnt;
unsigned char *dmmPhysPages;
struct dmmPhysPgLLT *freePagesStack;
struct dmmPhysPgLLT *usedPagesStack;

#ifdef CHECK_STACK
static void print_stack(struct dmmPhysPgLLT *stack, char *prefix, int line)
{
	while (stack) {
		printk(KERN_ERR "%s%p<-%p->%p", prefix, stack->prevPhysPg,
						stack, stack->nextPhysPg);
		stack = stack->prevPhysPg;
		prefix = "";
	}
	printk(KERN_ERR "in line %d.", line);
}

static void check_stack(struct dmmPhysPgLLT *stack, char *prefix, int line)
{
	if (!stack)
		return;
	if (stack->nextPhysPg != NULL)
		print_stack(stack, prefix, line);
	else {
		struct dmmPhysPgLLT *ptr = stack;
		while (ptr) {
			if (ptr->prevPhysPg != NULL &&
					ptr->prevPhysPg->nextPhysPg != ptr) {
				print_stack(stack, prefix, line);
				return;
			}
			ptr = ptr->prevPhysPg;
		}
	}
}
#endif

/*--------function prototypes ---------------------------------*/

/* ========================================================================== */
/**
 *  dmmPhysPageRepRefil()
 *
 * @brief  Refills the physical page repository with the predefined page amount.
 *
 * @return none
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
void dmm_phys_page_rep_refil(void)
{
	unsigned long iter;

	tilerdump(0);
	for (iter = 0; iter < DMM_MNGD_PHYS_PAGES; iter++) {
		struct dmmPhysPgLLT *tmpPgNode = kmalloc
				(sizeof(struct dmmPhysPgLLT), GFP_KERNEL);
		memset(tmpPgNode, 0x0, sizeof(struct dmmPhysPgLLT));

		if (tmpPgNode != NULL) {

			tmpPgNode->nextPhysPg = NULL;
			tmpPgNode->prevPhysPg = NULL;
			tmpPgNode->physPgPtr =
				(unsigned long *)__get_free_page(GFP_KERNEL);

			/* add to end */
			if (freePagesStack != NULL) {
				if (freePagesStack->nextPhysPg != NULL)
					lajosdump(freePagesStack->nextPhysPg);
				freePagesStack->nextPhysPg = tmpPgNode;
				tmpPgNode->prevPhysPg = freePagesStack;
			}

			freePagesStack = tmpPgNode;
			freePageCnt++;
		} else {
			printk(KERN_ERR "error\n");
		}
	}

#ifdef CHECK_STACK
	check_stack(freePagesStack, "free: ", __LINE__);
#endif
}

/* ========================================================================== */
/**
 *  dmmPhysPageRepInit()
 *
 * @brief  Initializes the physical memory page repository instance.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_phys_page_rep_init(void)
{
	/* DMM_ENTER_CRITICAL_SECTION */

	freePagesStack = NULL;
	usedPagesStack = NULL;



	freePageCnt = 0;

	dmm_phys_page_rep_refil();

	/* DMM_EXIT_CRITICAL_SETCTION */
	return DMM_NO_ERROR;
}

/* ========================================================================== */
/**
 *  dmmPhysPageRepDeinit()
 *
 * @brief  Releases all resources held by the physical memory page repository
 * instance.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_phys_page_rep_deinit(void)
{
	/* DMM_ENTER_CRITICAL_SECTION */
	struct dmmPhysPgLLT *tmpPgNode = NULL;

	while (usedPagesStack != NULL) {
		tmpPgNode = usedPagesStack->prevPhysPg;
		free_page((unsigned long)usedPagesStack->physPgPtr);
		kfree(usedPagesStack);
		usedPagesStack = tmpPgNode;
	}

	while (freePagesStack != NULL) {
		tmpPgNode = freePagesStack->prevPhysPg;
		free_page((unsigned long)freePagesStack->physPgPtr);
		kfree(freePagesStack);
		freePagesStack = tmpPgNode;
	}

	freePageCnt = 0;

	/* DMM_EXIT_CRITICAL_SETCTION */
	return DMM_NO_ERROR;
}

/* ========================================================================== */
/**
 *  dmmGetPhysPage()
 *
 * @brief  Return a pointer to a physical memory page and mark it as used.
 *
 * @return MSP_U32* pointer to a physical memory page, NULL if error occurs.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
unsigned long *dmm_get_phys_page(void)
{
	unsigned long *physPgPtr = NULL;

	/* DMM_ENTER_CRITICAL_SECTION */

	tilerdump(0);
	if (freePagesStack == NULL)
		dmm_phys_page_rep_refil();

	if (freePagesStack != NULL) {
		struct dmmPhysPgLLT *tmpPgNode = freePagesStack;

		/* remove page from top of freepages */
		if (freePagesStack->nextPhysPg != NULL)
			lajosdump(freePagesStack->nextPhysPg);
		freePagesStack = freePagesStack->prevPhysPg;
		if (freePagesStack != NULL)
			freePagesStack->nextPhysPg = NULL;

		/* add page to top of usedpages */
		tmpPgNode->prevPhysPg = usedPagesStack;
		if (usedPagesStack != NULL) {
			if (usedPagesStack->nextPhysPg != NULL)
				lajosdump(usedPagesStack->nextPhysPg);
			usedPagesStack->nextPhysPg = tmpPgNode;
		}
		tmpPgNode->nextPhysPg = NULL;
		usedPagesStack = tmpPgNode;

		physPgPtr = tmpPgNode->physPgPtr;
		freePageCnt--;
	}

#ifdef CHECK_STACK
	check_stack(freePagesStack, "free: ", __LINE__);
	check_stack(usedPagesStack, "used: ", __LINE__);
#endif
	/* DMM_EXIT_CRITICAL_SETCTION */
	return physPgPtr;
}

/* ========================================================================== */
/**
 *  dmmFreePhysPage()
 *
 * @brief  Frees a specified physical memory page.
 *
 * @param physPgPtr - MSP_U32* - [in] The address of the allocated physical page
 * that should be freed.
 *
 * @return errorCodeT
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see errorCodeT for further detail.
 */
/* ========================================================================== */
enum errorCodeT dmm_free_phys_page(unsigned long *physPgPtr)
{
	struct dmmPhysPgLLT *iterPgNode = usedPagesStack;

	/* DMM_ENTER_CRITICAL_SECTION */

	while (iterPgNode != NULL) {

		if (iterPgNode->physPgPtr == physPgPtr) {
			/* find address in used pages stack */

			/* remove from list */
			if (iterPgNode->prevPhysPg != NULL) {
				iterPgNode->prevPhysPg->nextPhysPg =
					iterPgNode->nextPhysPg;
			}

			if (iterPgNode->nextPhysPg != NULL) {
				iterPgNode->nextPhysPg->prevPhysPg =
					iterPgNode->prevPhysPg;
			} else if (iterPgNode == usedPagesStack) {
				usedPagesStack = usedPagesStack->prevPhysPg;
			} else {
				/* DMM_EXIT_CRITICAL_SETCTION */
				lajosdump(iterPgNode);
				return DMM_SYS_ERROR;
			}

			/* add to end of freepages */
			if (freePagesStack != NULL)
				freePagesStack->nextPhysPg = iterPgNode;
			iterPgNode->prevPhysPg = freePagesStack;
			freePagesStack = iterPgNode;
			freePageCnt++;

			while (freePageCnt > DMM_MNGD_PHYS_PAGES &&
						freePagesStack != NULL) {
				iterPgNode = freePagesStack->prevPhysPg;
				free_page((unsigned long)
						freePagesStack->physPgPtr);
				kfree(freePagesStack);
				freePagesStack = iterPgNode;
				freePageCnt--;
			}
			freePagesStack->nextPhysPg = NULL;

			/* DMM_EXIT_CRITICAL_SETCTION */
#ifdef CHECK_STACK
			check_stack(freePagesStack, "free: ", __LINE__);
			check_stack(usedPagesStack, "used: ", __LINE__);
#endif
			return DMM_NO_ERROR;
		}

		iterPgNode = iterPgNode->prevPhysPg;
	}

	/* DMM_EXIT_CRITICAL_SETCTION */
#ifdef CHECK_STACK
	check_stack(freePagesStack, "free: ", __LINE__);
	check_stack(usedPagesStack, "used: ", __LINE__);
#endif
	return DMM_WRONG_PARAM;
}

