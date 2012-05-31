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


/* one "object" */
struct Person
{
    char name[256];
    char email[256];
    int age;
};

/* printable name of "object" */
#define PERSON_NAME "person"
#define PEOPLE_NAME "people"

/* file to write to */
#define FILE_NAME   "test.xml"


/* some example data */
struct Person persons[] =
{
        { .name = "Bob"   , .email = "bob@example.com", .age = 30  },
        { .name = "Alice" , .email = "alice@example.com", .age = 30 },
};

/* our toplevel object */
struct People
{
        struct Person *people;
        size_t people_count;
};



/******************************************************************************/

/** 
 * function to generate preferences for a Person object -
 *
 * @param newNode emtpy <class/> node that has to be filled with object properties & attributes
 * @param obj the object that has to be described
 * @param userptr arbitrary user pointer (or NULL)
 * @result NFT_SUCCESS or NFT_FAILURE
 */
static NftResult _people_to_prefs(NftPrefs *p, NftPrefsNode *newNode, void *obj, void *userptr)
{
        if(!newNode || !obj)
                NFT_LOG_NULL(NFT_FAILURE);

        /* "People" object */
        struct People *people = (struct People *) obj;

        
        /* process all persons */
        size_t n;
        for(n=0; n < people->people_count; n++)
        {
                /* generate prefs node for each person */
                NftPrefsNode *node;
                if(!(node = nft_prefs_obj_to_node(p, PERSON_NAME, &people->people[n], NULL)))
                        return NFT_FAILURE;

                /* add person object as child of people object */
                nft_prefs_node_add_child(newNode, node);
        }
        
        return NFT_SUCCESS;
}


/** function to generate preferences for a Person object */
static NftResult _person_to_prefs(NftPrefs *p, NftPrefsNode *newNode, void *obj, void *userptr)
{
        /* NULL object means failure -> exit early */
        if(!p || !newNode || !obj)
                NFT_LOG_NULL(NFT_FAILURE);

        /* get our person object */
        struct Person *person = (struct Person *) obj;

        
        /* person name */
        if(!nft_prefs_node_prop_string_set(newNode, "name", person->name))
                return NFT_FAILURE;

        
        /* person email */
        if(!nft_prefs_node_prop_string_set(newNode, "email", person->email))
                return NFT_FAILURE;

        
        /* person age */
        if(!nft_prefs_node_prop_int_set(newNode, "age", person->age))
                return NFT_FAILURE;
        

        /** everything fine - node contains preferences now */
        return NFT_SUCCESS;
}


/******************************************************************************/


/** MINIMAL procedure to create preferences 
    from arbitrary "object" definitions */
int main(int argc, char *argv[])
{
        //~ /* fail per default */
        int result = EXIT_FAILURE;


        /* initialize libniftyprefs */
        NftPrefs *prefs = NULL;
        if(!(prefs = nft_prefs_init()))
                goto _deinit;

        /* register "people" class to niftyprefs */
        if(!(nft_prefs_class_register(prefs, PEOPLE_NAME, NULL, &_people_to_prefs)))
                goto _deinit;
        
        /* register "person" class to niftyprefs */
        if(!(nft_prefs_class_register(prefs, PERSON_NAME, NULL, &_person_to_prefs)))
                goto _deinit;

        
        printf("Generating preferences for objects:\n");


        /* register toplevel object (that holds all other objects) */
        struct People people = 
        { 
                .people = persons, 
                .people_count = sizeof(persons)/sizeof(struct Person) 
        };
        nft_prefs_obj_register(prefs, PEOPLE_NAME, &people);
        
        /* walk all persons in model */
        int i;
        for(i=0; i < sizeof(persons)/sizeof(struct Person); i++)
        {
                /* print info */
                printf("\tperson(name=\"%s\",email=\"%s\", age=\"%d\")\n",
                    persons[i].name, persons[i].email, persons[i].age);

                /* register object */
                if(!(nft_prefs_obj_register(prefs, PERSON_NAME, &persons[i])))
                        goto _deinit;
                
        }

        
        /* dump to file */
        if(!nft_prefs_obj_to_file(prefs, PEOPLE_NAME, &people, "test-prefs.xml", NULL))
                goto _deinit;
        
        /* all went fine */
        result = EXIT_SUCCESS;
           
_deinit:       
        nft_prefs_class_unregister(prefs, PEOPLE_NAME);
        nft_prefs_class_unregister(prefs, PERSON_NAME);
        nft_prefs_exit(prefs);
        
        return result;
}
