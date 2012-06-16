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
 * @file node.c
 */

/**
 * @addtogroup prefs_node
 * @{
 *
 */

#include <niftylog.h>
#include "prefs.h"
#include "class.h"




/**
 * add current node as child of parent node
 *
 * @param parent parent node
 * @param cur child node
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_node_add_child(NftPrefsNode *parent, NftPrefsNode *cur)
{
        return xmlAddChild(parent, cur) ? NFT_SUCCESS : NFT_FAILURE;
}


/**
 * get first child of a node
 *
 * @param n parent node
 * @result child node or NULL
 */
NftPrefsNode *nft_prefs_node_get_first_child(NftPrefsNode *n)
{
        return xmlFirstElementChild(n);
}


/**
 * get next sibling of a node
 *
 * @param n node from which sibling should be fetched
 * @result sibling node or NULL
 */
NftPrefsNode *nft_prefs_node_get_next(NftPrefsNode *n)
{
        return xmlNextElementSibling(n);
}


/**
 * get name of this NftPrefsNode
 *
 * @result classname of this NftPrefsNode or NULL
 */ 
const char *nft_prefs_node_get_name(NftPrefsNode *n)
{
	if(!n)
		NFT_LOG_NULL(NULL);

    	return (const char *) n->name;
}


/**
 * set string property of a node
 * @param n node where property should be set
 * @param name name of property
 * @param value string-value of property
 */
NftResult nft_prefs_node_prop_string_set(NftPrefsNode *n, const char *name, char *value)
{
	if(!n || !name || !value)
		NFT_LOG_NULL(NFT_FAILURE);

	if(!xmlSetProp(n, (xmlChar *) name, (xmlChar *) value))
	{
		NFT_LOG(L_DEBUG, "Failed to set property \"%s\" = \"%s\"", name, value);
		return NFT_FAILURE;
	}

	return NFT_SUCCESS;
}


/**
 * get string property
 *
 * @param n NftPrefsNode to get string property from
 * @param name name of property
 * @result string with value or NULL (free using nft_prefs_free())
 */
char *nft_prefs_node_prop_string_get(NftPrefsNode *n, const char *name)
{
        if(!n || !name)
                NFT_LOG_NULL(NULL);

        return (char *) xmlGetProp(n, BAD_CAST name);
}


/**
 * set integer property
 *
 * @param n node where property should be set
 * @param name name of property
 * @param val value of property
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_node_prop_int_set(NftPrefsNode *n, const char *name, int val)
{
        if(!n || !name)
                NFT_LOG_NULL(NFT_FAILURE);

        char *tmp;
        if(!(tmp = alloca(32)))
        {
                NFT_LOG_PERROR("alloca()");
                return NFT_FAILURE;
        }

        if(snprintf(tmp, 32, "%d", val) < 0)
        {
                NFT_LOG_PERROR("snprintf()");
                return NFT_FAILURE;
        }
        
        return nft_prefs_node_prop_string_set(n, name, tmp);
}


/**
 * get integer property
 *
 * @param n node to read property from
 * @param name name of property
 * @param val space for value of property
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_node_prop_int_get(NftPrefsNode *n, const char *name, int *val)
{
        if(!n || !name || !val)
                NFT_LOG_NULL(NFT_FAILURE);

        char *tmp;
        if(!(tmp = nft_prefs_node_prop_string_get(n, name)))
        {
                NFT_LOG(L_DEBUG, "property \"%s\" not found in <%s>", name, n->name);
                return NFT_FAILURE;
        }

	NftResult result = NFT_SUCCESS;
        if(sscanf(tmp, "%d", val) != 1)
        {
                NFT_LOG(L_ERROR, "sscanf() failed");
                result = NFT_FAILURE;
        }

	nft_prefs_free(tmp);
	
        return result;
}


/**
 * set double property
 *
 * @param n node where property should be set
 * @param name name of property
 * @param val value of property
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_node_prop_double_set(NftPrefsNode *n, const char *name, double val)
{
        if(!n || !name)
                NFT_LOG_NULL(NFT_FAILURE);

        char *tmp;
        if(!(tmp = alloca(32)))
        {
                NFT_LOG_PERROR("alloca()");
                return NFT_FAILURE;
        }

        if(snprintf(tmp, 32, "%lf", val) < 0)
        {
                NFT_LOG_PERROR("snprintf()");
                return NFT_FAILURE;
        }
        
        return nft_prefs_node_prop_string_set(n, name, tmp);
}


/**
 * get double property
 *
 * @param n node to read property from
 * @param name name of property
 * @param val space for value of property
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_node_prop_double_get(NftPrefsNode *n, const char *name, double *val)
{
        if(!n || !name || !val)
                NFT_LOG_NULL(NFT_FAILURE);

        char *tmp;
        if(!(tmp = nft_prefs_node_prop_string_get(n, name)))
        {
                NFT_LOG(L_DEBUG, "property \"%s\" not found in <%s>", name, n->name);
                return NFT_FAILURE;
        }

	NftResult result = NFT_SUCCESS;
        if(sscanf(tmp, "%lf", val) != 1)
        {
                NFT_LOG(L_ERROR, "sscanf() failed");
                result = NFT_FAILURE;
        }

	nft_prefs_free(tmp);
	
        return result;    
}


/**
 * create preferences buffer from NftPrefsNode
 * 
 * @param p NftPrefs context
 * @param n NftPrefsNode
 * @result string holding xml representation of object (use free() to deallocate)
 */
