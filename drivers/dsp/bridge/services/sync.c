/*
 * sync.c
 *
 * DSP-BIOS Bridge driver support functions for TI OMAP processors.
 *
 * Synchronization services.
 *
 * Copyright (C) 2005-2006 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*  ----------------------------------- Host OS */
#include <dspbridge/host_os.h>

/*  ----------------------------------- DSP/BIOS Bridge */
#include <dspbridge/std.h>
#include <dspbridge/dbdefs.h>
#include <dspbridge/errbase.h>

/*  ----------------------------------- Trace & Debug */
#include <dspbridge/dbc.h>

/*  ----------------------------------- OS Adaptation Layer */
#include <dspbridge/mem.h>

/*  ----------------------------------- This */
#include <dspbridge/sync.h>

DEFINE_SPINLOCK(sync_lock);

/*  ----------------------------------- Defines, Data Structures, Typedefs */
#define SIGNATURE       0x434e5953	/* "SYNC" (in reverse) */

enum wait_state {
	WO_WAITING,
	WO_SIGNALLED
};

enum sync_state {
	SO_RESET,
	SO_SIGNALLED
};

struct wait_object {
	enum wait_state state;
	struct sync_object *signalling_event;
	struct semaphore sem;
};

struct sync_dpccsobject {
	u32 dw_signature;	/* used for object validation */
	spinlock_t sync_dpccs_lock;
	s32 count;
};

static int test_and_set(volatile void *ptr, int val)
{
	int ret = val;
	asm volatile (" swp %0, %0, [%1]":"+r" (ret):"r"(ptr):"memory");
	return ret;
}

static void timeout_callback(unsigned long hWaitObj);

/*
 *  ======== sync_exit ========
 *  Purpose:
 *      Cleanup SYNC module.
 */
void sync_exit(void)
{
	/* Do nothing */
}

/*
 *  ======== sync_init ========
 *  Purpose:
 *      Initialize SYNC module.
 */
bool sync_init(void)
{
	return true;
}

/**
 * sync_set_event() - set or signal and specified event
 * @event:	Event to be set..
 *
 * set the @event, if there is an thread waiting for the event
 * it will be waken up, this function only wakes one thread.
 */

void sync_set_event(struct sync_object *event)
{
	spin_lock_bh(&sync_lock);
	complete(&event->comp);
	if (event->multi_comp)
		complete(event->multi_comp);
	spin_unlock_bh(&sync_lock);
}

/**
 * sync_wait_on_multiple_events() - waits for multiple events to be set.
 * @events:	Array of events to wait for them.
 * @count:	number of elements of the array.
 * @timeout	timeout on waiting for the evetns.
 * @pu_index	index of the event set.
 *
 * This functios will wait until any of the array element is set or until
 * timeout. In case of success the function will return DSP_SOK and
 * @pu_index will store the index of the array element set or in case
 * of timeout the function will return DSP_ETIMEOUT or in case of
 * interrupting by a signal it will return DSP_EFAIL.
 */

dsp_status sync_wait_on_multiple_events(struct sync_object **events,
				     unsigned count, unsigned timeout,
				     unsigned *index)
{
	unsigned i;
	dsp_status status = DSP_EFAIL;
	struct completion m_comp;

	init_completion(&m_comp);

	spin_lock_bh(&sync_lock);
	for (i = 0; i < count; i++) {
		if (completion_done(&events[i]->comp)) {
			INIT_COMPLETION(events[i]->comp);
			*index = i;
			spin_unlock_bh(&sync_lock);
			status = DSP_SOK;
			goto func_end;
		}
	}

	for (i = 0; i < count; i++)
		events[i]->multi_comp = &m_comp;

	spin_unlock_bh(&sync_lock);

	if (!wait_for_completion_interruptible_timeout(&m_comp,
					msecs_to_jiffies(timeout)))
		status = DSP_ETIMEOUT;

	spin_lock_bh(&sync_lock);
	for (i = 0; i < count; i++) {
		if (completion_done(&events[i]->comp)) {
			INIT_COMPLETION(events[i]->comp);
			*index = i;
			status = DSP_SOK;
		}
		events[i]->multi_comp = NULL;
	}
	spin_unlock_bh(&sync_lock);
func_end:
	return status;
}

static void timeout_callback(unsigned long hWaitObj)
{
	struct wait_object *wait_obj = (struct wait_object *)hWaitObj;
	if (test_and_set(&wait_obj->state, WO_SIGNALLED) == WO_WAITING)
		up(&wait_obj->sem);

}

/*
 *  ======== sync_delete_cs ========
 */
