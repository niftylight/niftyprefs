/*
 * libniftyprefs - lightweight modelless preferences management library
 * Copyright (C) 2006-2013 Daniel Hiepler <daniel@niftylight.de>
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
 * @file obj.c
 */

/**
 * @addtogroup prefs_obj
 * @{
 *
 */


#include <niftylog.h>
#include "prefs.h"
#include "class.h"







/******************************************************************************/
/**************************** STATIC FUNCTIONS ********************************/
/******************************************************************************/


/******************************************************************************/
/**************************** PRIVATE FUNCTIONS *******************************/
/******************************************************************************/


/******************************************************************************/
/**************************** API FUNCTIONS ***********************************/
/******************************************************************************/

/**
 * create a NftPrefsNode from a previously registered object 
 *
 * @param p NftPrefs context
 * @param className name of class
 * @param obj pointer to object
 * @param userptr arbitrary pointer that will be passed to NftPrefsFromObjFunc
 * @result newly created NftPrefsNode or NULL
 * @note you should only need to use that from inside a NftPrefsFromObjFunc where it will be freed
 */
NftPrefsNode *nft_prefs_obj_to_node(NftPrefs * p, const char *className,
                                    void *obj, void *userptr)
{
        if(!p || !className)
                NFT_LOG_NULL(NULL);

        /* find class */
        NftPrefsClass *c;
        if(!(c = _prefs_class_find_by_name(_prefs_classes(p), className)))
        {
                NFT_LOG(L_ERROR, "Unknown prefs class \"%s\"", className);
                return NULL;
        }


        /* find object descriptor */
        /* NftPrefsObjSlot os; if((os = _obj_find_by_ptr(c, obj)) < 0) return
         * NULL; NftPrefsObj *o = _obj_get(c, os); */

        /* new node */
        NftPrefsNode *node;
        if(!(node = nft_prefs_node_alloc(className)))
                return NULL;


        /* call prefsFromObj() registered for this class */
        if(!_prefs_class_fromObj(c) (p, node, obj, userptr))
        {
                NFT_LOG(L_ERROR, "prefsFromObj() of class \"%s\" failed.",
                        className);
                return NULL;
        }

        return node;
}


/**
 * create object from a NftPrefsNode
 *
 * @param p NftPrefs context
 * @param n NftPrefsNode
 * @param userptr arbitrary function that will be passed to NftPrefsToObjFunc
 * @result newly created object or NULL
 * @note you should only need to use that from inside a NftPrefsToObjFunc where it will be freed
 */
void *nft_prefs_obj_from_node(NftPrefs * p, NftPrefsNode * n, void *userptr)
{
        if(!p || !n)
                NFT_LOG_NULL(NULL);

        /* find object class */
        NftPrefsClass *c;
        if(!
           (c =
            _prefs_class_find_by_name(_prefs_classes(p),
                                     (const char *) n->name)))
        {
                NFT_LOG(L_ERROR, "Unknown prefs class \"%s\"", n->name);
                return NULL;
        }


        /* create object from prefs */
        void *result = NULL;
        if(!(_prefs_class_toObj(c) (p, &result, n, userptr)))
        {
                NFT_LOG(L_ERROR,
                        "prefsToObj() of class \"%s\" function failed",
                        n->name);
                return NULL;
        }

        return result;
}


/**
 * @}
 */
