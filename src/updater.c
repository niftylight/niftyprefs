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
 * @file updater.c
 */

/**
 * @addtogroup prefs_updater
 * @{
 *
 */


#include <niftylog.h>
#include "prefs.h"
#include "class.h"



#define VERSION_PROP      "version"



/** describes one NftPrefsUpdater function */
struct _NftPrefsUpdater
{
	/** function that updates an NftPrefsNode */
	NftPrefsUpdaterFunc *updater;
	/** name of object class */
    char className[NFT_PREFS_MAX_CLASSNAME + 1];
	/** version to update to */
	unsigned int version;
	/** user pointer */
	void *userptr;
};




/******************************************************************************/
/**************************** STATIC FUNCTIONS ********************************/
/******************************************************************************/

/** criterion for nft_array_find_slot() */
static bool _finder_by_version(void *element,
                                      void *criterion,
                                      void *userptr)
{
        if(!element || !criterion)
                NFT_LOG_NULL(false);


        NftPrefsUpdater *u = element;
		unsigned int *version = criterion;

        return (*version == u->version);
}


/** find updater for a specific version */
static NftPrefsUpdater *_find_updater(NftPrefsUpdaters *updaters, 
                                      unsigned int version)
{
        /* find updater in array */
        NftArraySlot slot;
        if(!nft_array_find_slot(updaters, &slot, _finder_by_version,
                                (void *) &version, NULL))
        {
                return NULL;
        }

        return nft_array_get_element(updaters, slot);
}


/** get version of node */
static NftResult _get_version(NftPrefsNode *node, unsigned int *version)
{
	if(!nft_prefs_node_prop_int_get(node, VERSION_PROP, (int *) version))
	{
			NFT_LOG(L_DEBUG, "node \"%s\" has no \"%s\" property...",
			        nft_prefs_node_get_name(node), VERSION_PROP);
			return NFT_FAILURE;
	}

	return NFT_SUCCESS;
}


/** run updater for node, all siblings and all child nodes */
static NftResult _update_node(NftPrefs *p, NftPrefsNode *node,
                              unsigned int fromVersion, unsigned int toVersion)
{
		/* update this node and all siblings */
		NftPrefsNode *n;
		for(n = node; n; n = nft_prefs_node_get_next(n))
		{

				/* find class */
				NftPrefsClass *c;
				if(!(c = _class_find_by_name(_prefs_classes(p),
											 nft_prefs_node_get_name(n))))
				{
						NFT_LOG(L_DEBUG, "Unknown prefs class \"%s\". Skipping...",
								nft_prefs_node_get_name(n));

						continue;
				}

				NFT_LOG(L_NOTICE, "Preferences older than context. Trying to update...");

				/* update version succesively */
				for(unsigned int v = fromVersion; v < toVersion; v++)
				{
						/* find updater */
						NftPrefsUpdater *u;
						if(!(u = _find_updater(_class_updaters(c), v)))
						{
								continue;
						}

						NFT_LOG(L_DEBUG, "Found updater function for "
									"class \"%s\" version \"%d\"",
									u->className, u->version);

						/* run updater */
						if(!(u->updater(n, v, u->userptr)))
						{
								NFT_LOG(L_ERROR, "Update for class \"%s\" "
											"(from version %d) failed!",
											u->className, v);
								return NFT_FAILURE;
						}

						NFT_LOG(L_NOTICE, "Node \"%s\" successfully "
									"updated to version %d",
									u->className, v);
				}

				/* update all child nodes */
				NftPrefsNode *nc;
				if((nc = nft_prefs_node_get_first_child(n)))
				{
							if(!(_update_node(p, nc, fromVersion, toVersion)))
							{
									return NFT_FAILURE;
							}
				}
		}

		return NFT_SUCCESS;
}



/******************************************************************************/
/**************************** PRIVATE FUNCTIONS *******************************/
/******************************************************************************/

