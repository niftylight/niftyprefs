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
 * @file class.c
 */

/**
 * @addtogroup prefs_class
 * @{
 *
 */


#include <niftylog.h>
#include "class.h"
#include "updater.h"
#include "obj.h"
#include "prefs.h"



/** a class of PrefsObjects (e.g. if your object is "Person", 
    you have one "Person" class) */
struct _NftPrefsClass
{
        /** name of object class */
        char name[NFT_PREFS_MAX_CLASSNAME + 1];
        /** callback to create a new object from preferences (or NULL) */
        NftPrefsToObjFunc *toObj;
        /** callback to create preferences from the current object state (or NULL) */
        NftPrefsFromObjFunc *fromObj;
        /** slot of this class inside its NftPrefsClasses array */
        NftArraySlot slot;
        /** updaters of this class another */
        NftPrefsUpdaters updaters;
};



/******************************************************************************/
/**************************** STATIC FUNCTIONS ********************************/
/******************************************************************************/

/** finder for nft_array_find_slot() */
static bool _find_by_name(void *element, void *criterion, void *userptr)
{
        if(!element || !criterion)
                NFT_LOG_NULL(false);


        NftPrefsClass *c = element;
        const char *name = criterion;

        return (strcmp(c->name, name) == 0);
}



/******************************************************************************/
/**************************** PRIVATE FUNCTIONS *******************************/
/******************************************************************************/

/** initialize class array */
NftResult _class_init_array(NftPrefsClasses * a)
{
        /* initialize class-array */
        return nft_array_init(a, sizeof(NftPrefsClass));
}


/** find class by name */
NftPrefsClass *_class_find_by_name(NftPrefsClasses * c, const char *name)
{
        /* find class in array */
        NftArraySlot slot;
        if(!nft_array_find_slot
           (c, &slot, _find_by_name, (void *) name, NULL))
        {
                NFT_LOG(L_DEBUG, "Class \"%s\" not found", name);
                return NULL;
        }

        return nft_array_get_element(c, slot);
}


/** free all resources of one NftPrefsClass */
void _class_free(NftPrefs * p, NftPrefsClass * klass)
{
        if(!klass)
                return;

        /* free updater array */
        nft_array_deinit(&klass->updaters);

        /* free array slot */
        nft_array_slot_free(_prefs_classes(p), klass->slot);

        /* invalidate class */
        klass->name[0] = '\0';
        klass->fromObj = NULL;
        klass->toObj = NULL;
}


/** getter */
NftPrefsFromObjFunc *_class_fromObj(NftPrefsClass * c)
{
        return c->fromObj;
}


/** getter */
NftPrefsToObjFunc *_class_toObj(NftPrefsClass * c)
{
        return c->toObj;
}


/** getter */
NftPrefsUpdaters *_class_updaters(NftPrefsClass * c)
{
        return &c->updaters;
}



/******************************************************************************/
/**************************** API FUNCTIONS ***********************************/
/******************************************************************************/

/**
 * register object class
 *
 * @param p NftPrefs context where new class should be registered to
 * @param className unique name of new class
 * @param toObj pointer to NftPrefsToObjFunc used by the new class
 * @param fromObj pointer to NftPrefsFromObjFunc used by the new class
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_class_register(NftPrefs * p, const char *className,
                                   NftPrefsToObjFunc * toObj,
                                   NftPrefsFromObjFunc * fromObj)
{
        if(!p || !className)
                NFT_LOG_NULL(NFT_FAILURE);


        if(strlen(className) == 0)
        {
                NFT_LOG(L_ERROR, "class name may not be empty");
                return NFT_FAILURE;
        }

        /* check if class is already registered */
        NFT_LOG(L_DEBUG,
                "Checking if another class \"%s\" is already registered...",
                className);
        if(_class_find_by_name(_prefs_classes(p), className))
        {
                NFT_LOG(L_ERROR, "class named \"%s\" already registered",
                        className);
                return NFT_FAILURE;
        }

        /* allocate new slot in class array */
        NftArraySlot s;
        if(!(nft_array_slot_alloc(_prefs_classes(p), &s)))
        {
                NFT_LOG(L_ERROR, "Failed to allocate new array slot");
                return NFT_FAILURE;
        }

        /* get empty array element */
        NftPrefsClass *n;
        if(!(n = nft_array_get_element(_prefs_classes(p), s)))
        {
                NFT_LOG(L_ERROR, "Failed to get element from array slot");
                goto _pcr_error;
        }

        /* allocate new array for updater functions */
        if(!_updater_init_array(&n->updaters))
        {
                NFT_LOG(L_ERROR, "Failed to init updater array");
                goto _pcr_error;
        }

        /* register new class */
        strncpy(n->name, className, NFT_PREFS_MAX_CLASSNAME);
        n->toObj = toObj;
        n->fromObj = fromObj;
        n->slot = s;

        return NFT_SUCCESS;

_pcr_error:
        nft_array_slot_free(_prefs_classes(p), s);
        return NFT_FAILURE;
}


/**
 * unregister class from current context
 *
 * @param p NftPrefs context
 * @param className name of class
 */
void nft_prefs_class_unregister(NftPrefs * p, const char *className)
{
        if(!p || !className)
                NFT_LOG_NULL();



        /* find class */
        NftPrefsClass *klass;
        if(!(klass = _class_find_by_name(_prefs_classes(p), className)))
        {
                NFT_LOG(L_ERROR,
                        "tried to unregister class \"%s\" that is not registered.",
                        className);
                return;
        }


        /* free class */
        _class_free(p, klass);

}

/**
 * @}
 */
