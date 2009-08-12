/*
 * dmm_2d_alloc.c
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

#include <linux/module.h>
#include <linux/vmalloc.h>
#include "dmm_def.h"
#include "dmm_2d_alloc.h"
#include "dmm_prv.h"

/* ========================================================================== */
/**
 *  overlapping_test()
 *
 * @brief  Performs an area overlap test for errors. Debug only.
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @return int: 1 if overlapping is detected.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
void overlapping_test(struct dmmTILERContCtxT *dmmTilerCtx)
{
	struct dmmTILERContPageLstT *itr1 = dmmTilerCtx->usdArList;

	while (itr1 != NULL) {
		struct dmmTILERContPageAreaT *chkPage = &(itr1->pgAr);
		struct dmmTILERContPageLstT *itr2 = dmmTilerCtx->usdArList;

		while (itr2 != NULL) {
			if (chkPage != &(itr2->pgAr)) {
				if ((itr2->pgAr.x0 <= chkPage->x1 &&
						itr2->pgAr.x1 >= chkPage->x0) &&
						(itr2->pgAr.y0 <= chkPage->y1 &&
						 itr2->pgAr.y1 >= chkPage->y0)){
					printk(KERN_ERR"overlap test fail\n");
				}
			}
			itr2 = itr2->pgArNext;
		}
		itr1 = itr1->pgArNext;
	}
	return;
}

/* ========================================================================== */
/**
 *  point_free_test()
 *
 * @brief  Tests if a page on a given coordinate is unoccupied.
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param areaHit - dmmTILERContPageAreaT** - [out] Returns a pointer to the
 *  allocated area that was hit by the specified coordinate.
 *
 * @return int: 1 if the selected coordinate page is unoccupied.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline int point_free_test(struct dmmTILERContCtxT *tlrCtx,
			 signed long X,
			 signed long Y,
			 struct dmmTILERContPageAreaT **areaHit)
{
	struct dmmTILERContPageLstT *arUsd = tlrCtx->usdArList;
	*areaHit = NULL;
	while (arUsd != NULL) {
		if (arUsd->pgAr.x0 <= X && arUsd->pgAr.x1 >= X &&
				arUsd->pgAr.y0 <= Y && arUsd->pgAr.y1 >= Y) {
			*areaHit = &(arUsd->pgAr);
			return 0;
		}
		arUsd = arUsd->pgArNext;
	}
	return 1;
}

/* ========================================================================== */
/**
 *  point_area_hit()
 *
 * @brief  Tests if a given coordinate is within a given area.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param areaHit - dmmTILERContPageAreaT* - [in] Given area to check if
 * contains the given coordinate point.
 *
 * @return int: 1 if the given coordinate is contained in the
 * given area.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline int point_area_hit(signed long X,
			signed long Y,
			struct dmmTILERContPageAreaT *area)
{
	if ((area->x0 <= X && area->x1 >= X) &&
			(area->y0 <= Y && area->y1 >= Y))
		return 1;
	return 0;
}

/* ========================================================================== */
/**
 *  zone_area_overlap()
 *
 * @brief  Tests if two areas overlap.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param SizeX - signed long - [in] Page width.
 *
 * @param SizeY - signed long - [in] Page height.
 *
 * @param areaHit - dmmTILERContPageAreaT* - [in] Given area to check.
 *
 * @return int: 1 if the given areas overlap.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline int zone_area_overlap(signed long X,
			   signed long Y,
			   signed long SizeX,
			   signed long SizeY,
			   struct dmmTILERContPageAreaT *area)
{
	if ((area->x0 <= X + SizeX && area->x1 >= X) &&
			(area->y0 <= Y + SizeY && area->y1 >= Y)) {
		return 1;
	}
	return 0;
}

/* ========================================================================== */
/**
 *  line_area_hit()
 *
 * @brief  Tests if a page line overlaps with an area. Either width or height
 * can be used to perform checks in both coordinates.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param SizeX - signed long - [in] Page width.
 *
 * @param SizeY - signed long - [in] Page height.
 *
 * @param areaHit - dmmTILERContPageAreaT* - [in] Given area to check.
 *
 * @return int: 1 if the given areas overlap.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline int line_area_hit(signed long X,
		       signed long Y,
		       signed long SizeX,
		       signed long SizeY,
		       struct dmmTILERContPageAreaT *area)
{
	if (((area->x0 <= X && area->x1 >= X) ||
			(area->x0 <= X + SizeX && area->x1 >= X + SizeX)) &&
			((area->y0 <= Y && area->y1 >= Y) ||
			 (area->y0 <= Y + SizeY && area->y1 >= Y + SizeY))) {
		return 1;
	}

	return 0;
}

/* ========================================================================== */
/**
 *  expand_right()
 *
 * @brief  Finds all of the unocupied pages ("expansion") along the specified
 * coordinate line (1D check).
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param areaHit - dmmTILERContPageAreaT** - [out] Returns a pointer to the
 * allocated area that was hit during the "expansion" along the specified
 * coordinate line.
 *
 * @return signed long the calculated coordinate point.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline signed long expand_right(struct dmmTILERContCtxT *tlrCtx,
			       signed long X,
			       signed long Y,
			       struct dmmTILERContPageAreaT **areaHit)
{
	areaHit = NULL;
	while (X < tlrCtx->contSizeX) {
		if (0 == point_free_test(tlrCtx, X, Y, areaHit))
			return X - 1;
		X++;
	}

	return X - 1;
}

/* ========================================================================== */
/**
 *  expand_left()
 *
 * @brief  Finds all of the unocupied pages ("expansion") along the specified
 * coordinate line (1D check).
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param areaHit - dmmTILERContPageAreaT** - [out] Returns a pointer to the
 * allocated area that was hit during the "expansion" along the specified
 * coordinate line.
 *
 * @return signed long the calculated coordinate point.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline signed long expand_left(struct dmmTILERContCtxT *tlrCtx,
			      signed long X,
			      signed long Y,
			      struct dmmTILERContPageAreaT **areaHit)
{
	areaHit = NULL;
	while (X >= 0) {
		if (0 == point_free_test(tlrCtx, X, Y, areaHit))
			return X + 1;
		X--;
	}

	return X + 1;
}

/* ========================================================================== */
/**
 *  expand_bottom()
 *
 * @brief  Finds all of the unocupied pages ("expansion") along the specified
 * coordinate line (1D check).
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param areaHit - dmmTILERContPageAreaT** - [out] Returns a pointer to the
 *  allocated area that was hit during the "expansion" along the specified
 *  coordinate line.
 *
 * @return signed long the calculated coordinate point.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline signed long expand_bottom(struct dmmTILERContCtxT *tlrCtx,
				signed long X,
				signed long Y,
				struct dmmTILERContPageAreaT **areaHit)
{
	areaHit = NULL;
	while (Y < tlrCtx->contSizeY) {
		if (0 == point_free_test(tlrCtx, X, Y, areaHit))
			return Y - 1;
		Y++;
	}

	return Y - 1;
}

/* ========================================================================== */
/**
 *  expand_top()
 *
 * @brief  Finds all of the unocupied pages ("expansion") along the specified
 * coordinate line (1D check).
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param areaHit - dmmTILERContPageAreaT** - [out] Returns a pointer to the
 *  allocated area that was hit during the "expansion" along the specified
 *  coordinate line.
 *
 * @return signed long the calculated coordinate point.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline signed long expand_top(struct dmmTILERContCtxT *tlrCtx,
			     signed long X,
			     signed long Y,
			     struct dmmTILERContPageAreaT **areaHit)
{
	areaHit = NULL;
	while (Y >= 0) {
		if (0 == point_free_test(tlrCtx, X, Y, areaHit))
			return Y + 1;
		Y--;
	}

	return Y + 1;
}

/* ========================================================================== */
/**
 *  expand_line_on_bottom_to_right()
 *
 * @brief  Finds all of the unocupied pages ("expansion") along the specified
 * coordinate line area (2D check).
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param elSizeY - signed long - [in] Size of the area to check.
 *
 * @return signed long the calculated coordinate point.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline signed long expand_line_on_bottom_to_right(
		struct dmmTILERContCtxT *tlrCtx,
		signed long X,
		signed long Y,
		signed long elSizeY)
{
	struct dmmTILERContPageLstT *iter;
	int elSizeX = 0;

	while (elSizeX < tlrCtx->contSizeX) {
		iter = tlrCtx->usdArList;
		while (iter != NULL) {
			if (1 == zone_area_overlap(X, Y, elSizeX, elSizeY,
						 &(iter->pgAr)))
				return X + elSizeX - 1;
			iter = iter->pgArNext;
		}
		elSizeX++;
	}

	return tlrCtx->contSizeX - 1;
}

/* ========================================================================== */
/**
 *  expand_line_on_bottom_to_left()
 *
 * @brief  Finds all of the unocupied pages ("expansion") along the specified
 * coordinate line area (2D check).
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param elSizeY - signed long - [in] Size of the area to check.
 *
 * @return signed long the calculated coordinate point.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline signed long expand_line_on_bottom_to_left(
		struct dmmTILERContCtxT *tlrCtx,
		signed long X,
		signed long Y,
		signed long elSizeY)
{
	struct dmmTILERContPageLstT *iter;
	int elSizeX = 0;

	while (X >= 0) {
		iter = tlrCtx->usdArList;
		while (iter != NULL) {
			if (1 == zone_area_overlap(X - elSizeX, Y,
						 elSizeX, elSizeY,
						 &(iter->pgAr)))
				return X + 1;
			iter = iter->pgArNext;
		}
		X--;
		elSizeX++;
	}

	return 0;
}

/* ========================================================================== */
/**
 *  expand_line_on_top_to_right()
 *
 * @brief  Finds all of the unocupied pages ("expansion") along the specified
 * coordinate line area (2D check).
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param elSizeY - signed long - [in] Size of the area to check.
 *
 * @return signed long the calculated coordinate point.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline signed long expand_line_on_top_to_right(struct dmmTILERContCtxT *tlrCtx,
		signed long X,
		signed long Y,
		signed long elSizeY)
{
	struct dmmTILERContPageLstT *iter;
	int elSizeX = 0;

	while (elSizeX < tlrCtx->contSizeX) {
		iter = tlrCtx->usdArList;
		while (iter != NULL) {
			if (1 == zone_area_overlap(X, Y, elSizeX, elSizeY,
						 &(iter->pgAr)))
				return X + elSizeX - 1;
			iter = iter->pgArNext;
		}
		elSizeX++;
	}

	return tlrCtx->contSizeX - 1;
}

/* ========================================================================== */
/**
 *  expand_line_on_top_to_left()
 *
 * @brief  Finds all of the unocupied pages ("expansion") along the specified
 * coordinate line area (2D check).
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param elSizeY - signed long - [in] Size of the area to check.
 *
 * @return signed long the calculated coordinate point.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline signed long expand_line_on_top_to_left(struct dmmTILERContCtxT *tlrCtx,
		signed long X,
		signed long Y,
		signed long elSizeY)
{
	struct dmmTILERContPageLstT *iter;
	int elSizeX = 0;

	while (X >= 0) {
		iter = tlrCtx->usdArList;
		while (iter != NULL) {
			if (1 == zone_area_overlap(X - elSizeX, Y - elSizeY,
					elSizeX, elSizeY, &(iter->pgAr)))
				return X + 1;
			iter = iter->pgArNext;
		}
		X--;
		elSizeX++;
	}

	return 0;
}

/* ========================================================================== */
/**
 *  expand_line_on_right_to_bottom()
 *
 * @brief  Finds all of the unocupied pages ("expansion") along the specified
 * coordinate line area (2D check).
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param elSizeX - signed long - [in] Size of the area to check.
 *
 * @return signed long the calculated coordinate point.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline signed long expand_line_on_right_to_bottom(
		struct dmmTILERContCtxT *tlrCtx,
		signed long X,
		signed long Y,
		signed long elSizeX)
{
	struct dmmTILERContPageLstT *iter;
	int elSizeY = 0;

	while (elSizeY < tlrCtx->contSizeY) {
		iter = tlrCtx->usdArList;
		while (iter != NULL) {
			if (1 == zone_area_overlap(X, Y, elSizeX, elSizeY,
						 &(iter->pgAr)))
				return Y + elSizeY - 1;
			iter = iter->pgArNext;
		}
		elSizeY++;
	}

	return tlrCtx->contSizeY - 1;
}

/* ========================================================================== */
/**
 *  expand_line_on_right_to_top()
 *
 * @brief  Finds all of the unocupied pages ("expansion") along the specified
 * coordinate line area (2D check).
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param elSizeX - signed long - [in] Size of the area to check.
 *
 * @return signed long the calculated coordinate point.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline signed long expand_line_on_right_to_top(struct dmmTILERContCtxT *tlrCtx,
		signed long X,
		signed long Y,
		signed long elSizeX)
{
	struct dmmTILERContPageLstT *iter;
	int elSizeY = 0;

	while (Y >= 0) {
		iter = tlrCtx->usdArList;
		while (iter != NULL) {
			if (1 == zone_area_overlap(X, Y - elSizeY, elSizeX,
						 elSizeY, &(iter->pgAr)))
				return Y + 1;
			iter = iter->pgArNext;
		}
		Y--;
		elSizeY++;
	}

	return 0;
}

/* ========================================================================== */
/**
 *  expand_line_on_left_to_bottom()
 *
 * @brief  Finds all of the unocupied pages ("expansion") along the specified
 * coordinate line area (2D check).
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param elSizeX - signed long - [in] Size of the area to check.
 *
 * @return signed long the calculated coordinate point.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline signed long expand_line_on_left_to_bottom(
		struct dmmTILERContCtxT *tlrCtx,
		signed long X,
		signed long Y,
		signed long elSizeX)
{
	struct dmmTILERContPageLstT *iter;
	int elSizeY = 0;

	while (elSizeY < tlrCtx->contSizeY) {
		iter = tlrCtx->usdArList;
		while (iter != NULL) {
			if (1 == zone_area_overlap(X, Y, elSizeX, elSizeY,
						 &(iter->pgAr)))
				return Y + elSizeY - 1;
			iter = iter->pgArNext;
		}
		elSizeY++;
	}

	return tlrCtx->contSizeY - 1;
}

/* ========================================================================== */
/**
 *  expand_line_on_left_to_top()
 *
 * @brief  Finds all of the unocupied pages ("expansion") along the specified
 * coordinate line area (2D check).
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X page coordinate.
 *
 * @param Y - signed long - [in] Y page coordinate.
 *
 * @param elSizeX - signed long - [in] Size of the area to check.
 *
 * @return signed long the calculated coordinate point.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
inline signed long expand_line_on_left_to_top(struct dmmTILERContCtxT *tlrCtx,
		signed long X,
		signed long Y,
		signed long elSizeX)
{
	struct dmmTILERContPageLstT *iter;
	int elSizeY = 0;

	while (Y >= 0) {
		iter = tlrCtx->usdArList;
		while (iter != NULL) {
			if (1 == zone_area_overlap(X - elSizeX, Y - elSizeY,
					elSizeX, elSizeY, &(iter->pgAr))) {
				return Y + 1;
			}
			iter = iter->pgArNext;
		}
		Y--;
		elSizeY++;
	}

	return 0;
}

/* ========================================================================== */
/**
 *  area_required_to_allocated()
 *
 * @brief  Given the required area to be allcoted, total area present and anchor
 * points along a side of attachemnt, calculates the actual position of the new
 * new 2D area that is allocated.
 *
 * @param areaReq - dmmTILERContPageAreaT* - [in] Required 2D area to be
 *  allocated.
 *
 * @param areaTotal - dmmTILERContPageAreaT* - [in] Total 2D area allocated for
 * this area attachment.
 *
 * @param areaAlloc - dmmTILERContPageAreaT* - [out] The actual allocated area.
 *
 * @param anchorX - signed long - [in] X page coordinate anchor point.
 *
 * @param anchorY - signed long - [in] Y page coordinate anchor point.
 *
 * @param side - SideAffinity - [in] Side of attachment.
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
void area_required_to_allocated(struct dmmTILERContPageAreaT *areaReq,
			     struct dmmTILERContPageAreaT *areaTotal,
			     struct dmmTILERContPageAreaT *areaAlloc,
			     signed long anchorX,
			     signed long anchorY,
			     enum SideAffinity side)
{
	switch (side) {
	case PSA_BOTTOM:
		if (areaTotal->x1 - anchorX < areaReq->x1) {
			if (areaTotal->x1 - areaTotal->x0 >= areaReq->x1) {
				areaAlloc->x0 = (unsigned short)(areaTotal->x1 -
								 areaReq->x1);
				areaAlloc->x1 = (unsigned short)(areaAlloc->x0 +
								 areaReq->x1);
			} else {
				areaAlloc->x0 = areaTotal->x0;
				areaAlloc->x1 = areaTotal->x1;
			}
		} else {
			areaAlloc->x0 = (unsigned short)(anchorX);
			areaAlloc->x1 = (unsigned short)(anchorX + areaReq->x1);
		}

		areaAlloc->y0 = (unsigned short)(anchorY);
		if (areaAlloc->y0 + areaReq->y1 <= areaTotal->y1)
			areaAlloc->y1 = (unsigned short)(areaAlloc->y0 +
							 areaReq->y1);
		else
			areaAlloc->y1 = areaTotal->y1;
		areaAlloc->fitToSide = PSA_BOTTOM;
		break;

	case PSA_TOP:
		if (areaTotal->x1 - anchorX < areaReq->x1) {
			if (areaTotal->x1 - areaTotal->x0 >= areaReq->x1) {
				areaAlloc->x0 = (unsigned short)(areaTotal->x1 -
								 areaReq->x1);
				areaAlloc->x1 = (unsigned short)(areaAlloc->x0 +
								 areaReq->x1);
			} else {
				areaAlloc->x0 = areaTotal->x0;
				areaAlloc->x1 = areaTotal->x1;
			}
		} else {
			areaAlloc->x0 = (unsigned short)(anchorX);
			areaAlloc->x1 = (unsigned short)(anchorX + areaReq->x1);
		}

		areaAlloc->y1 = (unsigned short)(anchorY);
		if (areaAlloc->y1 - areaReq->y1 >= areaTotal->y0) {
			areaAlloc->y0 = (unsigned short)(areaAlloc->y1 -
							 areaReq->y1);
		} else {
			areaAlloc->y0 = areaTotal->y0;
		}
		areaAlloc->fitToSide = PSA_TOP;
		break;

	case PSA_RIGHT:
		if (areaTotal->y1 - anchorY < areaReq->y1) {
			if (areaTotal->y1 - areaTotal->y0 >= areaReq->y1) {
				areaAlloc->y0 = (unsigned short)(areaTotal->y1 -
								 areaReq->y1);
				areaAlloc->y1 = (unsigned short)(areaAlloc->y0 +
								 areaReq->y1);
			} else {
				areaAlloc->y0 = areaTotal->y0;
				areaAlloc->y1 = areaTotal->y1;
			}
		} else {
			areaAlloc->y0 = (unsigned short)(anchorY);
			areaAlloc->y1 = (unsigned short)(anchorY + areaReq->y1);
		}

		areaAlloc->x0 = (unsigned short)(anchorX);
		if (areaAlloc->x0 + areaReq->x1 <= areaTotal->x1) {
			areaAlloc->x1 = (unsigned short)(areaAlloc->x0 +
							 areaReq->x1);
		} else {
			areaAlloc->x1 = areaTotal->x1;
		}
		areaAlloc->fitToSide = PSA_RIGHT;
		break;

	case PSA_LEFT:
		if (areaTotal->y1 - anchorY < areaReq->y1) {
			if (areaTotal->y1 - areaTotal->y0 >= areaReq->y1) {
				areaAlloc->y0 = (unsigned short)(areaTotal->y1 -
								 areaReq->y1);
				areaAlloc->y1 = (unsigned short)(areaAlloc->y0 +
								 areaReq->y1);
			} else {
				areaAlloc->y0 = areaTotal->y0;
				areaAlloc->y1 = areaTotal->y1;
			}
		} else {
			areaAlloc->y0 = (unsigned short)(anchorY);
			areaAlloc->y1 = (unsigned short)(anchorY + areaReq->y1);
		}

		areaAlloc->x1 = (unsigned short)(anchorX);
		if (areaAlloc->x1 - areaReq->x1 >= areaTotal->x0) {
			areaAlloc->x0 = (unsigned short)(areaAlloc->x1 -
							 areaReq->x1);
		} else {
			areaAlloc->x0 = areaTotal->x0;
		}
		areaAlloc->fitToSide = PSA_LEFT;
		break;
	}
}

/* ========================================================================== */
/**
 *  area_check_for_fit()
 *
 * @brief  Checks if a specified free area is big enough for the required
 * allocation, and if the specified free area is the best (smallest) choice
 * of all discovered free areas.
 *
 * @param tmpArea - dmmTILERContPageAreaSpecT* - [in] A free area that has
 * been discovered and needs to be checked if it is big and small enough
 * for the required allocation area.
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param areaReq - dmmTILERContPageAreaT* - [in] Required allcoation area.
 *
 * @return int: True if the specified area is the new best fit.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
int area_check_for_fit(struct dmmTILERContPageAreaSpecT *tmpArea,
		    struct dmmTILERContCtxT *tlrCtx,
		    struct dmmTILERContPageAreaT *areaReq)
{
	if ((tmpArea->ttlExpndAr.x1 - tmpArea->ttlExpndAr.x0) >= areaReq->x1 &&
			(tmpArea->ttlExpndAr.y1 - tmpArea->ttlExpndAr.y0) >=
			areaReq->y1) {
		if ((tmpArea->ttlExpndAr.x1 - tmpArea->ttlExpndAr.x0) *
				(tmpArea->ttlExpndAr.y1 -
				tmpArea->ttlExpndAr.y0) <
				(tlrCtx->tmpArSelect.ttlExpndAr.x1 -
				 tlrCtx->tmpArSelect.ttlExpndAr.x0) *
				(tlrCtx->tmpArSelect.ttlExpndAr.y1 -
				 tlrCtx->tmpArSelect.ttlExpndAr.y0)) {

			tlrCtx->tmpArSelect.ttlExpndAr.x0 =
				tmpArea->ttlExpndAr.x0;
			tlrCtx->tmpArSelect.ttlExpndAr.y0 =
				tmpArea->ttlExpndAr.y0;
			tlrCtx->tmpArSelect.ttlExpndAr.x1 =
				tmpArea->ttlExpndAr.x1;
			tlrCtx->tmpArSelect.ttlExpndAr.y1 =
				tmpArea->ttlExpndAr.y1;
			tlrCtx->tmpArSelect.ttlExpndAr.fitToSide =
				tmpArea->ttlExpndAr.fitToSide;

			tlrCtx->tmpArSelect.plmntAr.x0 = tmpArea->plmntAr.x0;
			tlrCtx->tmpArSelect.plmntAr.y0 = tmpArea->plmntAr.y0;
			tlrCtx->tmpArSelect.plmntAr.x1 = tmpArea->plmntAr.x1;
			tlrCtx->tmpArSelect.plmntAr.y1 = tmpArea->plmntAr.y1;
			tlrCtx->tmpArSelect.plmntAr.fitToSide =
				tmpArea->plmntAr.fitToSide;

			tlrCtx->tmpArSelect.anchrAr = tmpArea->anchrAr;

			return 1;
		}
	}

	return 0;
}

/* ========================================================================== */
/**
 *  area_fit_to_left()
 *
 * @brief  Checks if a specified free area is big enough for the required
 * allocation, and if the specified free area is the best (smallest) choice
 * of all discovered free areas.
 *
 * @param areaReq - dmmTILERContPageAreaT* - [in] Required allcoation area.
 *
 * @param atchAr - dmmTILERContPageAreaT* - [in] Area along which specified
 * side the search for a new free area is performed.
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @return int: True if a qualified free zones is discovered.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
int area_fit_to_left(struct dmmTILERContPageAreaT *areaReq,
		  struct dmmTILERContPageAreaT *atchAr,
		  struct dmmTILERContCtxT *tlrCtx)
{
	struct dmmTILERContPageAreaT *areaHit;
	struct dmmTILERContPageAreaSpecT fitArea;
	signed long X;
	signed long Y;
	signed long anchorX;
	signed long anchorY;
	int fit;

	areaHit = NULL;
	fitArea.anchrAr = atchAr;
	X = atchAr->x0 - 1;
	Y = atchAr->y0;
	anchorX = X;
	anchorY = Y;
	fit = 0;

	if (X >= 0 && X <= tlrCtx->contSizeX && Y >= 0 &&
			Y <= tlrCtx->contSizeY) {
		while (Y <= atchAr->y1) {
			anchorY = Y;
			if (1 == point_free_test(tlrCtx, X, Y, &areaHit)) {
				if (Y == atchAr->y0) {
					Y = (unsigned short)expand_top(tlrCtx,
					X, Y, &areaHit);
				}
				fitArea.ttlExpndAr.y0 = (unsigned short)(Y);
				fitArea.ttlExpndAr.y1 =
					(unsigned short)expand_bottom(tlrCtx,
					X, Y, &areaHit);
				fitArea.ttlExpndAr.x0 = (unsigned short)
					expand_line_on_bottom_to_left(tlrCtx,
					X, Y, fitArea.ttlExpndAr.y1 - Y);
				fitArea.ttlExpndAr.x1 = (unsigned short)(X);

				fitArea.ttlExpndAr.fitToSide = PSA_LEFT;
				area_required_to_allocated(areaReq,
					&(fitArea.ttlExpndAr),
					&(fitArea.plmntAr),
					anchorX, anchorY, PSA_LEFT);

				fit |= area_check_for_fit(&fitArea,
						       tlrCtx, areaReq);

				break;
			} else if (areaHit != NULL) {
				Y = areaHit->y1 + 1;
				areaHit = NULL;
			} else {
				printk(KERN_ERR"area fit failure\n");
			}
		}
	}

	return fit;
}

/* ========================================================================== */
/**
 *  area_fit_to_right()
 *
 * @brief  Checks if a specified free area is big enough for the required
 * allocation, and if the specified free area is the best (smallest) choice
 * of all discovered free areas.
 *
 * @param areaReq - dmmTILERContPageAreaT* - [in] Required allcoation area.
 *
 * @param atchAr - dmmTILERContPageAreaT* - [in] Area along which specified
 * side the search for a new free area is performed.
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @return int: True if a qualified free zones is discovered.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
int area_fit_to_right(struct dmmTILERContPageAreaT *areaReq,
		   struct dmmTILERContPageAreaT *atchAr,
		   struct dmmTILERContCtxT *tlrCtx)
{
	struct dmmTILERContPageAreaT *areaHit;
	struct dmmTILERContPageAreaSpecT fitArea;
	signed long X;
	signed long Y;
	signed long anchorX;
	signed long anchorY;
	int fit;

	areaHit = NULL;
	fitArea.anchrAr = atchAr;
	X = atchAr->x1 + 1;
	Y = atchAr->y0;
	anchorX = X;
	anchorY = Y;
	fit = 0;

	if (X >= 0 && X <= tlrCtx->contSizeX && Y >= 0 &&
			Y <= tlrCtx->contSizeY) {
		while (Y <= atchAr->y1) {
			anchorY = Y;
			if (1 == point_free_test(tlrCtx, X, Y, &areaHit)) {
				if (Y == atchAr->y0) {
					Y = (unsigned short)expand_top(tlrCtx,
					X, Y, &areaHit);
				}

				fitArea.ttlExpndAr.y0 = (unsigned short)(Y);
				fitArea.ttlExpndAr.y1 =
					(unsigned short)expand_bottom(tlrCtx,
					X, Y, &areaHit);
				fitArea.ttlExpndAr.x0 = (unsigned short)(X);
				fitArea.ttlExpndAr.x1 = (unsigned short)
					expand_line_on_bottom_to_right(tlrCtx,
					  X, Y, fitArea.ttlExpndAr.y1 - Y);

				fitArea.ttlExpndAr.fitToSide = PSA_RIGHT;
				area_required_to_allocated(areaReq,
					&(fitArea.ttlExpndAr),
					&(fitArea.plmntAr),
					anchorX, anchorY, PSA_RIGHT);

				fit |= area_check_for_fit(&fitArea,
						       tlrCtx, areaReq);

				break;
			} else if (areaHit != NULL) {
				Y = areaHit->y1 + 1;
				areaHit = NULL;
			} else {
				printk(KERN_ERR"area fit failure\n");
			}
		}
	}

	return fit;
}

/* ========================================================================== */
/**
 *  area_fit_to_top()
 *
 * @brief  Checks if a specified free area is big enough for the required
 * allocation, and if the specified free area is the best (smallest) choice
 * of all discovered free areas.
 *
 * @param areaReq - dmmTILERContPageAreaT* - [in] Required allcoation area.
 *
 * @param atchAr - dmmTILERContPageAreaT* - [in] Area along which specified
 * side the search for a new free area is performed.
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @return int: True if a qualified free zones is discovered.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
int area_fit_to_top(struct dmmTILERContPageAreaT *areaReq,
		 struct dmmTILERContPageAreaT *atchAr,
		 struct dmmTILERContCtxT *tlrCtx)
{
	struct dmmTILERContPageAreaT *areaHit;
	struct dmmTILERContPageAreaSpecT fitArea;
	signed long X;
	signed long Y;
	signed long anchorX;
	signed long anchorY;
	int fit;

	areaHit = NULL;
	fitArea.anchrAr = atchAr;
	X = atchAr->x0;
	Y = atchAr->y0 - 1;
	anchorX = X;
	anchorY = Y;
	fit = 0;

	if (X >= 0 && X < tlrCtx->contSizeX && Y >= 0
			&& Y < tlrCtx->contSizeY) {
		while (X <= atchAr->x1) {
			anchorX = X;
			if (1 == point_free_test(tlrCtx, X, Y, &areaHit)) {
				if (X == atchAr->x0) {
					X = (unsigned short)expand_left(tlrCtx,
					X, Y, &areaHit);
				}

				fitArea.ttlExpndAr.x0 = (unsigned short)(X);
				fitArea.ttlExpndAr.x1 =
					(unsigned short)expand_right(tlrCtx,
					    X, Y, &areaHit);
				fitArea.ttlExpndAr.y0 = (unsigned short)
					expand_line_on_right_to_top(tlrCtx,
					X, Y, fitArea.ttlExpndAr.x1 - X);
				fitArea.ttlExpndAr.y1 = (unsigned short)(Y);

				fitArea.ttlExpndAr.fitToSide = PSA_TOP;
				area_required_to_allocated(areaReq,
					&(fitArea.ttlExpndAr),
					&(fitArea.plmntAr), anchorX,
					anchorY, PSA_TOP);

				fit |= area_check_for_fit(&fitArea,
						       tlrCtx, areaReq);

				break;
			} else if (areaHit != NULL) {
				X = areaHit->x1 + 1;
				areaHit = NULL;
			} else {
				printk(KERN_ERR"area fit failure\n");
			}
		}
	}

	return fit;
}

/* ========================================================================== */
/**
 *  area_fit_to_bottom()
 *
 * @brief  Checks if a specified free area is big enough for the required
 * allocation, and if the specified free area is the best (smallest) choice
 * of all discovered free areas.
 *
 * @param areaReq - dmmTILERContPageAreaT* - [in] Required allcoation area.
 *
 * @param atchAr - dmmTILERContPageAreaT* - [in] Area along which specified
 * side the search for a new free area is performed.
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @return int: True if a qualified free zones is discovered.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
int area_fit_to_bottom(struct dmmTILERContPageAreaT *areaReq,
		    struct dmmTILERContPageAreaT *atchAr,
		    struct dmmTILERContCtxT *tlrCtx)
{
	struct dmmTILERContPageAreaT *areaHit;
	struct dmmTILERContPageAreaSpecT fitArea;
	signed long X;
	signed long Y;
	signed long anchorX;
	signed long anchorY;
	int fit;

	areaHit = NULL;
	fitArea.anchrAr = atchAr;
	X = atchAr->x0;
	Y = atchAr->y1 + 1;
	anchorX = X;
	anchorY = Y;
	fit = 0;

	if (X >= 0 && X < tlrCtx->contSizeX && Y >= 0 &&
			Y < tlrCtx->contSizeY) {
		while (X <= atchAr->x1) {
			if (1 == point_free_test(tlrCtx, X, Y, &areaHit)) {
				anchorX = X;
				if (X == atchAr->x0) {
					X = (unsigned short)expand_left(tlrCtx,
					X, Y, &areaHit);
				}

				fitArea.ttlExpndAr.x0 = (unsigned short)(X);
				fitArea.ttlExpndAr.x1 = (unsigned short)
					expand_right(tlrCtx, X, Y, &areaHit);
				fitArea.ttlExpndAr.y0 = (unsigned short)(Y);
				fitArea.ttlExpndAr.y1 = (unsigned short)
					expand_line_on_right_to_bottom(tlrCtx,
					  X, Y, fitArea.ttlExpndAr.x1 - X);

				fitArea.ttlExpndAr.fitToSide = PSA_BOTTOM;

				area_required_to_allocated(areaReq,
					&(fitArea.ttlExpndAr),
					&(fitArea.plmntAr), anchorX,
					anchorY, PSA_BOTTOM);

				fit |= area_check_for_fit(&fitArea,
						       tlrCtx, areaReq);

				break;
			} else if (areaHit != NULL) {
				X = areaHit->x1 + 1;
				areaHit = NULL;
			} else {
				printk(KERN_ERR"area fit failure\n");
			}
		}
	}

	return fit;
}

/* ========================================================================== */
/**
 *  alloc_2d_area()
 *
 * @brief  Allocates a 2D area and returns a pointer to it.
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param areaReq - dmmTILERContPageAreaT* - [in] Required area for allcoation.
 *
 * @return dmmTILERContPageAreaT*: pointer to the allocated area.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
struct dmmTILERContPageAreaT *alloc_2d_area(
				struct dmmTILERContCtxT *dmmTilerCtx,
				struct dmmTILERContPageAreaT *areaReq)
{
	struct dmmTILERContPageLstT *allocatedArea = NULL;
	struct dmmTILERContPageLstT *usedIter;

	usedIter = dmmTilerCtx->usdArList;

	dmmTilerCtx->tmpArSelect.ttlExpndAr.x0 = 0;
	dmmTilerCtx->tmpArSelect.ttlExpndAr.y0 = 0;
	dmmTilerCtx->tmpArSelect.ttlExpndAr.x1 = dmmTilerCtx->contSizeX - 1;
	dmmTilerCtx->tmpArSelect.ttlExpndAr.y1 = dmmTilerCtx->contSizeY - 1;

	if (usedIter != NULL) {
		int fit = 0;
		while (usedIter != NULL) {
			fit |= area_fit_to_top(areaReq, &(usedIter->pgAr),
					    dmmTilerCtx);
			fit |= area_fit_to_right(areaReq, &(usedIter->pgAr),
					      dmmTilerCtx);
			fit |= area_fit_to_bottom(areaReq, &(usedIter->pgAr),
					       dmmTilerCtx);
			fit |= area_fit_to_left(areaReq, &(usedIter->pgAr),
					     dmmTilerCtx);

			usedIter = usedIter->pgArNext;
		}

		if (fit > 0) {
			allocatedArea = (struct dmmTILERContPageLstT *)
					vmalloc(sizeof
					(struct dmmTILERContPageLstT));
			allocatedArea->pgAr.x0 =
				dmmTilerCtx->tmpArSelect.plmntAr.x0;
			allocatedArea->pgAr.y0 =
				dmmTilerCtx->tmpArSelect.plmntAr.y0;
			allocatedArea->pgAr.x1 =
				dmmTilerCtx->tmpArSelect.plmntAr.x1;
			allocatedArea->pgAr.y1 =
				dmmTilerCtx->tmpArSelect.plmntAr.y1;
			allocatedArea->pgAr.fitToSide =
				dmmTilerCtx->tmpArSelect.plmntAr.fitToSide;
			allocatedArea->anchrAr =
				dmmTilerCtx->tmpArSelect.anchrAr;

			usedIter = dmmTilerCtx->usdArList;
			while (usedIter->pgArNext != NULL)
				usedIter = usedIter->pgArNext;

			usedIter->pgArNext = allocatedArea;
		}
	} else {
		allocatedArea = (struct dmmTILERContPageLstT *)
				vmalloc(sizeof(struct dmmTILERContPageLstT));
		allocatedArea->pgAr.x0 = 0;
		allocatedArea->pgAr.y0 = 0;
		allocatedArea->pgAr.x1 = areaReq->x1;
		allocatedArea->pgAr.y1 = areaReq->y1;
		allocatedArea->pgAr.fitToSide = PSA_BOTTOM;
		allocatedArea->anchrAr = NULL;
		allocatedArea->pgArNext = NULL;

		dmmTilerCtx->usdArList = allocatedArea;
	}

	if (allocatedArea == NULL)
		return NULL;
	else
		return &(allocatedArea->pgAr);
}

/* ========================================================================== */
/**
 *  dealloc_2d_area()
 *
 * @brief  Deletes a 2D area from the TILER context.
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param areaRem - dmmTILERContPageAreaT* - [in] Area to remove.
 *
 * @return int: True if the specified area is successfuly deleted.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
int dealloc_2d_area(struct dmmTILERContCtxT *dmmTilerCtx,
			struct dmmTILERContPageAreaT *areaRem)
{
	struct dmmTILERContPageLstT *delItm;
	struct dmmTILERContPageLstT *usedIter;
	struct dmmTILERContPageLstT *usedPrev;

	delItm = NULL;
	usedIter = dmmTilerCtx->usdArList;
	usedPrev = NULL;

	while (usedIter != NULL) {
		if (areaRem->x0 == usedIter->pgAr.x0 &&
				areaRem->y0 == usedIter->pgAr.y0) {
			delItm = usedIter;
			if (usedPrev != NULL)
				usedPrev->pgArNext = usedIter->pgArNext;
			else
				dmmTilerCtx->usdArList =
					dmmTilerCtx->usdArList->pgArNext;

			break;
		}

		usedPrev = usedIter;
		usedIter = usedIter->pgArNext;
	}

	if (delItm != NULL) {

		signed long iter;
		enum errorCodeT eCode = DMM_NO_ERROR;

		if (delItm->pgAr.patCustomPages == 0) {
			unsigned long numPages = (delItm->pgAr.x1 -
			  delItm->pgAr.x0 + 1)*(delItm->pgAr.y1 -
			delItm->pgAr.y0 + 1);
			vfree(va);
		}

		if (eCode == DMM_NO_ERROR) {
			vfree(delItm->pgAr.patPageEntriesSpace);
			delItm->pgAr.patPageEntries = NULL;
			delItm->pgAr.patPageEntriesSpace = NULL;
		}

		vfree(delItm);
		return 1;
	} else {
		return 0;
	}
}

/* ========================================================================== */
/**
 *  search_2d_area()
 *
 * @brief  Deletes a 2D area from the TILER context.
 *
 * @param tlrCtx - dmmTILERContCtxT* - [in] TILER context structure.
 *
 * @param X - signed long - [in] X coordinate of the search area.
 *
 * @param Y - signed long - [in] X coordinate of the search area.
 *
 * @param xInvert - int - [in] X coordinate is inverted.
 *
 * @param yInvert - int - [in] Y coordinate is inverted.
 *
 * @return dmmTILERContPageAreaT*: pointer to the discovered 2D area
 * or NULL if no such area is found.
 *
 * @pre There is no pre conditions.
 *
 * @post There is no post conditions.
 *
 * @see
 */
