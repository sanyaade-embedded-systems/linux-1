/*
 * timer16.c
 * Description:
 * Architecture specific stuff.
 *
 * Copyright (C) 2009, Texas Instruments, Incorporated
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 */

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/irq.h>
#include <asm/setup.h>
#include <asm/param.h>
#include <asm/mach-types.h>
#include <asm/io.h>
#include <asm/mach/map.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>

#define OK				0
#define ERROR				-1

#define AVALANCHE_TIMER0_BASE		0xd8610b00
extern unsigned int cpu_freq;

#define TIMER16_CNTRL_PRESCALE_ENABLE       0x8000
#define TIMER16_CNTRL_PRESCALE              0x003C
#define TIMER16_CNTRL_MODE                  0x0002

#define TIMER16_MINPRESCALE                 2
#define TIMER16_MAXPRESCALE                 8192
#define TIMER16_PRESCALE_DEFAULT       0x05
#define TIMER16_MIN_LOAD_VALUE              1
#define TIMER16_MAX_LOAD_VALUE              0xFFFF

#define MHZ                                 1000000

/* set min clock divisor to a little higher value
 * so that we are not close to the edge.
 * so multiply by factor 2
 */
#define TIMER16_MAX_CLK_DIVISOR (TIMER16_MAX_LOAD_VALUE * TIMER16_MAXPRESCALE)
#define TIMER16_MIN_CLK_DIVISOR (TIMER16_MIN_LOAD_VALUE * TIMER16_MINPRESCALE * 2)

typedef struct {
                        u32 ctrl_reg;   /* Timer Control Register */
                        u32 load_reg;   /* Timer Load value register */
                        u32 count_reg;  /* Timer count register */
                        u32 intr_reg;   /* Timer Interrupt register */

} puma_timer_regs_t;

typedef enum
{
    TIMER16_MODE_ONESHOT  = 0,
    TIMER16_MODE_AUTOLOAD = 2
} puma_timer_mode;

typedef enum
{
    TIMER16_STATUS_STOP = 0,
    TIMER16_STATUS_START
} puma_timer_status;

/****************************************************************************
 * Type:        PAL_SYS_TIMER16_STRUCT_T
 ****************************************************************************
 * Description: This type defines the hardware configuration of the timer
 *              
 ***************************************************************************/
typedef struct PAL_SYS_TIMER16_STRUCT_tag
{
    u32 ctrl_reg;
    u32 load_reg;
    u32 count_reg;
    u32 intr_reg;
} PAL_SYS_TIMER16_STRUCT_T;


/****************************************************************************
 * Type:        PAL_SYS_TIMER16_MODE_T
 ****************************************************************************
 * Description: This type defines different timer modes. 
 *              
 ***************************************************************************/
typedef enum PAL_SYS_TIMER16_MODE_tag
{
    TIMER16_CNTRL_ONESHOT  = 0,
    TIMER16_CNTRL_AUTOLOAD = 2
} PAL_SYS_TIMER16_MODE_T;



/****************************************************************************
 * Type:        PAL_SYS_TIMER16_CTRL_T
 ****************************************************************************
 * Description: This type defines start and stop values for the timer. 
 *              
 ***************************************************************************/
typedef enum PAL_SYS_TIMER16_CTRL_tag
{
    TIMER16_CTRL_STOP = 0,
    TIMER16_CTRL_START
} PAL_SYS_TIMER16_CTRL_T ;

/****************************************************************************
 * FUNCTION: PAL_sysTimer16GetFreqRange
 ****************************************************************************
 * Description: The routine is called to obtain the values of maximum and
 *              minimum possible timer values  that the timer module can
 *              support.
 ***************************************************************************/

void PAL_sysTimer16GetFreqRange
(
    u32  refclk_freq,
    u32 *p_max_usec,
    u32 *p_min_usec
)
{
    u32 refclk_mhz = (refclk_freq / MHZ);
    
    refclk_mhz = (refclk_freq / MHZ);
    /* Min usecs */
    if(p_min_usec)
    {
        if ( refclk_mhz < TIMER16_MIN_CLK_DIVISOR )
        {
            *p_min_usec = TIMER16_MIN_CLK_DIVISOR / refclk_mhz;
        }
        else
        {
            *p_min_usec = 1;
        }
    }        

    if(p_max_usec)
        *p_max_usec = (TIMER16_MAX_CLK_DIVISOR) / refclk_mhz ;
}


