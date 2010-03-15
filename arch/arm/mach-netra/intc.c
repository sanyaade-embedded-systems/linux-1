/*
 * intc.c 
 * Description:
 * avalanche interrupt controller implementation
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
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/irq.h>

#include <asm/atomic.h>
#include <asm/io.h>

#if defined(CONFIG_ICRTL_DEBUG)
#define DEBUG(fmt,arg...)  printk(KERN_EMERG fmt , ##arg); 
#else 
#define DEBUG(fmt,arg...) 
#endif

#define ENTER DEBUG("[Ei %s-%d] \n", __FUNCTION__, __LINE__); 
#define EXIT  DEBUG("[Ex %s-%d] \n", __FUNCTION__, __LINE__);
#define INTERRUPT_PANIC(irq) \
        printk("whee, invalid irq_nr %d at %s line %d\n", \
                                (irq), __FILE__, __LINE__);\
        panic("IRQ, you lose...");

/* TODO: Check these as per HAPS54 configuration */
#define NUM_MAX_SYSTEM_INTS	32
#define NUM_HOST_INT_MAP_REGS	1
#define HOST_ARM_IRQ_NUM        ( 1 )  /* ARM nIRQ Number */
#define NUM_RAW_STATUS_REGS          ( 1 )  
#define NUM_ENABLED_STATUS_REGS      ( 1 )
#define NUM_ENABLE_SET_REGS          ( 1 )
#define NUM_ENABLE_CLEAR_REGS        ( 1 )  
#define NUM_POLARITY_REGS            ( 1 ) 
#define NUM_TYPE_REGS                ( 1 )
#define NUM_ENABLE_HINT_REGS         ( 1 ) 

#define INTC_SET_BIT_0                ( 0x00000001 )
#define INTC_CLEAR_BIT_0              ( 0xFFFFFFFE )

#define NUM_SYS_INTS_PER_REG         ( 4 )
#define NUM_CHANNELS_PER_REG         ( 4 )
#ifndef NUM_INTS_PER_REG
#define NUM_INTS_PER_REG             ( 32 )
#endif
#define INTC_MAX_NUMBER_PACERS       ( 16 )
#define INTC_NUM_MAX_CHANNEL         ( 1024 )

/****************************************************************************
 * Avalanche interrupt controller register base 
 ****************************************************************************/
#define AVALANCHE_ICTRL_REGS_BASE    AVALANCHE_INTC_BASE

/* NOTE: Read "ICTRL" as 'interrupt Controller' */
struct avalanche_ictrl_pacer_regs /* Avalanche Interrupt control pacer regs*/
{
    volatile unsigned long icpparamr; /* ICTRL Pacer Parameter Reg */
    volatile unsigned long icpdecr;   /* ICTRL Pacing Decrement Reg */
    volatile unsigned long icpmap;    /* ICTRL Pacer map Reg */
    volatile unsigned long reserved;  /* unused Reg */
};  
typedef struct avalanche_ictrl_pacer_regs ICTRL_PACER_REGS;

