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
 * @file node.c
 */

/**
 * @addtogroup prefs_node
 * @{
 *
 */

#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
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
 * add current node as child of parent node
 *
 * @param parent parent node
 * @param cur child node
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_node_add_child(NftPrefsNode * parent, NftPrefsNode * cur)
{
		if(!parent)
				NFT_LOG_NULL(NFT_FAILURE);

        return xmlAddChild(parent, cur) ? NFT_SUCCESS : NFT_FAILURE;
}


/**
 * get first child of a node
 *
 * @param n parent node
 * @result child node or NULL
 */
NftPrefsNode *nft_prefs_node_get_first_child(NftPrefsNode * n)
{
		if(!n)
				NFT_LOG_NULL(NULL);

        return xmlFirstElementChild(n);
}


/**
 * get next sibling of a node
 *
 * @param n node from which sibling should be fetched
 * @result sibling node or NULL
 */
NftPrefsNode *nft_prefs_node_get_next(NftPrefsNode * n)
{
        return xmlNextElementSibling(n);
}


/**
 * get next sibling of a node with a certain name
 *
 * @param n node from which sibling should be fetched
 * @param name name of sibling node
 * @result sibling node or NULL
 */
NftPrefsNode *nft_prefs_node_get_next_with_name(NftPrefsNode * n, const char *name)
{
		if(!n || !name)
				NFT_LOG_NULL(NULL);

		for(NftPrefsNode *sibling = nft_prefs_node_get_next(n);
		    sibling;
		    sibling = nft_prefs_node_get_next(sibling))
		{
				if(strcmp(nft_prefs_node_get_name(sibling), name) == 0)
				{
						return sibling;
				}
		}

		return NULL;
}


/**
 * get name of this NftPrefsNode
 *
 * @result classname of this NftPrefsNode or NULL
 */
const char *nft_prefs_node_get_name(NftPrefsNode * n)
{
        if(!n)
                NFT_LOG_NULL(NULL);

        return (const char *) n->name;
}


/**
 * create preferences minimal buffer from NftPrefsNode - compared to
 * nft_prefs_node_to_buffer, this doesn't include any encapsulation or headers.
 * Just the bare information contained in the node. This should be used to
 * export single nodes, e.g. for copying them to a clipboard 
 *
 * @param n NftPrefsNode
 * @result string holding xml representation of object (use free() to deallocate)
 * @note s. @ref nft_prefs_node_to_file for description
 */
char *nft_prefs_node_to_buffer_minimal(NftPrefsNode * n)
{
        if(!n)
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
        if(xmlNodeDump(buf, n->doc, n, 0, true) < 0)
        {
                NFT_LOG(L_ERROR, "xmlNodeDump() failed");
                goto _pntb_exit;
        }

        /* allocate buffer */
        size_t length = xmlBufferLength(buf);
        if(!(dump = malloc(length + 1)))
        {
                NFT_LOG_PERROR("malloc()");
                goto _pntb_exit;
        }

        /* copy buffer */
        strncpy(dump, (char *) xmlBufferContent(buf), length);
        dump[length] = '\0';

_pntb_exit:
        xmlBufferFree(buf);

        return dump;
}


/**
 * create preferences buffer with all format specific encapsulation from a 
 * NftPrefsNode. This is used when one needs a complete configuration.
 * 
 * @param n NftPrefsNode
 * @result string holding xml representation of object (use free() to deallocate)
 * @note s. @ref nft_prefs_node_to_file for description
 */
char *nft_prefs_node_to_buffer(NftPrefsNode * n)
{
        if(!n)
                NFT_LOG_NULL(NFT_FAILURE);

        /* create copy of node */
        NftPrefsNode *copy;
        if(!(copy = xmlCopyNode(n, 1)))
                return NULL;

        /* create temp xmlDoc */
        xmlDoc *d = NULL;
        if(!(d = xmlNewDoc(BAD_CAST "1.0")))
        {
                NFT_LOG(L_ERROR, "Failed to create new XML doc");
                xmlFreeNode(copy);
                return NULL;
        }

        /* set node as root element of temporary doc */
        xmlDocSetRootElement(d, copy);

        /* overall result */
        char *r = NULL;
        xmlChar *dump = NULL;
        int length = 0;

        /* dump document to buffer */
        xmlDocDumpFormatMemoryEnc(d, &dump, &length, "UTF-8", 1);
        if(!dump || length <= 0)
        {
                NFT_LOG(L_ERROR, "Failed to dump XML file");
                goto _pntbwh_exit;
        }

        if(!(r = malloc(length + 1)))
        {
                NFT_LOG_PERROR("malloc");
                goto _pntbwh_exit;
        }

        memcpy(r, dump, length);
        r[length] = '\0';

_pntbwh_exit:
        /* free node */
        if(copy)
        {
                /* unlink node from document again */
                xmlUnlinkNode(copy);
                xmlFreeNode(copy);
        }

        /* free xml buffer */
        if(dump)
                xmlFree(dump);

        /* free temporary xmlDoc */
        if(d)
        {
                xmlFreeDoc(d);
        }

        return r;
}


