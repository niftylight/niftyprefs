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
 * @file niftyprefs.c
 */

/**
 * @addtogroup prefs
 * @{
 *
 */


#include <niftyprefs.h>
#include <libxml/tree.h>
#include "config.h"



/** a class of PrefsNode (e.g. if your object is "Person", 
    you have one "Person" class) */
typedef struct _NftPrefsClass NftPrefsClass;



/** a class of PrefsNodes (e.g. if your objects reflect persons, 
    you have one "Person" class) */
struct _NftPrefsClass
{
        /** name of object class */
        char name[NFT_PREFS_MAX_CLASSNAME+1];
        /** callback to create a new object from preferences (or NULL) */
        NftPrefsToObjFunc *toObj;
        /** callback to create preferences from the current object state (or NULL) */
        NftPrefsFromObjFunc *fromObj;
};


/** a node that holds various properties about an object (e.g. if your object
    reflects persons, you might have one PrefsNode for Alice and one for Bob) */
struct _NftPrefsObj
{
        /** libxml node */
        xmlNode *node;
        /** object */
        void *object;
        /** name of this object class */
        char className[NFT_PREFS_MAX_CLASSNAME+1];
};


/** context descriptor */
struct _NftPrefs
{
        /** temporary xmlDoc */
        xmlDoc *doc;
        /** list of registered PrefsObjClasses */
        NftPrefsClass *classes;
        /** amount of entries stored in list */
        size_t class_count;
        /** total amount of classes we have space for */
        size_t class_list_length;
        /** list of registered PrefsObjNodes */
        NftPrefsObj *objects;
        /** amount of entries stored in list */
        size_t obj_count;
        /** total amount of objects we have space for */
        size_t obj_list_length;
};




/******************************************************************************/
/**************************** STATIC FUNCTIONS ********************************/
/******************************************************************************/


/** libxml error handler */
static void _xml_error_handler(void *ctx, const char * msg, ...)
{
	xmlError *err;
	if(!(err = xmlGetLastError()))
	{
		NFT_LOG(L_WARNING, "libxml2 error trigger but no xmlGetLastError()");
		return;
	}

	/* convert libxml error-level to loglevel */
	NftLoglevel level = L_NOISY;
	switch(err->level)
	{
		case XML_ERR_NONE:
		{
			level = L_DEBUG;
			break;
		}

		case XML_ERR_WARNING:
		{
			level = L_WARNING;
			break;
		}

		case XML_ERR_ERROR:
		{
			level = L_ERROR;
			break;
		}

		case XML_ERR_FATAL:
		{
			level = L_ERROR;
			break;
		}
	};
		
	va_list args;
	va_start(args, msg);

	nft_log_va(level, err->file, "libxml2", err->line, msg, args);
	
	va_end(args);
}


/** find first unallocated NftPrefsClass entry in list */
static NftPrefsClass *_class_find_free(NftPrefsClass list[], size_t length)
{
	if(!list)
		NFT_LOG_NULL(NULL);

	/* find in list */
	int i;
	for(i=0; i < length; i++)
	{
		/** looking for name = NULL index? */
		if(list[i].name[0] == '\0')
		{
			return &list[i];
		}
	}

	return NULL;
}


/** find class by name */
static NftPrefsClass *_class_find_by_name(NftPrefs *p, const char *className)
{
	if(!p || !className)
		NFT_LOG_NULL(NULL);

        /* get list */
        NftPrefsClass *list;
        if(!(list = p->classes))
        {
                NFT_LOG(L_ERROR, "NftPrefs has no classes registered, yet");
                return NULL;
        }
        
	/* find in list */
	int i;
	for(i=0; i < p->class_list_length; i++)
	{
		/** looking for name = NULL index? */
		if(strcmp(list[i].name, className) == 0)
		{
			return &list[i];
		}
	}

        NFT_LOG(L_DEBUG, "didn't find class \"%s\"");
        
	return NULL;
}