struct avalanche_ictrl_regs /* Avalanche Interrupt control registers */
{
   volatile unsigned long icrevr;      /* ICTRL Revision Reg 0x00*/
   volatile unsigned long iccntlr;     /* ICTRL Control Reg  0x04*/
   volatile unsigned long unused1;     /* 0x08 */
   volatile unsigned long ichcntlr;    /* ICTRL Host control Reg 0x0C */
   volatile unsigned long icglber;     /* ICTRL Global Enable Reg 0x10 */
   volatile unsigned long unused2[2];  /* 0x14  to 0x18*/
   volatile unsigned long icglbnlr;    /* ICTRL Global Nesting Level Reg 0x1C */      
   volatile unsigned long icsisr;      /* ICTRL Status index Set Reg 0x20 */
   volatile unsigned long icsicr;      /* ICTRL Status index Clear Reg 0x24 */
   volatile unsigned long iceisr;      /* ICTRL enable index Set Reg 0x28*/
   volatile unsigned long iceicr;      /* ICTRL enable  index Clear Reg 0x2C */
   volatile unsigned long icgwer;      /* ICTRL Global Wakeup Enable Reg 0x30 */
   volatile unsigned long ichinteisr;  /* ICTRL host interrupt enable index 
                                          Set  Reg 0x34 */
   volatile unsigned long ichinteicr;  /* ICTRL host interrupt enable index 
                                          Clear Reg 0x38 */
   volatile unsigned long unused4;     /* 0x3c */  
   volatile unsigned long icpprer;     /* ICTRL Pacer Prescale  Reg 0x40 */
   volatile unsigned long unused5[3];  /* 0x44  to 0x4C */  
   volatile unsigned long icvbr;       /* ICTRL Vector Base Reg 0x50 */
   volatile unsigned long icvszr;      /* ICTRL Vector Size Reg 0x54*/
   volatile unsigned long icvnullr;    /* ICTRL Vector Null Reg 0x58 */
   volatile unsigned long unused7[9];  /* 0x5c  to 0x7c */  
   volatile unsigned long icgpir;      /* ICTRL Global Priority Index Reg 0x80 */
   volatile unsigned long icgpvr;      /* ICTRL Global Priority vector Reg0x84 */
   volatile unsigned long unused17[2]; /* 0x88  to 0x8c */  
   volatile unsigned long icgsecinter; /* ICTRL Global security Interrupt 
                                          Enable Reg 0x90 */
   volatile unsigned long icsecpir;    /* ICTRL Secure prioritised Index Reg 0x94 */
   volatile unsigned long unused19[26];/* 0x098  to 0x0FC */  
   ICTRL_PACER_REGS  icpacer [16];        /* ICTRL pacing Regs array 0x100 */
   volatile unsigned long icrstar[32];    /* ICTRL Raw status  Reg 0x200 */
   volatile unsigned long icestar[32];    /* ICTRL Enabled Status Reg 0x280 */
   volatile unsigned long icenbsr[32];    /* ICTRL Enabler set Reg 0x300 */
   volatile unsigned long icenbcr[32];    /* ICTRL Enabler Clear Reg 0x380*/
   volatile unsigned long icchnlmpr[256]; /* ICTRL Channel Map Reg 0x400 */   
   volatile unsigned long ichmpr[64];     /* ICTRL Host Map Reg 0x800 */
   volatile unsigned long ichintpir[256]; /* ICTRL Host Interrupt
                                             Priotrized Index  Reg 0x900 */
   volatile unsigned long icpolr[32];     /* ICTRL polarity Reg 0xD00 */
   volatile unsigned long ictypr[32];     /* ICTRL type Reg 0xD80 */
   volatile unsigned long icwuper[64];    /* ICTRL Wakeup Enable Reg 0xE00 */
   volatile unsigned long icdbgsetr[64];  /* ICTRL Debug Set Reg 0xF00*/
   volatile unsigned long icsecer[64];    /* ICTRL secure Enble Reg 0x1000 */
   volatile unsigned long ichintnlr[256]; /* ICTRL Host Interrupt 
                                             Nesting level Reg 0x1100 */
   volatile unsigned long ichinter[8];    /* ICTRL Host Interrupt
                                              Enable Reg 0x1500 */
   volatile unsigned long unused45[184];  /* 0x1520  to 0x1800 */  
};


/*****************************************************************************
 *  General and Global Regsiters APIs
 ****************************************************************************/
/* Revision Register */
static inline unsigned int avalanche_intc_read_revision_register( void );

/* Control Register */
#define INTC_CNRTL_WAKEUP_MODE          ( 0x00000002 )
#define INTC_CNRTL_NORMAL_MODE          ( 0x00000000 )
#define INTC_CNRTL_NO_NESTING           ( 0x00000000 )
#define INTC_CNRTL_MANUAL_NESTING       ( 0x0000000C )
#define INTC_CNRTL_AUTO_GLB_NESTING     ( 0x00000008 )
#define INTC_CNRTL_AUTO_INDL_NESTING    ( 0x00000004 )

