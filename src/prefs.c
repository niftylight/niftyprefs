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
 * @file prefs.c
 */

/**
 * @addtogroup prefs
 * @{
 *
 */


#include <niftylog.h>
#include "niftyprefs.h"
#include "class.h"
#include "config.h"




/** context descriptor */
struct _NftPrefs
{
        /** temporary xmlDoc */
        xmlDoc *doc;
        /** list of registered PrefsObjClasses */
        NftPrefsClasses classes;
};





/******************************************************************************/
/**************************** PRIVATE FUNCTIONS *******************************/
/******************************************************************************/

/** getter */ 
NftPrefsClasses *prefs_classes(NftPrefs *p)
{
    return &p->classes;
}


/** getter */
xmlDoc *prefs_doc(NftPrefs *p)
{
	return p->doc;
}


/** setter */
void prefs_doc_set(NftPrefs *p, xmlDoc *doc)
{
	p->doc = doc;
}



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


/** helper for nft_array_foreach_element() */
bool _class_free(void *element, void *userptr)
{
    	NftPrefs *p = userptr;
    
	prefs_class_free(p, (NftPrefsClass *) element);
    	return TRUE;
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
        
        /*
         * this initializes the library and check potential ABI mismatches
         * between the version it was compiled for and the actual shared
         * library used.
         */
        NFT_PREFS_CHECK_VERSION
                
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

        if(!prefs_class_init_array(&p->classes))
    	{
		NFT_LOG(L_ERROR, "Failed to init class array");
		free(p);
		return NULL;
	}
    
        return p;
}


/**
 * deinitialize libniftyprefs - call this after doing the last API call to
 * finally clean up
 *
 * @param p NftPrefs context
 */
void nft_prefs_deinit(NftPrefs *p)
{
        /* free xmlDoc */
        if(p->doc)
        {
                xmlFreeDoc(p->doc);
                p->doc = NULL;
        }
        
        /* free all classes */
    	nft_array_foreach_element(&p->classes, _class_free, p);

    	/* free classes array */
    	nft_array_deinit(&p->classes);
            
        /* free xmlDoc */
        if(p->doc)
                xmlFreeDoc(p->doc);

        /* free descriptor */
        free(p);
        
        /* cleanup XML parser */
	xmlCleanupParser();
}


/**
 * wrapper for xmlFree()
 *
 * @param p pointer to memory previously allocated by niftyprefs
 */
void nft_prefs_free(void *p)
{
        if(p)
                xmlFree(p);
}









/**
 * @}
 */
