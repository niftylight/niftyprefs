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
 * @file niftyprefs.h
 */


/**      
 * @mainpage <a href="http://wiki.niftylight.de/libniftyprefs">lightweight library to handle preferences for arbitrary (tree) objects using XML</a>
 *
 * <h1>Introduction</h1>
 * The basic idea is to define and manage "classes" for arbitrary (void *)
 * pointers (encapsuled in "objects") in a comfortable way. 
 * So you simply can:
 * - "snapshot" the state of an object for saving it in a preferences definition 
 * - create an object from a previously created "snapshot"
 *
 *
 * <h1>Usage</h1>
 * In every case call:
 *  - nft_prefs_init() before doing anything
 *  - nft_prefs_class_register("OBJ-KLASS-NAME") for every object-class, before using the class 
 *    with any of nft_prefs_*()
 *  - ...
 *  - nft_prefs_class_unregister() when a class is not used anymore
 *  - nft_prefs_exit() when no nft_prefs_*() needs to be called anymore
 *
 *
 * Use case 1: Generate preferences from existing object
 *  - ...
 *  - [create object]
 *    ...
 *  - [use prefsObj]
 *  - create NftPrefsNode:
 *    prefsNode = nft_prefs_obj_to_node(obj);
 *  - dump:
 *    nft_prefs_node_to_file/buffer(prefsNode, ...);
 *    ...
 *  - [free object]
 *
 * Use case 2: Generate object from preferences
 *  - ...
 *  - create NftPrefsNode:
 *    prefsNode = nft_prefs_node_from_file/buffer();
 *  - create object:
 *    foo = nft_prefs_obj_from_node(prefsNode, "OBJ-KLASS-NAME");
 *
 *
 * @defgroup prefs niftyprefs
 * @brief abstract preference handling for arbitrary objects.
 * @{
 */

#ifndef _NIFTYPREFS_H
#define _NIFTYPREFS_H




/** a context holding a list of PrefsClasses and PrefsNodes - acquired by nft_prefs_init() */
typedef struct _NftPrefs NftPrefs;


#include "nifty-primitives.h"
#include "nifty-array.h"
#include "niftyprefs-version.h"
#include "niftyprefs-node.h"
#include "niftyprefs-obj.h"
#include "niftyprefs-class.h"











NftPrefs *      nft_prefs_init();
void            nft_prefs_deinit(NftPrefs *prefs);
void            nft_prefs_free(void *p);










#endif /* _NIFTYPREFS_H */


/**
 * @}
 */
