/*
 * libniftyprefs - lightweight modelless preferences management library
 * Copyright (C) 2006-2014 Daniel Hiepler <daniel@niftylight.de>
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
 * @file niftyprefs-updater.h
 */

/**
 * @addtogroup prefs_node
 * @{
 * @defgroup prefs_updater NftPrefsUpdater
 * @brief API to handle updaters that update preferences from one version to
 * another
 * @{
 */

#ifndef _NIFTYPREFS_UPDATER_H
#define _NIFTYPREFS_UPDATER_H

#include "nifty-primitives.h"
#include "niftyprefs.h"




/** array of updaters */
typedef NftArray     NftPrefsUpdaters;

/** updater descriptor */
typedef              struct _NftPrefsUpdater NftPrefsUpdater;


/**
 * function that updates a node
 *
 * @param node the node that is beeing updated
 * @param version current version of this node. Updater should update to version+1
 * @param userptr arbitrary pointer
 * @result NFT_SUCCESS or NFT_FAILURE
 */
typedef              NftResult(NftPrefsUpdaterFunc)(NftPrefsNode *node, unsigned int version, void *userptr);




NftResult            nft_prefs_updater_register(NftPrefs *p, NftPrefsUpdaterFunc *updater, const char *className, unsigned int version, void *userptr);


#endif /** _NIFTYPREFS_UPDATER_H */


/**
 * @}
 * @}
 */
