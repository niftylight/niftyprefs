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





/** a node that holds various properties about an object (e.g. if your object
    reflects persons, you might have one PrefsObj for Alice and one for Bob) */
struct _NftPrefsObj
{
        /** object */
        void *object;
        /** NftPrefsClass this object belongs to */
        NftPrefsClass *klass;
        /** slot of this object inside its NftPrefsObjs array */
    	NftArraySlot slot;
};



/******************************************************************************/
/**************************** STATIC FUNCTIONS ********************************/
/******************************************************************************/

/** finder for nft_array_find_slot() */
static bool _obj_find_by_ptr(void *element, void *criterion, void *userptr)
{
	if(!element || !criterion)
		NFT_LOG_NULL(-1);
        
	NftPrefsObj *o = element;

    	return (o->object == criterion);
}



/******************************************************************************/
/**************************** PRIVATE FUNCTIONS *******************************/
/******************************************************************************/

/** initialize obj array */
NftResult prefs_obj_init_array(NftPrefsObjs *o)
{
	/* initialize class-array */
	return nft_array_init(o, sizeof(NftPrefsObj));
}


/** free all resources of one NftPrefsObj */
void prefs_obj_free(NftPrefsObj *obj)
{
        if(!obj)
               return;

        /* deallocate array slot */
    	nft_array_slot_free(prefs_class_objects(obj->klass), obj->slot);

    	/* invalidate obj descriptor */
        obj->object = NULL;
        obj->klass = NULL;
}


/** find object by pointer */
NftPrefsObj *prefs_obj_find_by_ptr(NftPrefsObjs *array, void *obj)
{
    	if(!array)
		NFT_LOG_NULL(NULL);
    
	NftArraySlot slot;
        if(nft_array_find_slot(array, &slot, _obj_find_by_ptr, (void *) obj, NULL))
    	{
		NFT_LOG(L_DEBUG, "Object %p not found", obj);
		return NULL;
	}
    
    	/* get object */
        NftPrefsObj *o;
    	if(!(o = nft_array_get_element(array, slot)))
    	{
		NFT_LOG(L_ERROR, "Null object?");
		return NULL;
	}

    	return o;
}

/******************************************************************************/
/**************************** API FUNCTIONS ***********************************/
/******************************************************************************/

/**
 * register an object
 *
 * @param p NftPrefs context
 * @param className name of class
 * @param obj pointer to object to register
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_obj_register(NftPrefs *p, const char *className, void *obj)
{
         if(!p || !className || !obj)
                NFT_LOG_NULL(NFT_FAILURE);

        /* find class */
    	NftPrefsClass *c;
    	if(!(c = prefs_class_find_by_name(prefs_classes(p), className)))
        {
                NFT_LOG(L_ERROR, "Unknown class \"%s\"", className);
                return NFT_FAILURE;
        }

    	/** allocate new slot in objects array */
	NftArraySlot s;
	if(!(nft_array_slot_alloc(prefs_class_objects(c), &s)))
	{
		NFT_LOG(L_ERROR, "Failed to allocate new slot");
		return NFT_FAILURE;
	}
    
        /* get empty array element */
        NftPrefsObj *o;
    	if(!(o = nft_array_get_element(prefs_class_objects(c), s)))
    	{
		NFT_LOG(L_ERROR, "Failed to get element from array");
		nft_array_slot_free(prefs_class_objects(c), s);
		return NFT_FAILURE;
	}
    
             
        /* register new node */
        o->object = obj;
        o->klass = c;
        o->slot = s;
    
        return NFT_SUCCESS;
}


/**
 * unregister object
 *
 * @param p NftPrefs context
 * @param className name of class
 * @param obj pointer to object to unregister
 */
void nft_prefs_obj_unregister(NftPrefs *p, const char *className, void *obj)
{
        if(!p || !className || !obj)
                return;

        /* find class */
        NftPrefsClass *c;
        if((c = prefs_class_find_by_name(prefs_classes(p), className)) < 0)
        {
                NFT_LOG(L_ERROR, "Unknown class \"%s\"", className);
                return;
        }


    	/* find object in class */
    	NftPrefsObj *o;
    	if(!(o = prefs_obj_find_by_ptr(prefs_class_objects(c), obj)))
	{
                NFT_LOG(L_ERROR, "Object \"%p\" not found in class \"%s\"", obj, className);
	    	return;
        }
    
        prefs_obj_free(o);
}


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
NftPrefsNode *nft_prefs_obj_to_node(NftPrefs *p, const char *className, void *obj, void *userptr)
{
        if(!p || !className || !obj)
                NFT_LOG_NULL(NULL);

        /* find class */
        NftPrefsClass *c;
        if((c = prefs_class_find_by_name(prefs_classes(p), className)) < 0)
        {
                NFT_LOG(L_ERROR, "Unknown class \"%s\"", className);
                return NULL;
        }

        
        /* find object descriptor */
        /*NftPrefsObjSlot os;
        if((os = _obj_find_by_ptr(c, obj)) < 0)
                return NULL;

        NftPrefsObj *o = _obj_get(c, os);*/

        /* new node */
        NftPrefsNode *node;
        if(!(node = xmlNewNode(NULL, BAD_CAST className)))
                return NULL;

        /* set name of node */
        xmlNodeSetName(node, BAD_CAST className);
        
        /* call prefsFromObj() registered for this class */
        if(!prefs_class_fromObj(c)(p, node, obj, userptr))
    	{
		NFT_LOG(L_ERROR, "fromObj() of class \"%s\" failed.",
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
void *nft_prefs_obj_from_node(NftPrefs *p, NftPrefsNode *n, void *userptr)
{
        /* find object class */
        NftPrefsClass *c;
        if((c = prefs_class_find_by_name(prefs_classes(p), (const char *) n->name)) < 0)
        {
                NFT_LOG(L_ERROR, "Unknown class \"%s\"", n->name);
                return NULL;
        }

        
        /* create object from prefs */
        void *result = NULL;
        if(!(prefs_class_toObj(c)(p, &result, n, userptr)))
        {
                NFT_LOG(L_ERROR, "toObj() of class \"%s\" function failed",
                        n->name);
	    	return NULL;
        }

        /* validate */
        if(!(result))
    	{
                NFT_LOG(L_ERROR, "<%s> toObj() function returned successfully but created NULL object", n->name);
		return NULL;
	}
    
    	/* register new object */
    	if(!(nft_prefs_obj_register(p, (const char *) n->name, result)))
    	{
		NFT_LOG(L_ERROR, "Failed to register new \"%s\" object", n->name);
	}
    
        return result;
}


/**
 * @}
 */
