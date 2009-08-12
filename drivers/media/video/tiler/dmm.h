/*
 * dmm.h
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

#ifndef DMM_H
#define DMM_H

extern int errno;

struct dmm_data {
	int pixfmt;
	unsigned short w;
	unsigned short h;
	short seczone;
	unsigned long ssptr;
	unsigned long tsptr;
	short syx;
};

struct dmmViewOrientT {
	unsigned char dmm90Rotate;
	unsigned char dmmXInvert;
	unsigned char dmmYInvert;
};

#define IOCSINIT _IOWR('z', 100, unsigned long)
#define IOCGALLOC _IOWR('z', 101, unsigned long)
#define IOCSFREE _IOWR('z', 102, unsigned long)
#define IOCGTSPTR _IOWR('z', 103, unsigned long)

int
dmm_tiler_buf_alloc(void *dmmInstanceCtxPtr,
		    unsigned short sizeWidth,
		    unsigned short sizeHeight,
		    int contMod,
		    void **allocedPtr,
		    void *custmPagesPtr);

int
dmm_tiler_buf_free(void *dmmInstanceCtxPtr,
		   void *allocedPtr,
		   int aliasViewPtr);

void *
dmm_tiler_translate_sysptr(void *dmmInstanceCtxPtr,
			   void *sysPtr,
			   struct dmmViewOrientT orient,
			   unsigned int validDataWidth,
			   unsigned int validDataHeight,
			   int aliasViewPtr,
			   int ptrToaliasView);

#endif
