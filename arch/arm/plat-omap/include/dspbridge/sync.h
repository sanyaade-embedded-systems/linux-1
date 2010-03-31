/*
 * sync.h
 *
 * DSP-BIOS Bridge driver support functions for TI OMAP processors.
 *
 * Provide synchronization services.
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

#ifndef _SYNC_H
#define _SYNC_H

#include <dspbridge/errbase.h>
#define SIGNATURECS     0x53435953	/* "SYCS" (in reverse) */
#define SIGNATUREDPCCS  0x53445953	/* "SYDS" (in reverse) */

/* Special timeout value indicating an infinite wait: */
#define SYNC_INFINITE  0xffffffff

/* Maximum string length of a named event */
#define SYNC_MAXNAMELENGTH 32

/**
 * struct sync_object - the basic sync_object structure
 * @comp:	use to signal events
 * @multi_comp:	use to signal multiple events.
 *
 */
struct sync_object{
	struct completion comp;
	struct completion *multi_comp;
};

/* Generic SYNC CS object: */
struct sync_csobject {
	u32 dw_signature;	/* used for object validation */
	struct semaphore sem;
};

/* SYNC object attributes: */
struct sync_attrs {
	bhandle user_event;	/* Platform's User Mode synch. object. */
	bhandle kernel_event;	/* Platform's Kernel Mode sync. object. */
	u32 dw_reserved1;	/* For future expansion. */
	u32 dw_reserved2;	/* For future expansion. */
};

/*
 *  ======== sync_delete_cs ========
 *  Purpose:
 *      Delete a critical section.
 *  Parameters:
 *      hcs_obj: critical section handle.
 *  Returns:
 *      DSP_SOK:        Success.
 *      DSP_EHANDLE:    Invalid handle.
 *  Requires:
 *  Ensures:
 */
extern dsp_status sync_delete_cs(IN struct sync_csobject *hcs_obj);

/*
 *  ======== sync_enter_cs ========
 *  Purpose:
 *      Enter the critical section.
 *  Parameters:
 *      hcs_obj: critical section handle.
 *  Returns:
 *      DSP_SOK:        Success.
 *      DSP_EHANDLE:    Invalid handle.
 *  Requires:
 *  Ensures:
 */
extern dsp_status sync_enter_cs(IN struct sync_csobject *hcs_obj);

/*
 *  ======== sync_exit ========
 *  Purpose:
 *      Discontinue usage of module; free resources when reference count
 *      reaches 0.
 *  Parameters:
 *  Returns:
 *  Requires:
 *      SYNC initialized.
 *  Ensures:
 *      Resources used by module are freed when cRef reaches zero.
 */
extern void sync_exit(void);

/*
 *  ======== sync_init ========
 *  Purpose:
 *      Initializes private state of SYNC module.
 *  Parameters:
 *  Returns:
 *      TRUE if initialized; FALSE if error occured.
 *  Requires:
 *  Ensures:
 *      SYNC initialized.
 */
extern bool sync_init(void);

/*
 *  ======== sync_initialize_cs ========
 *  Purpose:
 *      Initialize the critical section.
 *  Parameters:
 *      hcs_obj: critical section handle.
 *  Returns:
 *      DSP_SOK:        Success.
 *      DSP_EMEMORY:    Out of memory.
 *  Requires:
 *  Ensures:
 */
extern dsp_status sync_initialize_cs(OUT struct sync_csobject **phCSObj);

/*
 *  ======== sync_initialize_dpccs ========
 *  Purpose:
 *      Initialize the critical section between process context and DPC.
 *  Parameters:
 *      hcs_obj: critical section handle.
 *  Returns:
 *      DSP_SOK:        Success.
 *      DSP_EMEMORY:    Out of memory.
 *  Requires:
 *  Ensures:
 */
extern dsp_status sync_initialize_dpccs(OUT struct sync_csobject
					**phCSObj);

/*
 *  ======== sync_leave_cs ========
 *  Purpose:
 *      Leave the critical section.
 *  Parameters:
 *      hcs_obj: critical section handle.
 *  Returns:
 *      DSP_SOK:        Success.
 *      DSP_EHANDLE:    Invalid handle.
 *  Requires:
 *  Ensures:
 */
extern dsp_status sync_leave_cs(IN struct sync_csobject *hcs_obj);

/**
 * sync_init_event() - set initial state for a sync_event element
 * @event:	event to be initialized.
 *
 * Set the initial state for a sync_event element.
 */

static inline void sync_init_event(struct sync_object *event)
{
	init_completion(&event->comp);
	event->multi_comp = NULL;
}

/*
 * ========= sync_post_message ========
 *  Purpose:
 *      To post a windows message
 *  Parameters:
 *      hWindow:    Handle to the window
 *      uMsg:       Message to be posted
 *  Returns:
 *      DSP_SOK:        Success
 *      DSP_EFAIL:      Post message failed
 *      DSP_EHANDLE:    Invalid Window handle
 *  Requires:
 *      SYNC initialized
 *  Ensures
 */
extern dsp_status sync_post_message(IN bhandle hWindow, IN u32 uMsg);

/**
 * sync_reset_event() - reset a sync_event element
 * @event:	event to be reset.
 *
 * This function reset to the initial state to @event.
 */

static inline void sync_reset_event(struct sync_object *event)
{
	INIT_COMPLETION(event->comp);
	event->multi_comp = NULL;
}

/**
 * sync_set_event() - set or signal and specified event
 * @event:	Event to be set..
 *
 * set the @event, if there is an thread waiting for the event
 * it will be waken up, this function only wakes one thread.
 */

void sync_set_event(struct sync_object *event);

/**
 * sync_wait_on_event() - waits for a event to be set.
 * @event:	events to wait for it.
 * @timeout	timeout on waiting for the evetn.
 *
 * This functios will wait until @event is set or until timeout. In case of
 * success the function will return DSP_SOK and
 * in case of timeout the function will return DSP_ETIMEOUT
 */

static inline dsp_status sync_wait_on_event(struct sync_object *event,
							unsigned timeout)
{
	return wait_for_completion_timeout(&event->comp,
		msecs_to_jiffies(timeout)) ? DSP_SOK : DSP_ETIMEOUT;
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
 * @pu_index will store the index of the array element set and in case
 * of timeout the function will return DSP_ETIMEOUT.
 */

dsp_status sync_wait_on_multiple_events(struct sync_object **events,
				     unsigned count, unsigned timeout,
				     unsigned *index);

#endif /* _SYNC_H */