static unsigned int avalanche_intc_get_cntrl_mode( void ); 

/* Global Prioritized Index Register  */
#define INTC_PRIORITY_INT_FLAG         ( 0x1FF )


/*****************************************************************************
 *  Index Register APIs
 ****************************************************************************/
/* Interrupt Status Index Register */
static int avalanche_intc_clear_status( unsigned int irq );
 
/****************************************************************************
 * Interrupt Mapping APIs
 ****************************************************************************/
/* Channel Map Registers */
static int avalanche_intc_map_system_int_to_channel( unsigned int sys_irq, 
                                                     unsigned int channel_num);
/* Host  Interrupt  Map Registers */
static int avalanche_intc_map_channel_to_host_int( unsigned int channel_num,
                                                   unsigned int host_irq);

/*****************************************************************************
 * Global Data types  
 ****************************************************************************/
//extern irq_desc_t irq_desc [NR_IRQS];
struct avalanche_ictrl_regs  *avalanche_hw0_icregs;  
extern asmlinkage void armIRQ(void);

/*****************************************************************************
 * Utility Functions
 ****************************************************************************/
inline unsigned char  get_bit_position ( unsigned int irq )
{
    return((unsigned char)( irq % NUM_INTS_PER_REG ));
}

inline unsigned char  get_reg_index( unsigned int irq )
{
    return((unsigned char )( irq / NUM_INTS_PER_REG ));
}
/*****************************************************************************
 * Function Definitions
 ****************************************************************************/
static inline unsigned int avalanche_intc_read_revision_register( void )
{
    return(avalanche_hw0_icregs->icrevr);
}


inline static unsigned int avalanche_intc_get_cntrl_mode( void ) 
{
    return((unsigned  int)avalanche_hw0_icregs->iccntlr);
}

static int avalanche_intc_clear_status( unsigned int irq )
{
    avalanche_hw0_icregs->icsicr = irq;
    return(0);	
}

/* Channel Map Registers */
static int avalanche_intc_map_system_int_to_channel( unsigned int sys_irq,
                                                     unsigned int channel_num )
{
    unsigned int chnl_reg_indx = sys_irq/NUM_SYS_INTS_PER_REG;
    /* There is one register per 4 system Interrupts, so get the proper 
     * register base for the given interrupt.
     */
    avalanche_hw0_icregs->icchnlmpr[chnl_reg_indx] |= 
    (channel_num << ((sys_irq % NUM_SYS_INTS_PER_REG) * 8));
    return(0);
}

/* Host  Interrupt  Map Registers */
static int avalanche_intc_map_channel_to_host_int( unsigned int channel_num,
                                                   unsigned int host_irq )
{
    unsigned int hst_int_reg_indx = channel_num /NUM_CHANNELS_PER_REG;
    /* There is one register per 4 channels so get the proper 
     * register base for the given channel.
     */
    avalanche_hw0_icregs->ichmpr[hst_int_reg_indx] |= 
    (host_irq << ((channel_num % NUM_CHANNELS_PER_REG)* 8 ));
    return(0);
}

/* low level INTC type set */
int avalanche_intc_set_interrupt_type( unsigned int irq,
                                       unsigned char  int_type)
{
    unsigned int int_reg_indx = get_reg_index( irq );
	if (int_type)
    {
       avalanche_hw0_icregs->ictypr[int_reg_indx] |= 
           (1 << get_bit_position( irq ));
    }
    else
    {
       avalanche_hw0_icregs->ictypr[int_reg_indx] &=
           ~(1 << get_bit_position( irq ));
	}
    return(0);
}

/* low level INTC pol set */
int avalanche_intc_set_interrupt_polarity( unsigned int irq, 
                                           unsigned char int_polarity)
{
    unsigned int int_reg_indx =  get_reg_index( irq );
    if(int_polarity)
	{
		avalanche_hw0_icregs->icpolr[int_reg_indx] |= 
						(1 << get_bit_position( irq ));
	}
	else
	{
		avalanche_hw0_icregs->icpolr[int_reg_indx] &=
						~(1 << get_bit_position( irq ));
	}
    return(0);
}
	
