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


#define OBJNUM 1024

/** some generic API "stresstests" */
int main(int argc, char *argv[])
{

        int res = EXIT_FAILURE;
        NftPrefs *p;
        if(!(p = nft_prefs_init()))
                goto _deinit;


        /* a bunch of objects */
        struct Object
        {
                int n;
                char *name;
        }objs[OBJNUM];

        int i;
        for(i=0; i < OBJNUM; i++)
        {
                objs[i].n = i;
                objs[i].name = "foobar";
        }
        
        /* register a bunch of classes */
        for(i=0; i < OBJNUM; i++)
        {
                char cName[64];
                snprintf(cName, sizeof(cName), "%s.%d", objs[i].name, objs[i].n);
                NFT_LOG(L_DEBUG, "Registering class %d (%s)", i, cName);
                if(!(nft_prefs_class_register(p, cName, NULL, NULL)))
                        goto _deinit;
        }
        
        //~ for(i=0; i < 1024; i++)
        //~ {
                //~ char cName[64];
                //~ snprintf(cName, sizeof(cName), "%s.%d", objs[i].name, objs[i].n);
                //~ NFT_LOG(L_DEBUG, "Registering object %d", i);
                //~ if(!nft_prefs_obj_register(p, cName, &objs[i]))
                        //~ goto _deinit;
        //~ }
        
        //~ for(i=0; i < 1024; i++)
        //~ {
                //~ char cName[64];
                //~ snprintf(cName, sizeof(cName), "%s.%d", objs[i].name, objs[i].n);
                //~ NFT_LOG(L_DEBUG, "Unregistering object %d", i);
                //~ nft_prefs_obj_unregister(p, cName, &objs[i]);
        //~ }
        
        //~ for(i=0; i < 1024; i++)
        //~ {
                //~ char cName[64];
                //~ snprintf(cName, sizeof(cName), "%s.%d", objs[i].name, objs[i].n);
                //~ NFT_LOG(L_DEBUG, "Unregistering class %d", i);
                //~ nft_prefs_class_unregister(p, cName);
        //~ }
        
        res = EXIT_SUCCESS;
        
_deinit:
        nft_prefs_exit(p);
        
        return res;
}
