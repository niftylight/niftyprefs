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
 * @file niftyprefs-node.h
 */

/**
 * @addtogroup prefs_obj
 * @{
 * @defgroup prefs_node NftPrefsNode
 * @brief simple API to NftPrefsNodes.
 * "nodes" are used to represent the preferences of one object. They can
 * hold various properties.
 * NftPrefsNode is a kind of "meta-type" used to describe an (unexisting) object.
 * Basically a NftPrefsNode is the last abstraction for one object before the
 * actual preference representation is written. And the other way, if a preference
 * representation is parsed it will be converted into NftPrefsNode(s) first.
 *
 * Currently this is just a wrapper to libxml2 but can be abstracted to any
 * other container format that supports trees & all needed properties.
 *
 * @{
 */


#ifndef _NIFTYPREFS_NODE_H
#define _NIFTYPREFS_NODE_H


#include <libxml/tree.h>
#include <libxml/xinclude.h>
#include "nifty-primitives.h"


/** wrapper type for one xmlNode */
typedef xmlNode                 NftPrefsNode;



NftResult                       nft_prefs_node_add_child(NftPrefsNode * parent, NftPrefsNode * cur);
NftPrefsNode                   *nft_prefs_node_get_first_child(NftPrefsNode * n);
NftPrefsNode                   *nft_prefs_node_get_next(NftPrefsNode * n);
NftPrefsNode *                  nft_prefs_node_get_next_with_name(NftPrefsNode * n, const char *name);
const char                     *nft_prefs_node_get_name(NftPrefsNode * n);
const char                     *nft_prefs_node_get_uri(NftPrefsNode * p);


char                           *nft_prefs_node_to_buffer(NftPrefsNode * n);
char                           *nft_prefs_node_to_buffer_minimal(NftPrefsNode * n);
NftResult                       nft_prefs_node_to_file(NftPrefsNode * n, const char *filename, bool overwrite);
NftResult                       nft_prefs_node_to_file_minimal(NftPrefsNode * n, const char *filename, bool overwrite);
NftPrefsNode                   *nft_prefs_node_from_buffer(char *buffer, size_t bufsize);
NftPrefsNode                   *nft_prefs_node_from_file(const char *filename);


NftPrefsNode                   *nft_prefs_node_alloc(const char *name);
void                            nft_prefs_node_free(NftPrefsNode * n);


#endif /** _NIFTYPREFS_NODE_H */

/**
 * @}
 * @}
 */