/* Linux irq_chip framework hook for set type */
/* check include/linux/irq.h for type flags */
int avalanche_intc_set_type(unsigned int irq, unsigned int flow_type)
{
	/* identify interrupt type and set the same */
	if((flow_type & IRQ_TYPE_EDGE_RISING) || 
			(flow_type & IRQ_TYPE_EDGE_FALLING) ||
				(flow_type & IRQ_TYPE_EDGE_BOTH))
	{
		avalanche_intc_set_interrupt_type(irq, 1);
		set_irq_handler(irq, handle_edge_irq);
	}
	else
	{
		avalanche_intc_set_interrupt_type(irq, 0);
		set_irq_handler(irq, handle_level_irq);
	}

	/* identify polarity and set the same */
	if((flow_type & IRQ_TYPE_EDGE_FALLING) ||
			(flow_type & IRQ_TYPE_LEVEL_LOW))
	{
		avalanche_intc_set_interrupt_polarity(irq, 0);	
	}
	else
	{
		avalanche_intc_set_interrupt_polarity(irq, 1);
	}
	return(0);
}

static inline void avalanche_intc_enable_irq( unsigned int irq )
{
	/*
	* !!HACK!!
	* Clear interrupt pending status before enabling interurpts - needed to
	* avoid spurious interrupts observed on HAPS platform.
	* __IMPORTANT__: This may have a side effect when an interrupt is missed
	* which might occur just after the handler has exited - unless of course
	* the interrupt status from device remains active.
	*/
	avalanche_intc_clear_status(irq);

	avalanche_hw0_icregs->iceisr = irq;
}

static inline void avalanche_intc_disable_irq( unsigned int irq )
{   
    avalanche_hw0_icregs->ichinteicr = HOST_ARM_IRQ_NUM;
    avalanche_hw0_icregs->iceicr = irq;
    avalanche_hw0_icregs->ichinteisr = HOST_ARM_IRQ_NUM;
}

static unsigned int avalanche_intc_startup_irq( unsigned int irq )
{  
	/* clear already latched interrupts */
	avalanche_intc_clear_status(irq);
 
    /* Enable IRQ */ 
    avalanche_hw0_icregs->iceisr = irq;
    return(0); 
}

static void avalanche_intc_end_irq( unsigned int irq ) 
{   
    //FIXME: Uncommenting the line below gives compilation errors
    //if (! ( irq_desc[ irq ].status & ( IRQ_DISABLED|IRQ_INPROGRESS ) ) )
	avalanche_hw0_icregs->iceisr = irq; /* enable IRQ */
}

static void avalanche_intc_ack_irq( unsigned int irq )
{   
    /* clear status */
	avalanche_intc_clear_status(irq);
}


#define startup_avalanche_primary_irq	      avalanche_intc_startup_irq
#define shutdown_avalanche_primary_irq        avalanche_intc_disable_irq
#define enable_avalanche_primary_irq          avalanche_intc_enable_irq
#define disable_avalanche_primary_irq         avalanche_intc_disable_irq
#define ack_avalanche_primary_irq             avalanche_intc_ack_irq
#define mask_avalanche_primary_irq  	      avalanche_intc_disable_irq
#define unmask_avalanche_primary_irq  	      avalanche_intc_enable_irq
#define end_avalanche_primary_irq             avalanche_intc_end_irq

static struct irq_chip avalanche_primary_irq_type = {
    .name       = "[INTC Primary]",
    .startup    = startup_avalanche_primary_irq,
    .shutdown   = shutdown_avalanche_primary_irq,
    .unmask	= enable_avalanche_primary_irq,
    .mask	= disable_avalanche_primary_irq,
    .ack	= ack_avalanche_primary_irq,
    .end	= end_avalanche_primary_irq,
    .set_type	= avalanche_intc_set_type,
};

/* Initializes the avalanche interrupt controller hardware to a sane
 * state.
 */
