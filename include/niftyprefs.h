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
 * @brief abstract preference handling for arbitrary objects
 *
 * Use case 1: Generate preferences from existing object
 *  1.1 nft_prefs_init()
 *  1.2 nft_prefs_class_register()
 *  1.3 [create object]
 *  1.4 nft_prefs_obj_register(obj)
 *  1.5 prefsObj = nft_prefs_from_obj(obj)
 *  1.6 [use prefsObj]
 *  1.7 nft_prefs_obj_free(prefsObj)
 *
 * Use case 2: Generate object from preferences
 *  1.1 nft_prefs_init()
 *  1.2 nft_prefs_class_register()
 *  1.3 create NftPrefsNode nft_prefs_node_from_file() / nft_prefs_node_from_buffer()
 *  1.4 create object(s) from NftPrefsNode
 *
 * Use case 3: Dump preferences to file
 * Use case 4: Dump preferences to string
 * Use case 5: Parse preferences from file
 * Use case 6: Parse preferences from string
 * @{
 */

#ifndef _NIFTYPREFS_H
#define _NIFTYPREFS_H


#include <niftylog.h>


/** maximum length of classname */
#define NFT_PREFS_MAX_CLASSNAME 64


/** a context holding a list of PrefsClasses and PrefsNodes */
typedef struct _NftPrefs NftPrefs;
/** an object descriptor that holds various properties about an object */
typedef struct _NftPrefsObj NftPrefsObj;
/** wrapper type for one xmlNode */
typedef xmlNode NftPrefsNode;


/** 
 * function that creates a config-node for a certain object 
 *
 * @param newNode newly created empty node that will be filled by the function
 * @param parentNode possible parentNode when having a tree structure
 * @param obj the object to process
 * @param userptr arbitrary pointer defined upon registering the object class
 * @result NFT_SUCCESS or NFT_FAILURE (processing will be aborted upon failure)
 */
typedef NftResult (NftPrefsFromObjFunc)(NftPrefsObj *newObj, 
                                        void *obj, void *userptr);


/** 
 * function that allocates a new object from a config-node 
 *
 * @param newObj space to put the pointer to a newly allocated object
 * @param parentObj possible parent object (or NULL)
 * @param firstChildObj possible child object (or NULL) 
 * @param prevObj possible previous object (or NULL) 
 * @param nextObj possible next object (or NULL)
 * @param node the preference node describing the object that's about to be created
 * @param userptr arbitrary pointer defined upon registering the object class
 * @result NFT_SUCCESS or NFT_FAILURE (processing will be aborted upon failure)
 */
typedef NftResult (NftPrefsToObjFunc)(void **newObj, void *parentObj, 
                                      void *firstChildObj, 
                                      void *prevObj, void *nextObj, 
                                      NftPrefsObj *node, const void *userptr);




NftPrefs *      nft_prefs_init();
void            nft_prefs_exit(NftPrefs *prefs);

NftResult       nft_prefs_class_register(NftPrefs *p, const char *className, NftPrefsToObjFunc *toObj, NftPrefsFromObjFunc *fromObj);
void            nft_prefs_class_unregister(NftPrefs *p, const char *className);

NftResult       nft_prefs_obj_register(NftPrefs *p, const char *className, void *obj);
void            nft_prefs_obj_unregister(NftPrefs *p, void *obj);

NftPrefsNode *  nft_prefs_node_from_file(NftPrefs *p, const char *filename);
NftPrefsNode *  nft_prefs_node_from_buffer(NftPrefs *p, char *buffer, size_t bufsize);

NftResult       nft_prefs_node_to_file(NftPrefs *p, NftPrefsNode *n, const char *filename);
char *          nft_prefs_node_to_buffer(NftPrefs *p, NftPrefsNode *n);





#endif /* _NIFTYPREFS_H */


/**
 * @}
 */
