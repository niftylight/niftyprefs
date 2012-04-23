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


/* one "object" */
struct Person
{
    char name[256];
    char email[256];
};

/* printable name of "object" */
#define PERSON_NAME "person"
/* file to write to */
#define FILE_NAME   "test.xml"


/* some example data */
struct Person persons[] =
{
        { .name = "Bob"   , .email = "bob@example.com"   },
        { .name = "Alice" , .email = "alice@example.com" },
};

/******************************************************************************/

/** function to generate preferences for a Person object */
static NftResult _person_to_prefs(const NftPrefsNode *newNode, const NftPrefsNode *parentNode, const void *obj, const void *userptr)
{
    struct Person *p = (struct Person *) obj;

        
    /** NULL object means failure -> exit early */
    if(!p)
        return NFT_FAILURE;

        
    /** fill empty node that has been created for us */


    /** everything fine - node contains preferences now */
    return NFT_SUCCESS;
}


/******************************************************************************/


/** MINIMAL procedure to create preferences 
    from arbitrary "object" definitions */
int main(int argc, char *argv[])
{
        /* fail per default */
        int result = EXIT_FAILURE;


        /* initialize libniftyprefs */
        if(!nft_prefs_init())
                goto _deinit;
        
        /* register "person" object to niftyprefs */
        if(!nft_prefs_obj_class_register(PERSON_NAME, NULL, &_person_to_prefs))
                goto _deinit;

        
        printf("Generating preferences for objects:\n");

        
        /* walk all persons in model */
        int i;
        for(i=0; i < sizeof(persons)/sizeof(struct Person); i++)
        {
            /* print info */
            printf("\tperson(name=\"%s\",email=\"%s\")\n",
                    persons[i].name, persons[i].email);

            /* generate preferences */
            NftPrefsNode *n;
            if(!nft_prefs_obj_to_prefs(&n, &persons[i]))
                goto _deinit;
        }

        
        /* dump to file */

        
        /* all went fine */
        result = EXIT_SUCCESS;
           
_deinit:       
        nft_prefs_obj_class_unregister(PERSON_NAME);
        nft_prefs_exit();
        
        return result;
}
