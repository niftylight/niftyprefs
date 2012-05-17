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


#include <stdio.h>
#include <stdlib.h>
#include <niftyprefs.h>




/** finder function for nft_array_find_slot() */
static bool _finder(void *element, void *criterion)
{
        return ((int) element == (int) criterion);
}


/** some testing for NftArray */
int main(int argc, char *argv[])
{

        int r = EXIT_FAILURE;

        /* initialize an array */
        NftArray a;
        nft_array(&a);

        /* store some stuff in array */
        int i;
        for(i=0; i < 1024; i++)
        {
                nft_array_store(&a, (void *) i, NULL);
        }

        /* walk all elements & check content */
        NftArraySlot slot;
        for(slot=0; slot < nft_array_get_space(&a); slot++)
        {
                if(nft_array_fetch_slot(&a, slot) != (int) slot)
                {
                        NFT_LOG(L_WARNING, "element hasn't expected content");
                }
        }

        /* find an element */
        if(!(nft_array_find_slot(&a, &slot, _finder, 512)))
        {
                NFT_LOG(L_ERROR, "Couldn't find element although I should have...");
                goto _deinit;
        }

        /* check if found element is correct */
        if(nft_array_fetch_slot(&a, slot) != 512)
        {
                NFT_LOG(L_ERROR, "nft_array_find_slot() appears to have found the wrong element");
                goto _deinit;
        }

        /* all fine */
        r = EXIT_SUCCESS;


_deinit:
        /* release array */
        nft_array_free(&a);
        
        return r;
}