/**
 * create  preferences file with headers from NftPrefsNode and child nodes
 *
 * This will create the same output as nft_prefs_node_to_file_minimal() would but
 * with all encapsulation/headers/footers/... of the underlying prefs mechanism.
 * e.g. for XML this adds the "<?xml version="1.0" encoding="UTF-8"?>" header.
 * This is used when one needs a complete configuration (e.g. saved preferences 
 * file)
 * 
 * @param n NftPrefsNode
 * @param filename full path of file to be written
 * @param overwrite if a file called "filename" already exists, it 
 * will be overwritten if this is "true", otherwise NFT_FAILURE will be returned 
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_node_to_file(NftPrefsNode * n, const char *filename,
                                 bool overwrite)
{
        if(!n || !filename)
                NFT_LOG_NULL(NFT_FAILURE);

        /* create copy of node */
        NftPrefsNode *copy;
        if(!(copy = xmlCopyNode(n, 1)))
                return NFT_FAILURE;

        /* create temp xmlDoc */
        xmlDoc *d = NULL;
        if(!(d = xmlNewDoc(BAD_CAST "1.0")))
        {
                NFT_LOG(L_ERROR, "Failed to create new XML doc");
                xmlFreeNode(copy);
                return NFT_FAILURE;
        }

        /* overall result */
        NftResult r = NFT_FAILURE;

        /* set node as root element of temporary doc */
        xmlDocSetRootElement(d, copy);

        /* file already existing? */
        struct stat sts;
        if(stat(filename, &sts) == -1)
        {
                /* continue if stat error was caused because file doesn't exist 
                 */
                if(errno != ENOENT)
                {
                        NFT_LOG(L_ERROR, "Failed to access \"%s\" - %s",
                                filename, strerror(errno));
                        goto _pntfwh_exit;
                }
        }
        /* stat succeeded, file exists */
        else if(strcmp("-", filename) != 0)
        {
                /* remove old file? */
                if(!overwrite)
                        goto _pntfwh_exit;

                /* delete existing file */
                if(unlink(filename) == -1)
                {
                        NFT_LOG(L_ERROR,
                                "Failed to remove old version of \"%s\" - %s",
                                filename, strerror(errno));
                        goto _pntfwh_exit;
                }
        }

        /* write document to file */
        if(xmlSaveFormatFileEnc(filename, d, "UTF-8", 1) < 0)
        {
                NFT_LOG(L_ERROR, "Failed to save XML file \"%s\"", filename);
                goto _pntfwh_exit;
        }

        /* successfully written file */
        r = NFT_SUCCESS;


_pntfwh_exit:
        if(copy)
        {
                /* unlink node from document again */
                xmlUnlinkNode(copy);
                xmlFreeNode(copy);
        }

        /* free temporary xmlDoc */
        if(d)
        {
                xmlFreeDoc(d);
        }

        return r;
}