/* ========================================================================== */
struct dmmTILERContPageAreaT *search_2d_area(
				struct dmmTILERContCtxT *dmmTilerCtx,
				signed long X,
				signed long Y,
				int xInvert,
				int yInvert)
{
	struct dmmTILERContPageLstT *usedIter;
	usedIter = dmmTilerCtx->usdArList;

	if (!xInvert && !yInvert) {
		while (usedIter != NULL) {
			if (X == usedIter->pgAr.x0 && Y == usedIter->pgAr.y0)
				return &(usedIter->pgAr);

			usedIter = usedIter->pgArNext;
		}
	} else if (xInvert && !yInvert) {
		while (usedIter != NULL) {
			if (X == usedIter->pgAr.x1 && Y == usedIter->pgAr.y0)
				return &(usedIter->pgAr);

			usedIter = usedIter->pgArNext;
		}
	} else if (!xInvert && yInvert) {
		while (usedIter != NULL) {
			if (X == usedIter->pgAr.x0 && Y == usedIter->pgAr.y1)
				return &(usedIter->pgAr);

			usedIter = usedIter->pgArNext;
		}
	} else {
		while (usedIter != NULL) {
			if (X == usedIter->pgAr.x1 && Y == usedIter->pgAr.y1)
				return &(usedIter->pgAr);

			usedIter = usedIter->pgArNext;
		}
	}

	return NULL;
}
