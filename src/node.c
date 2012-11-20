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
 *
 * @todo fix xmlDocs memory leaked when saved in xmlNode->_private (free when last node is freed)
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
/**************************** PRIVATE FUNCTIONS *******************************/
/******************************************************************************/

/******************************************************************************/
/**************************** STATIC FUNCTIONS ********************************/
/******************************************************************************/

/** set private pointer in a node recursively */
void _set_private_recursively(NftPrefsNode * node, void *ptr)
{
        NftPrefsNode *n;
        for(n = node; n; n = nft_prefs_node_get_next(n))
        {
                NftPrefsNode *nc;
                if((nc = nft_prefs_node_get_first_child(n)))
                        _set_private_recursively(nc, ptr);

                n->_private = ptr;
        }
}

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
 * set string property of a node
 * @param n node where property should be set
 * @param name name of property
 * @param value string-value of property
 */
NftResult nft_prefs_node_prop_string_set(NftPrefsNode * n, const char *name,
                                         char *value)
{
        if(!n || !name || !value)
                NFT_LOG_NULL(NFT_FAILURE);

        if(!xmlSetProp(n, (xmlChar *) name, (xmlChar *) value))
        {
                NFT_LOG(L_DEBUG, "Failed to set property \"%s\" = \"%s\"",
                        name, value);
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
char *nft_prefs_node_prop_string_get(NftPrefsNode * n, const char *name)
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
NftResult nft_prefs_node_prop_int_set(NftPrefsNode * n, const char *name,
                                      int val)
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
NftResult nft_prefs_node_prop_int_get(NftPrefsNode * n, const char *name,
                                      int *val)
{
        if(!n || !name || !val)
                NFT_LOG_NULL(NFT_FAILURE);

        char *tmp;
        if(!(tmp = nft_prefs_node_prop_string_get(n, name)))
        {
                NFT_LOG(L_DEBUG, "property \"%s\" not found in <%s>", name,
                        n->name);
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
NftResult nft_prefs_node_prop_double_set(NftPrefsNode * n, const char *name,
                                         double val)
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
NftResult nft_prefs_node_prop_double_get(NftPrefsNode * n, const char *name,
                                         double *val)
{
        if(!n || !name || !val)
                NFT_LOG_NULL(NFT_FAILURE);

        char *tmp;
        if(!(tmp = nft_prefs_node_prop_string_get(n, name)))
        {
                NFT_LOG(L_DEBUG, "property \"%s\" not found in <%s>", name,
                        n->name);
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
 * set boolean property
 *
 * @param n node where property should be set
 * @param name name of property
 * @param val value of property
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_node_prop_boolean_set(NftPrefsNode * n, const char *name,
                                          bool val)
{
        if(!n || !name)
                NFT_LOG_NULL(NFT_FAILURE);

        char *tmp;
        if(!(tmp = alloca(16)))
        {
                NFT_LOG_PERROR("alloca()");
                return NFT_FAILURE;
        }

        if(snprintf(tmp, 16, "%s", val ? "true" : "false") < 0)
        {
                NFT_LOG_PERROR("snprintf()");
                return NFT_FAILURE;
        }

        return nft_prefs_node_prop_string_set(n, name, tmp);
}


/**
 * get boolean property
 *
 * @param n node to read property from
 * @param name name of property
 * @param val space for value of property
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_node_prop_boolean_get(NftPrefsNode * n, const char *name,
                                          bool * val)
{
        if(!n || !name || !val)
                NFT_LOG_NULL(NFT_FAILURE);

        xmlChar *tmp;
        if(!(tmp = (xmlChar *) nft_prefs_node_prop_string_get(n, name)))
        {
                NFT_LOG(L_DEBUG, "property \"%s\" not found in <%s>", name,
                        n->name);
                return NFT_FAILURE;
        }

        if(xmlStrcasecmp(tmp, BAD_CAST "true") == 0 ||
           xmlStrcasecmp(tmp, BAD_CAST "yes") == 0 ||
           xmlStrcasecmp(tmp, BAD_CAST "on") == 0 ||
           xmlStrcasecmp(tmp, BAD_CAST "enable") == 0 ||
           xmlStrcasecmp(tmp, BAD_CAST "1") == 0)
        {
                *val = true;
        }
        else
        {
                *val = false;
        }

        nft_prefs_free(tmp);

        return NFT_SUCCESS;
}


/**
 * create preferences buffer from NftPrefsNode
 *
 * @param n NftPrefsNode
 * @result string holding xml representation of object (use free() to deallocate)
 * @note s. @ref nft_prefs_node_to_file for description
 */
char *nft_prefs_node_to_buffer_light(NftPrefsNode * n)
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
        if(xmlNodeDump(buf, n->_private, n, 0, TRUE) < 0)
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
 * create preferences buffer with all format specific encapsulation from a NftPrefsNode
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
 * This will create the same output as nft_prefs_node_to_file_light() would but
 * with all encapsulation/headers/footers/... of the underlying prefs mechanism.
 * e.g. for XML this adds the "<?xml version="1.0" encoding="UTF-8"?>" header.
 * This is used when one needs a complete configuration (e.g. saved preferences file)
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
NftResult nft_prefs_node_to_file_light(NftPrefsNode * n, const char *filename,
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
        if(xmlNodeDump(buf, n->_private, n, 0, TRUE) < 0)
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

        /* save document in node & all child nodes */
        _set_private_recursively(node, doc);

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

        /* save document in node & all child nodes */
        _set_private_recursively(node, doc);

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

        xmlUnlinkNode(n);
        xmlFreeNode(n);
}


/**
 * get filename of document this node was parsed from (or NULL)
 *
 * @result URL of node origin or NULL if unset 
 */
const char *nft_prefs_node_get_filename(NftPrefsNode * n)
{
        if(!n || !n->_private)
                NFT_LOG_NULL(NULL);

        xmlDoc *doc = n->_private;

        return (const char *) doc->URL;
}


/**
 * @}
 */