/** initialize array to store updaters */
NftResult _updater_init_array(NftPrefsUpdaters * a)
{
		/* initialize class-array */
        return nft_array_init(a, sizeof(NftPrefsUpdater));
}


/** update a node that has a version property,
    all its siblings & all child nodes recursively */
NftResult _updater_node_process(NftPrefs *p, NftPrefsNode *node)
{
		/* get version of context */
		unsigned int contextVersion = _prefs_get_version(p);

		/* get version of node */
		unsigned int nodeVersion = 0;
		if(!_get_version(node, &nodeVersion))
		{
				/* no version found */
				return NFT_SUCCESS;
		}

		/* versions differ? */
		if(contextVersion == nodeVersion)
		{
				/* same version - no update required */
				return NFT_SUCCESS;
		}

		/* our preferences are too new */
		if(contextVersion < nodeVersion)
		{
				NFT_LOG(L_WARNING, "Preferences are newer than we are! "
				        "Please update application. "
				        "Trying to continue anyway...");
				return NFT_SUCCESS;
		}


		/* update node and all child nodes */
		if(!_update_node(p, node, nodeVersion, contextVersion))
		{
				return NFT_FAILURE;
		}

		return NFT_SUCCESS;
}


/** add version to NftPrefsNode */
NftResult _updater_node_add_version(NftPrefs *p, NftPrefsNode *node)
{
		if(!p || !node)
				NFT_LOG_NULL(NFT_FAILURE);

		/* set version property */
		if(!(nft_prefs_node_prop_int_set(node, VERSION_PROP,
		                                 _prefs_get_version(p))))
				return NFT_FAILURE;

		return NFT_SUCCESS;
}


/** remove version from NftPrefsNode */
void _updater_node_remove_version(NftPrefsNode *node)
{
		if(!node)
				NFT_LOG_NULL();

		/* unset version property */
		nft_prefs_node_prop_unset(node, VERSION_PROP);
}



/******************************************************************************/
/**************************** API FUNCTIONS ***********************************/
/******************************************************************************/

/**
 * register function to update a node of a certain class automatically in case
 * it's format changed. It is tried to call an update handler for each version,
 * beginning from the version of the node up to the version of the current
 * context (the one that was passed to nft_prefs_init()). If a (root) node
 * doesn't have a NFT_PREFS_VERSION ("version") property, it's supposed that the
 * node has the version 0.
 *
 * @param p NftPrefs context
 * @param updater NftPrefsUpdaterFunc update handler
 * @param className class of nodes this updater will handle
 * @param version update nodes with this version (to version+1)
 * @param userptr arbitrary pointer that will be passed to the updater function
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_updater_register(NftPrefs *p,
                                     NftPrefsUpdaterFunc *updater,
                                     const char *className,
                                     unsigned int version, void *userptr)
{
	if(!p || !updater || !className)
			NFT_LOG_NULL(NFT_FAILURE);


	/* get class */
	NftPrefsClass *c;
	if(!(c = _class_find_by_name(_prefs_classes(p), className)))
	{
			NFT_LOG(L_ERROR, "Class \"%s\" not registered", className);
			return NFT_FAILURE;
	}

	/* allocate new slot in updater array */
    NftArraySlot s;
    if(!(nft_array_slot_alloc(_class_updaters(c), &s)))
    {
            NFT_LOG(L_ERROR, "Failed to allocate new array slot");
            return NFT_FAILURE;
    }

    /* get newly allocated empty array element */
    NftPrefsUpdater *n;
    if(!(n = nft_array_get_element(_class_updaters(c), s)))
    {
            NFT_LOG(L_ERROR, "Failed to get element from array slot");
			nft_array_slot_free(_class_updaters(c), s);
            return NFT_FAILURE;
    }

	/* register updater */
	n->updater = updater;
	n->version = version;
	n->userptr = userptr;
	strncpy(n->className, className, NFT_PREFS_MAX_CLASSNAME);

	return NFT_SUCCESS;
}



/**
 * @}
 */