/** find first unallocated NftPrefsObj entry in list */
static NftPrefsObj *_obj_find_free(NftPrefsObj list[], size_t length)
{
	if(!list)
		NFT_LOG_NULL(NULL);

	/* find in list */
	int i;
	for(i=0; i < length; i++)
	{
                if(!list[i].object && 
                   !list[i].node && 
                   list[i].className[0] == '\0')
                        return &list[i];
	}

	return NULL;
}


/** find node by object */
static NftPrefsObj *_obj_find_by_ptr(NftPrefsObj list[], size_t length, void *obj)
{
	if(!list || !obj)
		NFT_LOG_NULL(NULL);
        
	/* find in list */
	int i;
	for(i=0; i < length; i++)
	{
		/** looking for name = NULL index? */
		if(list[i].object == obj)
		{
			return &list[i];
		}
	}

        NFT_LOG(L_DEBUG, "object %p not registered", obj);
        
	return NULL;
}



/** clear prefs object descriptor */
static void _obj_clear(NftPrefs *p, NftPrefsObj *o)
{
        memset(o, 0, sizeof(NftPrefsObj));
        p->obj_count--;
}


/******************************************************************************/
/**************************** API FUNCTIONS ***********************************/
/******************************************************************************/


/**
 * initialize libniftyprefs - call this once before doing any other API call
 *
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftPrefs *nft_prefs_init()
{
        /* welcome */
        NFT_LOG(L_INFO, "%s - v%s", PACKAGE_NAME, PACKAGE_VERSION);
        
        /*
         * this initializes the library and check potential ABI mismatches
         * between the version it was compiled for and the actual shared
         * library used.
         */
        LIBXML_TEST_VERSION
        xmlSetBufferAllocationScheme(XML_BUFFER_ALLOC_DOUBLEIT);

        /* register error-logging function */
        xmlSetGenericErrorFunc(NULL, _xml_error_handler);

        /* needed for indented output */
        xmlKeepBlanksDefault(0);

        /* allocate new NftPrefs context */
        NftPrefs *p;
        if(!(p = calloc(1, sizeof(NftPrefs))))
        {
                NFT_LOG_PERROR("calloc");
                return NULL;
        }

        /* preallocate xmlDoc descriptor for this context */
        if(!(p->doc = xmlNewDoc(BAD_CAST "1.0")))
        {
                free(p);
                return NULL;
        }
        
        return p;
}


/**
 * deinitialize libniftyprefs - call this after doing the last API call to
 * finally clean up
 */
void nft_prefs_exit(NftPrefs *p)
{
        /* free xmlDoc */
        if(p->doc)
                xmlFreeDoc(p->doc);
        
        /* free lists */
        free(p->objects);
        free(p->classes);
        free(p);
        
        /* cleanup XML parser */
	xmlCleanupParser();
}


/**
 * register object class
 *
 * @param className unique name of this class
 * @param toObj pointer to NftPrefsToObjFunc used by this class
 * @param fromObj pointer to NftPrefsFromObjFunc used by this class
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_class_register(NftPrefs *p, const char *className, 
                                       NftPrefsToObjFunc *toObj, 
                                       NftPrefsFromObjFunc *fromObj)
{
        if(!className)
                NFT_LOG_NULL(NFT_FAILURE);

        
        if(strlen(className) == 0)
        {
                NFT_LOG(L_ERROR, "class name may not be empty");
                return NFT_FAILURE;
        }

        
/** increase list by this amount of entries if space runs out */
#define NFT_PREFS_CLASSBUF_INC	64

        
        /* enough space left? */
        if(p->class_list_length <= p->class_count)
        {
                /* increase buffer */
                if(!(p->classes = realloc(p->classes, 
                                          (p->class_list_length + NFT_PREFS_CLASSBUF_INC)*
                                          sizeof(NftPrefsClass))))
                {
                        NFT_LOG_PERROR("realloc()");
                        return NFT_FAILURE;
                }

                /* clear new memory */
                memset(&p->classes[p->class_list_length],
                       0, 
                       NFT_PREFS_CLASSBUF_INC*sizeof(NftPrefsClass));

                /* remember new listlength */
                p->class_list_length += NFT_PREFS_CLASSBUF_INC;
        }

        
        /* get an empty list entry */
        NftPrefsClass *n;
	if(!(n = _class_find_free(p->classes, p->class_list_length)))
	{
		NFT_LOG(L_ERROR, "Couldn't find free slot in NftPrefsClass list");
		return NFT_FAILURE;
	}

        
        /* register new class */
        strncpy(n->name, className, NFT_PREFS_MAX_CLASSNAME);
        n->toObj = toObj;
        n->fromObj = fromObj;

        
        /* another class registered */
        p->class_count++;

        
        return NFT_SUCCESS;
}