/****************************************************************************
 * FUNCTION: PAL_sysTimer16SetParams
 ****************************************************************************
 * Description: The routine is called to configure the timer mode and
 *              time period (in micro seconds).
 ***************************************************************************/
s32 PAL_sysTimer16SetParams
(
    u32                    base_address,
    u32                    refclk_freq,
    PAL_SYS_TIMER16_MODE_T    mode,
    u32                    usec
)
{
    volatile PAL_SYS_TIMER16_STRUCT_T *p_timer;
    u32    prescale;
    u32    count;
    u32    refclk_mhz = (refclk_freq / MHZ);

    if( (base_address == 0) || (usec == 0) )
        return ERROR;

    if ( (mode != TIMER16_CNTRL_ONESHOT) && (mode != TIMER16_CNTRL_AUTOLOAD) )
        return ERROR;
        
    /* The min time period is 1 usec and since the reference clock freq is always going 
       to be more than "min" divider value, minimum value is not checked.
       Check the max time period that can be derived from the timer in micro-seconds
    */
    if (usec > ( (TIMER16_MAX_CLK_DIVISOR) / refclk_mhz ))
    {
        return ERROR; /* input argument speed, out of range */
    }

    p_timer = (PAL_SYS_TIMER16_STRUCT_T *)(base_address);
    count = refclk_mhz * usec;

    for(prescale = 0; prescale < 12; prescale++)
    {
        count = count >> 1;
        if( count <= TIMER16_MAX_LOAD_VALUE )
        {
            break;
        }
    }

    /*write the load counter value*/
    p_timer->load_reg = count;

    /* write prescalar and mode to control reg */
    p_timer->ctrl_reg = mode | TIMER16_CNTRL_PRESCALE_ENABLE | (prescale << 2 );

    return OK;
}

/****************************************************************************
 * FUNCTION: PAL_sysTimer16Ctrl
 ****************************************************************************
 * Description: The routine is called to start/stop the timer
 *
 ***************************************************************************/
void PAL_sysTimer16Ctrl(u32 base_address, PAL_SYS_TIMER16_CTRL_T status)
{
    volatile PAL_SYS_TIMER16_STRUCT_T *p_timer;

    if( base_address )
    {
        p_timer = (PAL_SYS_TIMER16_STRUCT_T *)(base_address);

        if( status == TIMER16_CTRL_START )
        {
            p_timer->ctrl_reg |= TIMER16_CTRL_START;
        }
        else
        {
             p_timer->ctrl_reg &=   ~(TIMER16_CTRL_START);
        }
    }

}

static irqreturn_t
puma_timer0_interrupt(int irq, void *dev_id)
{
	write_seqlock(&xtime_lock);
	timer_tick();
	write_sequnlock(&xtime_lock);
	return IRQ_HANDLED;
}

static struct irqaction puma_timer0_irq = {
	.name = "Timer16",
        .flags          = IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
        .handler        = puma_timer0_interrupt
};

static void __init puma5_timer_init(void)
{
	int timer_clk;

	/* get the input clock frequency */
	timer_clk = 33300000;
	
	pr_info("Netra clockevent source: TIMER16 at %u Hz\n", timer_clk);
	puma_timer0_irq.dev_id = (void *)0;

	/* timer0 - 
	 * 	***Assuming its out or reset already.
	 * 	Enable timer and auto load, and go off every 1 ms
	 */
	PAL_sysTimer16SetParams(AVALANCHE_TIMER0_BASE, timer_clk, 
			TIMER16_CNTRL_AUTOLOAD, (int)((1.0/(float)(HZ)) * 1000000.0));
	PAL_sysTimer16Ctrl(AVALANCHE_TIMER0_BASE, TIMER16_CTRL_START);
	
	/* 
	 * TODO: Put correct interrupt number here. Also use correct irq
	 * registration call as required by latest kernel version.
	 */
	setup_irq(4, &puma_timer0_irq);

	printk("Timer0 initialized\n");
}

struct sys_timer timer16 = {
	.init   = puma5_timer_init,
};
