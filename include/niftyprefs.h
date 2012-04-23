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
 * @file niftyprefs.h
 */


/**      
 * @defgroup prefs niftyprefs
 * @brief main API
 *
 * @{
 */

#ifndef _NIFTYPREFS_H
#define _NIFTYPREFS_H


#include <niftylog.h>



/** a node that holds various properties about an object */
typedef struct _NftPrefsNode NftPrefsNode;


/** 
 * function that creates a config-node for a certain object 
 *
 * @param newNode newly created empty node that will be filled by the function
 * @param parentNode possible parentNode when having a tree structure
 * @param obj the object to process
 * @param userptr arbitrary pointer defined upon registering the object class
 * @result NFT_SUCCESS or NFT_FAILURE (processing will be aborted upon failure)
 */
typedef NftResult (NftPrefsFromObjFunc)(const NftPrefsNode *newNode, const NftPrefsNode *parentNode, const void *obj, const void *userptr);
/** 
 * function that creates an object from a config-node 
 *
 * @param newObject space to put the pointer to a newly allocated object
 * @param node the preference node describing the object that's about to be created
 * @param parentObject possible parentObject when having a tree stracture
 * @param userptr arbitrary pointer defined upon registering the object class
 * @result NFT_SUCCESS or NFT_FAILURE (processing will be aborted upon failure)
 */
typedef NftResult (NftPrefsToObjFunc)(void **newObject, const NftPrefsNode *node, const void *parentObject, const void *userptr);




NftResult   nft_prefs_init();
void        nft_prefs_exit();

NftResult   nft_prefs_obj_class_register(const char *className, NftPrefsToObjFunc *toObj, NftPrefsFromObjFunc *fromObj);
void        nft_prefs_obj_class_unregister(const char *className);
NftResult   nft_prefs_obj_register(const char *className, void *obj);
void        nft_prefs_obj_unregister(void *obj);

NftResult   nft_prefs_obj_to_prefs(NftPrefsNode **node, void *obj);
NftResult   nft_prefs_obj_from_prefs(const NftPrefsNode *node, void **obj);



#endif /* _NIFTYPREFS_H */


/**
 * @}
 */
