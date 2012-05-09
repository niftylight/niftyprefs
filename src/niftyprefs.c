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
#include "config.h"
#include "niftyprefs-version.h"



/** a class of PrefsNode (e.g. if your object is "Person", 
    you have one "Person" class) */
typedef struct _NftPrefsClass NftPrefsClass;
/** type for position in a table */
typedef ssize_t NftPrefsClassSlot;
/** type for position in a table */
typedef ssize_t NftPrefsObjSlot;




/** a node that holds various properties about an object (e.g. if your object
    reflects persons, you might have one PrefsObj for Alice and one for Bob) */
struct _NftPrefsObj
{
        /** object */
        void *object;
        /** class this object belongs to */
        NftPrefsClassSlot classSlot;
};


/** a class of PrefsObjects (e.g. if your objects reflect persons, 
    you have one "Person" class) */
struct _NftPrefsClass
{
        /** name of object class */
        char name[NFT_PREFS_MAX_CLASSNAME+1];
        /** callback to create a new object from preferences (or NULL) */
        NftPrefsToObjFunc *toObj;
        /** callback to create preferences from the current object state (or NULL) */
        NftPrefsFromObjFunc *fromObj;
        /** list of registered PrefsObjs */
        NftPrefsObj *objects;
        /** amount of entries stored in list */
        size_t obj_count;
        /** total amount of objects we have space for */
        size_t obj_list_length;
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


/** get class from slot */
static NftPrefsClass *_class_get(NftPrefs *p, NftPrefsClassSlot s)
{
        if(!p || !p->classes)
                NFT_LOG_NULL(NULL);

        if(s < 0 || s >= p->class_list_length)
        {
                NFT_LOG(L_ERROR, "invalid slot: %d (must be 0 < slot < %d)", s, p->class_list_length);
                return NULL;
        }
        
        return &p->classes[s];
}

/** get object from slot */
static NftPrefsObj *_obj_get(NftPrefsClass *c, NftPrefsObjSlot s)
{
        if(!c || !c->objects)
                NFT_LOG_NULL(NULL);

        if(s < 0 || s >= c->obj_list_length)
        {
                NFT_LOG(L_ERROR, "invalid slot: %d (must be 0 < slot < %d)", s, c->obj_list_length);
                return NULL;
        }
        
        return &c->objects[s];
}


/** find first unallocated NftPrefsClass entry in list */
static NftPrefsClass *_class_find_free(NftPrefs *p)
{
	if(!p)
		NFT_LOG_NULL(NULL);

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
        
	/* find free in list */
	int i;
	for(i=0; i < p->class_list_length; i++)
	{
		/** looking for name = NULL index? */
		if(p->classes[i].name[0] == '\0')
		{
			return &p->classes[i];
		}
	}

	return NULL;
}


/** find first unallocated NftPrefsObj entry in list */
static NftPrefsObj *_obj_find_free(NftPrefsClass *c)
{
	if(!c)
		NFT_LOG_NULL(NULL);

        /** increase list by this amount of entries if space runs out */
#define NFT_PREFS_OBJBUF_INC	64

        
        /* enough space left? */
        if(c->obj_list_length <= c->obj_count)
        {
                /* increase buffer */
                if(!(c->objects = realloc(c->objects, 
                                          (c->obj_list_length + NFT_PREFS_OBJBUF_INC)*
                                          sizeof(NftPrefsObj))))
                {
                        NFT_LOG_PERROR("realloc()");
                        return NFT_FAILURE;
                }

                
                /* clear new memory */
                memset(&c->objects[c->obj_list_length],
                       0, 
                       NFT_PREFS_OBJBUF_INC*sizeof(NftPrefsObj));

                
                /* remember new listlength */
                c->obj_list_length += NFT_PREFS_OBJBUF_INC;
        }

        
	/* find free slot in list */
	int i;
	for(i=0; i < c->obj_list_length; i++)
	{
                if(!c->objects[i].object)
                        return &c->objects[i];
	}

	return NULL;
}





/** find class by name */
static NftPrefsClassSlot _class_find_by_name(NftPrefs *p, const char *className)
{
	if(!p || !className || !p->classes)
		NFT_LOG_NULL(-1);

        
	/* find in list */
	NftPrefsClassSlot i;
	for(i=0; i < p->class_list_length; i++)
	{
		/** looking for name = NULL index? */
		if(strcmp(p->classes[i].name, className) == 0)
		{
			return i;
		}
	}

        NFT_LOG(L_DEBUG, "didn't find class \"%s\"", className);
        
	return -1;
}


/** find node by object */
static NftPrefsObjSlot _obj_find_by_ptr(NftPrefsClass *c, void *obj)
{
	if(!c || !obj || !c->objects)
		NFT_LOG_NULL(-1);
        
	/* find in list */
	NftPrefsObjSlot i;
	for(i=0; i < c->obj_list_length; i++)
	{
		/** looking for name = NULL index? */
		if(c->objects[i].object == obj)
		{
			return i;
		}
	}

        NFT_LOG(L_DEBUG, "object %p not registered", obj);
        
	return -1;
}





/** create object from xmlDoc */
static NftPrefsObj *_obj_from_doc(NftPrefs *p, xmlDoc *doc, void *userptr)
{
        if(!doc)
                NFT_LOG_NULL(NULL);
        
        /* get node */
        xmlNode *node;
        if(!(node = xmlDocGetRootElement(doc)))
        {
                NFT_LOG(L_ERROR, "No root element found in XML");
                return NULL;
        }

        /* find object class */
        NftPrefsClassSlot cs;
        if((cs = _class_find_by_name(p, (char *) node->name)) < 0)
                return NULL;

        NftPrefsClass *c = _class_get(p, cs);

        /* register new object */
        NftPrefsObj *o;
        if(!(o = _obj_find_free(c)))
                return NULL;

        /* remember object class */
        o->classSlot = cs;
        
        /* create object from prefs */
        void *result = NULL;
        if(!(c->toObj(&result, node, userptr)))
        {
                NFT_LOG(L_ERROR, "toObj() function failed");
        }

        /* remember new object */
        o->object = result;

        return o;
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
        NFT_LOG(L_INFO, "%s - v%s", PACKAGE_NAME, NFT_PREFS_VERSION_LONG);
        
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

        
        return p;
}


/**
 * free all resources of one NftPrefsObj
 */
static void prefs_obj_free(NftPrefsObj *obj)
{
        if(!obj)
               return;

      
        /* invalidate obj descriptor */
        obj->object = NULL;
        obj->classSlot = -1;
}


/**
 * free all resources of one NftPrefsClass
 */
static void prefs_class_free(NftPrefsClass *klass)
{
        if(!klass)
                return;

        if(klass->objects)
        {
                /* free all objects */
                int i;
                for(i = 0; i < klass->obj_list_length; i++)
                {
                        prefs_obj_free(_obj_get(klass, i));
                }

                /* free object list */
                free(klass->objects);
        }
        
        /* invalidate class */
        klass->name[0] = '\0';
        klass->fromObj = NULL;
        klass->toObj = NULL;
        klass->objects = NULL;
        klass->obj_count = 0;
        klass->obj_list_length = 0;
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

        
        /* free all classes */
        if(p->classes)
        {
                int i;
                for(i = 0; i < p->class_list_length; i++)
                {
                        prefs_class_free(_class_get(p, i));
                }

                free(p->classes);
                p->classes = NULL;
                p->class_count = 0;
                p->class_list_length = 0;
        }

        
        /* free xmlDoc */
        if(p->doc)
                xmlFreeDoc(p->doc);

        
        /* free descriptor */
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

        if(_class_find_by_name(p, className) >= 0)
        {
                NFT_LOG(L_ERROR, "class named \"%s\" already registered", className);
                return NFT_FAILURE;
        }
        
        /* get an empty list entry */
        NftPrefsClass *n;
	if(!(n = _class_find_free(p)))
	{
		NFT_LOG(L_ERROR, "Couldn't find free slot in NftPrefsClass list");
		return NFT_FAILURE;
	}

        
        /* register new class */
        strncpy(n->name, className, NFT_PREFS_MAX_CLASSNAME);
        n->toObj = toObj;
        n->fromObj = fromObj;
        n->objects = NULL;
        n->obj_count = 0;
        n->obj_list_length = 0;
        
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
        NftPrefsClassSlot cs;
        if((cs = _class_find_by_name(p, className)) < 0)
        {
                NFT_LOG(L_ERROR, 
                        "tried to unregister class \"%s\" that is not registered.", 
                        className);
        }

        
        /* free class */
        prefs_class_free(_class_get(p, cs));
        
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
        NftPrefsClassSlot cs;
        if((cs = _class_find_by_name(p, className)) < 0)
        {
                NFT_LOG(L_ERROR, "Unknown class \"%s\"", className);
                return NFT_FAILURE;
        }
        
        /* get a empty list entry */
        NftPrefsObj *o;
	if(!(o = _obj_find_free(_class_get(p, cs))))
	{
		NFT_LOG(L_ERROR, "Couldn't find free slot in NftPrefsObj list");
		return NFT_FAILURE;
	}

        
        /* register new node */
        o->object = obj;
        o->classSlot = cs;
        
        
        /* another class registered */
        p->classes[cs].obj_count++;

        return NFT_SUCCESS;
}


/**
 * unregister object
 */
void nft_prefs_obj_unregister(NftPrefs *p, const char *className, void *obj)
{
        if(!p || !className || !obj)
                return;

        /* find class */
        NftPrefsClassSlot cs;
        if((cs = _class_find_by_name(p, className)) < 0)
        {
                NFT_LOG(L_ERROR, "Unknown class \"%s\"", className);
                return;
        }
        
        NftPrefsObjSlot os;
        if((os = _obj_find_by_ptr(_class_get(p, cs), obj)) < 0)
                return;

        prefs_obj_free(_obj_get(_class_get(p, cs), os));
}


/**
 * create preferences buffer from current state of object
 * 
 * @result string holding xml representation of object (use free() to deallocate)
 */
char *nft_prefs_obj_to_buffer(NftPrefs *p, const char *className, void *obj, void *userptr)
{
        if(!p || !className || !obj)
                NFT_LOG_NULL(NULL);

        /* find object class */
        NftPrefsClassSlot cs;
        if((cs = _class_find_by_name(p, className)) < 0)
                return NULL;

        NftPrefsClass *c = _class_get(p, cs);

        
        /* find object descriptor */
        /*NftPrefsObjSlot os;
        if((os = _obj_find_by_ptr(c, obj)) < 0)
                return NULL;

        NftPrefsObj *o = _obj_get(c, os);*/


        /* new node */
        NftPrefsNode *node;
        if(!(node = xmlNewNode(NULL, BAD_CAST c->name)))
                return NULL;

        /* call prefsFromObj() registered for this class */
        if(!c->fromObj(node, obj, userptr))
                return NULL;

        

        /* result pointer (xml dump) */
        char *dump = NULL;

        
        /* create buffer */
        xmlBufferPtr buf;
        if(!(buf = xmlBufferCreate()))
        {
                NFT_LOG(L_ERROR, "failed to xmlBufferCreate()");
                goto _potx_exit;
        }

        /* dump node */
        if(xmlNodeDump(buf, p->doc, node, 0, TRUE) < 0)
        {
                NFT_LOG(L_ERROR, "xmlNodeDump() failed");
                goto _potx_exit;
        }

        /* allocate buffer */
        if(!(dump = malloc(xmlBufferLength(buf)+1)))
        {
                NFT_LOG_PERROR("malloc()");
                goto _potx_exit;
        }

        /* copy buffer */
        strncpy(dump, (char *) xmlBufferContent(buf), xmlBufferLength(buf));
        dump[xmlBufferLength(buf)] = '\0';

_potx_exit:
        xmlBufferFree(buf);
        xmlFreeNode(node);
        
        return dump;
}


/**
 * create preferences file from current state of object
 *
 * @param p NftPrefs context
 * @param className name of class this object belongs to
 * @param obj the object to snapshot
 * @param filename full path of file to write to
 * @param userptr arbitrary function that will be passed to NftPrefsToObjFunc
 * @result newly created object or NULL
 */
NftResult nft_prefs_obj_to_file(NftPrefs *p, const char *className, void *obj, const char *filename, void *userptr)
{
        if(!p || !className || !obj)
                NFT_LOG_NULL(NFT_FAILURE);

        /* find object class */
        NftPrefsClassSlot cs;
        if((cs = _class_find_by_name(p, className)) < 0)
                return NFT_FAILURE;

        NftPrefsClass *c = _class_get(p, cs);

        
        /* find object descriptor */
        /*NftPrefsObjSlot os;
        if((os = _obj_find_by_ptr(c, obj)) < 0)
                return NFT_FAILURE;

        NftPrefsObj *o = _obj_get(c, os);*/


        /* new node */
        NftPrefsNode *node;
        if(!(node = xmlNewNode(NULL, BAD_CAST c->name)))
                return NFT_FAILURE;

        /* set name of node */
        xmlNodeSetName(node, BAD_CAST className);
        
        /* overall result */
        NftResult r = NFT_FAILURE;
        xmlDoc *d = NULL;
        
        /* call prefsFromObj() registered for this class */
        if(!c->fromObj(node, obj, userptr))
        {
                NFT_LOG(L_ERROR, "Object failed to create it's prefs");
                goto _potb_exit;
        }
        
        /* create temp xmlDoc */
        if(!(d = xmlNewDoc(BAD_CAST "1.0")))
        {
                NFT_LOG(L_ERROR, "Failed to create new XML doc");
                goto _potb_exit;
        }
        
        /* set node as root element of temporary doc */
        xmlDocSetRootElement(d, node);
                
        /* write document to file */
        if(xmlSaveFormatFileEnc(filename, d, "UTF-8", 1) < 0)
        {
                NFT_LOG(L_ERROR, "Failed to save XML file \"%s\"", filename);
                goto _potb_exit;
        }
        
        /* successfully written file */
        r = NFT_SUCCESS;

        
_potb_exit:
        /* free node */
        xmlUnlinkNode(node);
        xmlFreeNode(node);

        /* free temporary xmlDoc */
        if(d)
                xmlFreeDoc(d);
        
        return r;
}


/**
 * create new object from preferences buffer
 *
 * @param p NftPrefs context
 * @param buffer XML buffer
 * @param bufsize size of XML buffer
 * @param userptr arbitrary function that will be passed to NftPrefsToObjFunc
 * @result newly created object or NULL
 */
void *nft_prefs_obj_from_buffer(NftPrefs *p, char *buffer, size_t bufsize, void *userptr)
{
        if(!p || !buffer)
                NFT_LOG_NULL(NULL);

        
        /* parse XML */
        xmlDocPtr doc;
        if(!(doc = xmlReadMemory(buffer, bufsize, NULL, NULL, 0)))
        {
                NFT_LOG(L_ERROR, "Failed to xmlReadMemory()");
                return NULL;
        }

        
        /* create object */        
        NftPrefsObj *o;
        if(!(o = _obj_from_doc(p, doc, userptr)))
        {
                NFT_LOG(L_ERROR, "Failed to create object");
                return NULL;
        }

        /* free old doc? */
        if(p->doc)
                xmlFreeDoc(p->doc);

        /* save new doc */
        p->doc = doc;
        
        return o->object;
}


/**
 * create new object from preferences file
 *
 * @param p NftPrefs context
 * @param filename full path of file
 * @param userptr arbitrary function that will be passed to NftPrefsToObjFunc
 * @result newly created object or NULL
 */
void *nft_prefs_obj_from_file(NftPrefs *p, const char *filename, void *userptr)
{
        if(!p || !filename)
                NFT_LOG_NULL(NULL);

        
        /* parse XML */
        xmlDocPtr doc;
        if(!(doc = xmlReadFile(filename, NULL, 0)))
        {
		NFT_LOG(L_ERROR, "Failed to xmlReadFile(\"%s\")", filename);
                return NULL;
        }


        /* create object */        
        NftPrefsObj *o;
        if(!(o = _obj_from_doc(p, doc, userptr)))
        {
                NFT_LOG(L_ERROR, "Failed to create object");
                return NULL;
        }

        /* free old doc? */
        if(p->doc)
                xmlFreeDoc(p->doc);

        /* save new doc */
        p->doc = doc;
        
        return o->object;
}


/**
 * @}
 */
