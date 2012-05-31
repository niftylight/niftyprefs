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


#include <stdlib.h>
#include <niftylog.h>
#include <niftyprefs.h>




/** finder function for nft_array_find_slot() */
static bool _finder(void *element, void *criterion, void *userptr)
{
	/** current array element */
	int *e = element;
	/** search criterion */
	int c = (int) criterion;
    
    	return (*e == c);
}


/** element validator used with nft_array_foreach_element() */
static bool _element_validator(void *element, void *userptr)
{
	static int current;
	int *n = element;
    
    	if(*n != current++)
    	{
		NFT_LOG(L_ERROR, "Element didn't have expected content. Got %d, expected %d.", 
		        *n, current);
		return FALSE;
	}
    
    	return TRUE;
}


/** some testing for NftArray */
int main(int argc, char *argv[])
{

        int r = EXIT_FAILURE;

        /* initialize an array */
        NftArray a;
        nft_array_init(&a, sizeof(int));
	nft_array_set_name(&a, "TestArray01");
    
        /* store some stuff in array */
        int i;
        for(i=0; i < 1024; i++)
        {
	    	/** allocate new slot */
	    	NftArraySlot s;
	    	if(!(nft_array_slot_alloc(&a, &s)))
	    	{
			NFT_LOG(L_ERROR, "Failed to allocate new slot");
			goto _deinit;
		}

	    	/* get pointer */
	    	int *e;
	    	if(!(e = nft_array_get_element(&a, s)))
	    	{
			NFT_LOG(L_ERROR, "Failed to nft_array_get_element()");
			goto _deinit;
		}

	    	/* store integer */
                *e = i;
        }

        /* walk all elements & check content */
    	if(!(nft_array_foreach_element(&a, _element_validator, NULL)))
		goto _deinit;
    

    	/* false error start */
    	NFT_LOG(L_INFO, "==== IGNORE ERROR MESSAGES ====");
    
    	/* try to get out-of-bound slot */
    	if(nft_array_get_element(&a, 1024))
	{
		NFT_LOG(L_ERROR, "Out-of-bound fetch succeeded although it shouldn't");
	    	goto _deinit;
	}

    	NFT_LOG(L_INFO, "==== END IGNORING ERROR MESSAGES ====");
    
        /* find an element */
    	NftArraySlot slot;
        if(!(nft_array_find_slot(&a, &slot, _finder, (void *) 512, NULL)))
        {
                NFT_LOG(L_ERROR, "Couldn't find element although I should have...");
                goto _deinit;
        }

    	/* fetch found element */    
    	int *element;
        if(!(element = nft_array_get_element(&a, slot)))
    	{
		NFT_LOG(L_ERROR, "Found NULL element (clearly a bug)");
		goto _deinit;
	}
    
	/* check if found element is correct */
    	if(*element != 512)
        {
                NFT_LOG(L_ERROR, "nft_array_find_slot() appears to have found the wrong element");
                goto _deinit;
        }

    	/* check array properties */
    	if(nft_array_get_elementcount(&a) != 1024)
    	{
		NFT_LOG(L_ERROR, "array is not the size it should be: got %d, wanted %d.", nft_array_get_elementcount(&a), 1024);
		goto _deinit;
	}
    
        /* all fine */
        r = EXIT_SUCCESS;


_deinit:
        /* release array */
        nft_array_deinit(&a);
        
        return r;
}
