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


/** a node that holds various properties about an object */
struct _PrefsNode
{
    /** name of object class */
    const char *className;
    /** libxml node */
    xmlNode *node;
    /** object (NULL to describe a complete object class) */
    void *object;
    /** callback to create a new object from preferences */
    NftPrefsToObjFunc *toObj;
    /** callback to create preferences from the current object state */
    NftPrefsFromObjFunc *fromObj;
};


/** context descriptor */
static struct
{
    /** list of defined PrefsNodes */
    NftPrefsNode *nodes;
    /** amount of entries stored in list */
	size_t from_obj_elements;
	/** length of objects list */
	size_t from_obj_length;
}_c;


/******************************************************************************/
/**************************** STATIC FUNCTIONS ********************************/
/******************************************************************************/


/** 
 * libxml error handler 
 */
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


/******************************************************************************/
/**************************** API FUNCTIONS ***********************************/
/******************************************************************************/


/**
 * initialize libniftyprefs - call this once before doing any other API call
 *
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_init()
{
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

        return NFT_SUCCESS;
}


/**
 * deinitialize libniftyprefs - call this after doing the last API call to
 * finally clean up
 */
void nft_prefs_exit()
{
    /* free object-list */
	free(_c.nodes);
        
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
NftResult nft_prefs_obj_class_register(const char *className, 
                                       NftPrefsToObjFunc *toObj, 
                                       NftPrefsFromObjFunc *fromObj)
{
        return NFT_SUCCESS;
}


void nft_prefs_obj_class_unregister(const char *className)
{

}


NftResult nft_prefs_obj_register(const char *className, void *obj)
{
        return NFT_SUCCESS;
}


void nft_prefs_obj_unregister(void *obj)
{

}


NftResult nft_prefs_obj_to_prefs(NftPrefsNode **node, void *obj)
{
        return NFT_SUCCESS;
}


NftResult nft_prefs_obj_from_prefs(const NftPrefsNode *node, void **obj)
{
        return NFT_SUCCESS;
}


/**
 * @}
 */