static void avalanche_init_intc_hw( void )
{
    unsigned int i,cntrl_ver;
    avalanche_hw0_icregs = 
    (struct avalanche_ictrl_regs *)NETRA_INTC_VIRT;

    /* read revision */
    cntrl_ver = *(volatile unsigned int*)(NETRA_INTC_VIRT);
    printk("Interrupt controller revision : %x\n", cntrl_ver);    

    /* Globally disable all hosts ints */
    avalanche_hw0_icregs->icglber = 0;

    /* disable host interrupts */
    for(i=0; i < NUM_ENABLE_HINT_REGS; i++ )
    {
        avalanche_hw0_icregs->ichinter[i] = 0x0;
    }
    /* disable system interrupts */
    for(i=0; i < NUM_ENABLE_CLEAR_REGS; i++ )
    {
        avalanche_hw0_icregs->icenbcr[i] = 0xFFFFFFFF;
    }
 
    /* set all s/m interrupt polarity to active high*/
    for(i=0; i < NUM_POLARITY_REGS; i++ )
    {
        avalanche_hw0_icregs->icpolr[i] = 0xFFFFFFFF;
    }
	/* set all s/m interrupt type to level*/ 
    for(i=0; i < NUM_TYPE_REGS; i++ )
    {
        avalanche_hw0_icregs->ictypr[i] =0x0;
    }

    /* disable nesting and wakeup mode */
    avalanche_hw0_icregs->iccntlr =( INTC_CNRTL_NO_NESTING |
                                     INTC_CNRTL_NORMAL_MODE ) ;

    /* disable vic */
    avalanche_hw0_icregs->ichcntlr = 0;

	/* enable only timer interrupt */
    avalanche_hw0_icregs->icenbsr[0] = 0x10;

    /* clear system interrupts */
    for(i=0; i < NUM_RAW_STATUS_REGS; i++ )
    {
        avalanche_hw0_icregs->icestar[i] = 0xFFFFFFFF; 
    }
}

void __init arch_init_irq( void )
{
    int irq,chnl_num;
    unsigned int sys_to_chan_map[NUM_MAX_SYSTEM_INTS];
//    ENTER
    avalanche_init_intc_hw();
    printk("The arch intc base is %x \n", *avalanche_hw0_icregs); 	
    /* Assuming INTC is already out of reset */
 
    /* extern unsigned int sys_to_chan_map[NUM_MAX_SYSTEM_INTS];
     * Initilize the system int to channel map arry 
     * This should be done outside this file 
     * Currently all the int will be mapped to channel 0
     */
    memset(sys_to_chan_map, 0, (NUM_MAX_SYSTEM_INTS * sizeof(unsigned int))); 
 
    /* map all system interrupts to channel 0 */
    for( irq = 0; irq < NUM_MAX_SYSTEM_INTS; irq++ )
    {
        avalanche_intc_map_system_int_to_channel(irq, sys_to_chan_map[irq]);
    }

    /* As VIC is disabled, All the channel will map to to either nFIQ or nIRQ.
     * ARM11 have only one IRQ line,so map all the channels to this single host 
     * ARM11 IRQ line.
     */
    for( chnl_num = 0; chnl_num < (NUM_HOST_INT_MAP_REGS * NUM_CHANNELS_PER_REG); chnl_num++ )
    {
        avalanche_intc_map_channel_to_host_int(chnl_num, HOST_ARM_IRQ_NUM);
    }

    /* enable host interrupt */   
    avalanche_hw0_icregs->ichinteisr = HOST_ARM_IRQ_NUM;

    /* setup the IRQ description array. */
    for (irq = 0; irq < NUM_MAX_SYSTEM_INTS; irq++)
    {
       	    set_irq_chip(irq, &avalanche_primary_irq_type);
	    set_irq_flags(irq, IRQF_VALID /* TODO: Check if needed --> | IRQF_PROBE */);
	    set_irq_handler(irq, handle_level_irq);
    }
    /* set global enable */
    avalanche_hw0_icregs->icglber = INTC_SET_BIT_0;
//    EXIT	
}