char *nft_prefs_node_to_buffer(NftPrefs *p, NftPrefsNode *n)
{
    	if(!p || !n)
		NFT_LOG_NULL(NULL);
    
	/* result pointer (xml dump) */
        char *dump = NULL;

        /* create buffer */
        xmlBufferPtr buf;
        if(!(buf = xmlBufferCreate()))
        {
                NFT_LOG(L_ERROR, "failed to xmlBufferCreate()");
                return NULL;
        }

        /* dump node */
        if(xmlNodeDump(buf, prefs_doc(p), n, 0, TRUE) < 0)
        {
                NFT_LOG(L_ERROR, "xmlNodeDump() failed");
                goto _pntb_exit;
        }

        /* allocate buffer */
        if(!(dump = malloc(xmlBufferLength(buf)+1)))
        {
                NFT_LOG_PERROR("malloc()");
                goto _pntb_exit;
        }

        /* copy buffer */
        strncpy(dump, (char *) xmlBufferContent(buf), xmlBufferLength(buf));
        dump[xmlBufferLength(buf)] = '\0';

_pntb_exit:
        xmlBufferFree(buf);
        
        return dump;
}


/**
 * create preferences file from NftPrefsNode and child nodes
 *
 * @param p NftPrefs context
 * @param n NftPrefsNode
 * @param filename full path of file to be written 
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_node_to_file(NftPrefs *p, NftPrefsNode *n, const char *filename)
{
    	if(!p || !n || !filename)
		NFT_LOG_NULL(NFT_FAILURE);


        
        /* create temp xmlDoc */
        xmlDoc *d = NULL;
        if(!(d = xmlNewDoc(BAD_CAST "1.0")))
        {
                NFT_LOG(L_ERROR, "Failed to create new XML doc");
                return NFT_FAILURE;
        }
           
	/* overall result */
        NftResult r = NFT_FAILURE;
    
        /* set node as root element of temporary doc */
        xmlDocSetRootElement(d, n);
                
        /* write document to file */
        if(xmlSaveFormatFileEnc(filename, d, "UTF-8", 1) < 0)
        {
                NFT_LOG(L_ERROR, "Failed to save XML file \"%s\"", filename);
                goto _potb_exit;
        }
        
        /* successfully written file */
        r = NFT_SUCCESS;

        
_potb_exit:
        /* free temporary xmlDoc */
        if(d)
                xmlFreeDoc(d);
        
        return r;
}


/**
 * create new NftPrefsNode from preferences file
 *
 * @param p NftPrefs context
 * @param filename full path of file
 * @result newly created NftPrefsNode or NULL
 */
NftPrefsNode *nft_prefs_node_from_file(NftPrefs *p, const char *filename)
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

       	/* parse XInclude stuff */
        int xinc_res;
        if((xinc_res = xmlXIncludeProcess(doc)) == -1)
        {
                NFT_LOG(L_ERROR, "XInclude parsing failed.");
                return NFT_FAILURE;
        }
        NFT_LOG(L_DEBUG, "%d XInclude substitutions done", xinc_res);
    
        /* free old doc? */
        if(prefs_doc(p))
        {
                xmlFreeDoc(prefs_doc(p));
        }
        
        /* save new doc */
        prefs_doc_set(p,doc);
    
        /* get node */
        xmlNode *node;
        if(!(node = xmlDocGetRootElement(doc)))
        {
                NFT_LOG(L_ERROR, "No root element found in XML");
                return NULL;
        }

        return node;
}


/**
 * create new NftPrefsNode from preferences buffer
 *
 * @param p NftPrefs context
 * @param buffer XML buffer to parse
 * @param bufsize size of XML buffer
 * @result newly created NftPrefsNode or NULL
 */
NftPrefsNode *nft_prefs_node_from_buffer(NftPrefs *p, char *buffer, size_t bufsize)
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

    	/* parse XInclude stuff */
        int xinc_res;
        if((xinc_res = xmlXIncludeProcess(doc)) == -1)
        {
                NFT_LOG(L_ERROR, "XInclude parsing failed.");
                return NFT_FAILURE;
        }
        NFT_LOG(L_DEBUG, "%d XInclude substitutions done", xinc_res);
    
        /* free old doc? */
        if(prefs_doc(p))
                xmlFreeDoc(prefs_doc(p));

        /* save new doc */
        prefs_doc_set(p, doc);
    
        /* get node */
        xmlNode *node;
        if(!(node = xmlDocGetRootElement(doc)))
        {
                NFT_LOG(L_ERROR, "No root element found in XML");
                return NULL;
        }

	return node;
}


/**
 * allocate a new custom NftPrefsNode
 *
 * @param name the name of the node
 * @result freshly created NftPrefsNode
 * @note use nft_prefs_node_free() if node is not used anymore
 */
NftPrefsNode *nft_prefs_node_alloc(const char *name)
{
	if(!name)
		NFT_LOG_NULL(NULL);

    	return xmlNewNode(NULL, BAD_CAST name);
}


/**
 * free resources of a NftPrefsNode
 *
 * @param n NftPrefsNode to free
 */
void nft_prefs_node_free(NftPrefsNode *n)
{
	if(!n)
		NFT_LOG_NULL();

    	xmlUnlinkNode(n);
    	xmlFreeNode(n);
}


/**
 * @}
 */