/**
 * create preferences file from NftPrefsNode and child nodes
 *
 * This will create the same output as nft_prefs_node_to_file() would but
 * without all encapsulation/headers/footers/... of the underlying prefs mechanism.
 * e.g. for XML this omits the "<?xml version="1.0" encoding="UTF-8"?>" header.
 * This is used when one only needs an "incomplete" snippet of a configuration.
 * e.g. for copy/paste or to use the XInclude feature of XML
 * 
 * @param n NftPrefsNode
 * @param filename full path of file to be written
 * @param overwrite if a file called "filename" already exists, it 
 * will be overwritten if this is "true", otherwise NFT_FAILURE will be returned
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_node_to_file_minimal(NftPrefsNode * n, const char *filename,
                                       bool overwrite)
{
        if(!n || !filename)
                NFT_LOG_NULL(NFT_FAILURE);

        /* file already existing? */
        struct stat sts;
        if(stat(filename, &sts) == -1)
        {
                /* continue if stat error was caused because file doesn't exist 
                 */
                if(errno != ENOENT)
                {
                        NFT_LOG(L_ERROR, "Failed to access \"%s\" - %s",
                                filename, strerror(errno));
                        return NFT_FAILURE;
                }
        }
        /* stat succeeded, file exists */
        else if(strcmp("-", filename) != 0)
        {
                /* remove old file? */
                if(!overwrite)
                        return NFT_FAILURE;

                /* delete existing file */
                if(unlink(filename) == -1)
                {
                        NFT_LOG(L_ERROR,
                                "Failed to remove old version of \"%s\" - %s",
                                filename, strerror(errno));
                        return NFT_FAILURE;
                }
        }

        /* overall result */
        NftResult r = NFT_FAILURE;


        /* create buffer */
        xmlBufferPtr buf;
        if(!(buf = xmlBufferCreate()))
        {
                NFT_LOG(L_ERROR, "failed to xmlBufferCreate()");
                return NFT_FAILURE;
        }

        /* dump node */
        if(xmlNodeDump(buf, n->doc, n, 0, true) < 0)
        {
                NFT_LOG(L_ERROR, "xmlNodeDump() failed");
                goto _pntf_exit;
        }

        /* stdout? */
        int fd;
        if(strcmp("-", filename) == 0)
        {
                fd = STDOUT_FILENO;
        }
        /* open file */
        else
        {
#ifdef WIN32
                if((fd = open(filename,
                              O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) == -1)
#else
                if((fd = open(filename,
                              O_CREAT | O_WRONLY,
                              S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP)) == -1)
#endif
                {
                        NFT_LOG_PERROR("open");
                        goto _pntf_exit;
                }
        }

        /* write to file */
        ssize_t length = xmlBufferLength(buf);
        if(write(fd, (char *) xmlBufferContent(buf), length) != length)
        {
                NFT_LOG_PERROR("write");
                goto _pntf_exit;
        }

        if(fd != STDOUT_FILENO)
                close(fd);

        r = NFT_SUCCESS;

_pntf_exit:
        xmlBufferFree(buf);

        return r;
}


/**
 * create new NftPrefsNode from preferences file
 *
 * @param filename full path of file
 * @result newly created NftPrefsNode or NULL
 */
NftPrefsNode *nft_prefs_node_from_file(const char *filename)
{
        if(!filename)
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


        /* get node */
        xmlNode *node;
        if(!(node = xmlDocGetRootElement(doc)))
        {
                NFT_LOG(L_ERROR, "No root element found in XML");
                /* free resources of document */
                xmlFreeDoc(doc);
                return NULL;
        }

        return node;
}


/**
 * create new NftPrefsNode from preferences buffer
 *
 * @param buffer XML buffer to parse
 * @param bufsize size of XML buffer
 * @result newly created NftPrefsNode or NULL
 */
NftPrefsNode *nft_prefs_node_from_buffer(char *buffer, size_t bufsize)
{
        if(!buffer)
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
void nft_prefs_node_free(NftPrefsNode * n)
{
        if(!n)
                NFT_LOG_NULL();

        /* save document for later */
        xmlDoc *doc = n->doc;

        /* unlink node from doc */
        xmlUnlinkNode(n);

        /* free node */
        xmlFreeNode(n);

        /* check if node was the last in doc */
        if(doc && !doc->children)
        {
                xmlFreeDoc(doc);
        }
}


/**
 * get URI of document this node was parsed from (or NULL)
 *
 * @result URI of node origin or NULL if unset 
 */
const char *nft_prefs_node_get_uri(NftPrefsNode * n)
{
        if(!n)
                NFT_LOG_NULL(NULL);

		if(!n->doc)
		{
				NFT_LOG(L_DEBUG, "node \"%s\" has no uri set.",
				        nft_prefs_node_get_name(n));
				return NULL;
		}
	
        return (const char *) n->doc->URL;
}


/**
 * @}
 */
