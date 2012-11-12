/*
 * libniftyprefs - lightweight modelless preferences management library
 * Copyright (C) 2006-2012 Daniel Hiepler <daniel@niftylight.de>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


/**
 * @file nifty-array.h
 */

/**
 * @defgroup array NftArray
 * @brief simple API to handle arrays
 * @{
 */


#ifndef _NIFTYPREFS_ARRAY_H
#define _NIFTYPREFS_ARRAY_H



#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include "nifty-primitives.h"



/** slot to define position inside array */
typedef size_t                  NftArraySlot;

/** descriptor for one array element */
typedef struct _NftElement      NftElement;


/** increase array by this amount of element entries if space runs out */
#define NFT_ARRAY_DEFAULT_INC 64
/** maximum length of NftArray->name */
#define NFT_ARRAY_NAME_MAXLEN 64

/** descriptor to handle arbitrary pointer arrays */
typedef struct _NftArray
{
        /** optional variable to differ array types */
        int                             type;
        /** optional printable name of this array */
        char                            name[NFT_ARRAY_NAME_MAXLEN];
        /** size of one element in bytes */
        size_t                          elementsize;
        /** amount of elements currently in array */
        size_t                          elementcount;
        /** amount of elements array can currently hold (size of NftArray->elements) */
        size_t                          arraysize;
        /** NftElement array with pointers to elements inside buffer */
        NftElement                     *elements;
        /** buffer for actual elements. large enough to hold space*elementsize bytes */
        char                           *buffer;
} NftArray;



NftResult                       nft_array_init(NftArray * a, size_t elementSize);
void                            nft_array_deinit(NftArray * a);

void                            nft_array_set_name(NftArray * a, const char *name);
void                            nft_array_set_type(NftArray * a, int type);
bool                            nft_array_is_type(NftArray * a, int type);

ssize_t                         nft_array_get_elementcount(NftArray * a);
const char                     *nft_array_get_name(NftArray * a);
int                             nft_array_get_type(NftArray * a);

NftResult                       nft_array_slot_alloc(NftArray * a, NftArraySlot * s);
void                            nft_array_slot_free(NftArray * a, NftArraySlot s);
void                           *nft_array_get_element(NftArray * a, NftArraySlot s);

NftResult                       nft_array_find_slot(NftArray * a, NftArraySlot * s, bool(*finder) (void *element, void *criterion, void *userptr), void *criterion, void *userptr);
NftResult                       nft_array_foreach_element(NftArray * a, bool(*foreach) (void *element, void *userptr), void *userptr);

#endif /** _NIFTYPREFS_ARRAY_H */

/**
 * @}
 */