/**
 * unregister class from current context
 */
void nft_prefs_class_unregister(NftPrefs *p, const char *className)
{
        if(!p || !className)
                NFT_LOG_NULL();

        
        /* find class */
        NftPrefsClass *c;
        if(!(c = _class_find_by_name(p, className)))
        {
                NFT_LOG(L_ERROR, 
                        "tried to unregister class \"%s\" that is not registered.", 
                        className);
        }

        
        /* unregister all objects from this class */
        int i;
        for(i=0; i < p->obj_list_length; i++)
        {
                if(strcmp(p->objects[i].className, className) == 0)
                        _obj_clear(p, &p->objects[i]);
        }
        
        memset(c, 0, sizeof(NftPrefsClass));
        
        if(--p->class_count < 0)
                NFT_LOG(L_ERROR, "Negative class count. You messed up class_register() and class_unregister()... Double free?");
}


/**
 * register an object
 */
NftResult nft_prefs_obj_register(NftPrefs *p, const char *className, void *obj)
{
         if(!p || !className || !obj)
                NFT_LOG_NULL(NFT_FAILURE);

        /* find class */
        NftPrefsClass *c;
        if(!(c = _class_find_by_name(p, className)))
        {
                NFT_LOG(L_ERROR, "Unknown class \"%s\"", className);
                return NFT_FAILURE;
        }
        
/** increase list by this amount of entries if space runs out */
#define NFT_PREFS_OBJBUF_INC	64

        
        /* enough space left? */
        if(p->obj_list_length <= p->obj_count)
        {
                /* increase buffer */
                if(!(p->objects = realloc(p->objects, 
                                          (p->obj_list_length + NFT_PREFS_OBJBUF_INC)*
                                          sizeof(NftPrefsObj))))
                {
                        NFT_LOG_PERROR("realloc()");
                        return NFT_FAILURE;
                }

                
                /* clear new memory */
                memset(&p->objects[p->obj_list_length],
                       0, 
                       NFT_PREFS_OBJBUF_INC*sizeof(NftPrefsObj));

                
                /* remember new listlength */
                p->obj_list_length += NFT_PREFS_OBJBUF_INC;
        }

        
        /* get a empty list entry */
        NftPrefsObj *n;
	if(!(n = _obj_find_free(p->objects, p->obj_list_length)))
	{
		NFT_LOG(L_ERROR, "Couldn't find free slot in NftPrefsObj list");
		return NFT_FAILURE;
	}

        
        /* register new node */
        n->node = NULL;
        n->object = obj;
        strncpy(n->className, className, NFT_PREFS_MAX_CLASSNAME);

        
        /* another class registered */
        p->obj_count++;

        return NFT_SUCCESS;
}


/**
 * unregister object
 */
void nft_prefs_obj_unregister(NftPrefs *p, void *obj)
{
        if(!p || !obj)
                return;

        NftPrefsObj *n;
        if(!(n = _obj_find_by_ptr(p->objects, p->obj_list_length, obj)))
                return;

        _obj_clear(p, n);
}


NftResult nft_prefs_obj_to_prefs(NftPrefs *p, void *obj)
{
        return NFT_SUCCESS;
}


void *nft_prefs_obj_from_prefs(const char *className)
{
        return NULL;
}




/**
 * @}
 */