dsp_status sync_delete_cs(struct sync_csobject *hcs_obj)
{
	dsp_status status = DSP_SOK;
	struct sync_csobject *pcs_obj = (struct sync_csobject *)hcs_obj;

	if (MEM_IS_VALID_HANDLE(hcs_obj, SIGNATURECS)) {
		if (down_trylock(&pcs_obj->sem) != 0)
			DBC_ASSERT(0);

		MEM_FREE_OBJECT(hcs_obj);
	} else if (MEM_IS_VALID_HANDLE(hcs_obj, SIGNATUREDPCCS)) {
		struct sync_dpccsobject *pdpccs_obj =
		    (struct sync_dpccsobject *)hcs_obj;
		if (pdpccs_obj->count != 1)
			DBC_ASSERT(0);

		MEM_FREE_OBJECT(pdpccs_obj);
	} else {
		status = DSP_EHANDLE;
	}

	return status;
}

/*
 *  ======== sync_enter_cs ========
 */
dsp_status sync_enter_cs(struct sync_csobject *hcs_obj)
{
	dsp_status status = DSP_SOK;
	struct sync_csobject *pcs_obj = (struct sync_csobject *)hcs_obj;

	if (MEM_IS_VALID_HANDLE(hcs_obj, SIGNATURECS)) {
		if (in_interrupt()) {
			status = DSP_EFAIL;
			DBC_ASSERT(0);
		} else if (down_interruptible(&pcs_obj->sem)) {
			status = DSP_EFAIL;
		}
	} else if (MEM_IS_VALID_HANDLE(hcs_obj, SIGNATUREDPCCS)) {
		struct sync_dpccsobject *pdpccs_obj =
		    (struct sync_dpccsobject *)hcs_obj;
		spin_lock_bh(&pdpccs_obj->sync_dpccs_lock);
		pdpccs_obj->count--;
		if (pdpccs_obj->count != 0) {
			/* FATAL ERROR : Failed to acquire DPC CS */
			spin_unlock_bh(&pdpccs_obj->sync_dpccs_lock);
			DBC_ASSERT(0);
		}
	} else {
		status = DSP_EHANDLE;
	}

	return status;
}

/*
 *  ======== sync_initialize_cs ========
 */
dsp_status sync_initialize_cs(OUT struct sync_csobject **phCSObj)
{
	dsp_status status = DSP_SOK;
	struct sync_csobject *pcs_obj = NULL;

	/* Allocate memory for sync CS object */
	MEM_ALLOC_OBJECT(pcs_obj, struct sync_csobject, SIGNATURECS);
	if (pcs_obj != NULL)
		init_MUTEX(&pcs_obj->sem);
	else
		status = DSP_EMEMORY;

	/* return CS object */
	*phCSObj = pcs_obj;
	DBC_ASSERT(DSP_FAILED(status) || (pcs_obj));
	return status;
}

dsp_status sync_initialize_dpccs(OUT struct sync_csobject **phCSObj)
{
	dsp_status status = DSP_SOK;
	struct sync_dpccsobject *pcs_obj = NULL;

	DBC_REQUIRE(phCSObj);

	if (phCSObj) {
		/* Allocate memory for sync CS object */
		MEM_ALLOC_OBJECT(pcs_obj, struct sync_dpccsobject,
				 SIGNATUREDPCCS);
		if (pcs_obj != NULL) {
			pcs_obj->count = 1;
			spin_lock_init(&pcs_obj->sync_dpccs_lock);
		} else {
			status = DSP_EMEMORY;
		}

		/* return CS object */
		*phCSObj = (struct sync_csobject *)pcs_obj;
	} else {
		status = DSP_EPOINTER;
	}

	DBC_ASSERT(DSP_FAILED(status) || (pcs_obj));

	return status;
}

/*
 *  ======== sync_leave_cs ========
 */
dsp_status sync_leave_cs(struct sync_csobject *hcs_obj)
{
	dsp_status status = DSP_SOK;
	struct sync_csobject *pcs_obj = (struct sync_csobject *)hcs_obj;

	if (MEM_IS_VALID_HANDLE(hcs_obj, SIGNATURECS)) {
		up(&pcs_obj->sem);
	} else if (MEM_IS_VALID_HANDLE(hcs_obj, SIGNATUREDPCCS)) {
		struct sync_dpccsobject *pdpccs_obj =
		    (struct sync_dpccsobject *)hcs_obj;
		pdpccs_obj->count++;
		if (pdpccs_obj->count != 1) {
			/* FATAL ERROR : Invalid DPC CS count */
			spin_unlock_bh(&pdpccs_obj->sync_dpccs_lock);
			DBC_ASSERT(0);
			spin_lock_bh(&pdpccs_obj->sync_dpccs_lock);
		}
		spin_unlock_bh(&pdpccs_obj->sync_dpccs_lock);
	} else {
		status = DSP_EHANDLE;
	}

	return status;
}
