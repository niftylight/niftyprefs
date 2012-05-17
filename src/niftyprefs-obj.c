
#include <stdlib.h>
#include "niftyprefs.h"









//~ /******************************************************************************/
//~ /**************************** STATIC FUNCTIONS ********************************/
//~ /******************************************************************************/


//~ /** get object from slot */
//~ static NftPrefsObj *_obj_get(NftPrefsClass *c, NftPrefsObjSlot s)
//~ {
        //~ if(!c || !c->objects)
                //~ NFT_LOG_NULL(NULL);

        //~ if(s < 0 || s >= c->obj_list_length)
        //~ {
                //~ NFT_LOG(L_ERROR, "invalid slot: %d (must be 0 < slot < %d)", s, c->obj_list_length);
                //~ return NULL;
        //~ }
        
        //~ return &c->objects[s];
//~ }


//~ /** find first unallocated NftPrefsObj entry in list */
//~ static NftPrefsObj *_obj_find_free(NftPrefsClass *c)
//~ {
	//~ if(!c)
		//~ NFT_LOG_NULL(NULL);

        //~ /** increase list by this amount of entries if space runs out */
//~ #define NFT_PREFS_OBJBUF_INC	64

        
        //~ /* enough space left? */
        //~ if(c->obj_list_length <= c->obj_count)
        //~ {
                //~ /* increase buffer */
                //~ if(!(c->objects = realloc(c->objects, 
                                          //~ (c->obj_list_length + NFT_PREFS_OBJBUF_INC)*
                                          //~ sizeof(NftPrefsObj))))
                //~ {
                        //~ NFT_LOG_PERROR("realloc()");
                        //~ return NFT_FAILURE;
                //~ }

                
                //~ /* clear new memory */
                //~ memset(&c->objects[c->obj_list_length],
                       //~ 0, 
                       //~ NFT_PREFS_OBJBUF_INC*sizeof(NftPrefsObj));

                
                //~ /* remember new listlength */
                //~ c->obj_list_length += NFT_PREFS_OBJBUF_INC;
        //~ }

        
	//~ /* find free slot in list */
	//~ int i;
	//~ for(i=0; i < c->obj_list_length; i++)
	//~ {
                //~ if(!c->objects[i].object)
                        //~ return &c->objects[i];
	//~ }

	//~ return NULL;
//~ }


//~ /** find node by object */
//~ static NftPrefsObjSlot _obj_find_by_ptr(NftPrefsClass *c, void *obj)
//~ {
	//~ if(!c || !obj || !c->objects)
		//~ NFT_LOG_NULL(-1);
        
	//~ /* find in list */
	//~ NftPrefsObjSlot i;
	//~ for(i=0; i < c->obj_list_length; i++)
	//~ {
		//~ /** looking for name = NULL index? */
		//~ if(c->objects[i].object == obj)
		//~ {
			//~ return i;
		//~ }
	//~ }

        //~ NFT_LOG(L_DEBUG, "object %p not registered", obj);
        
	//~ return -1;
//~ }

/******************************************************************************/
/**************************** PRIVATE FUNCTIONS *******************************/
/******************************************************************************/

/** free all resources of one NftPrefsObj */
void prefs_obj_free(NftPrefsObj *obj)
{
        if(!obj)
               return;

      
        /* invalidate obj descriptor */
        obj->object = NULL;
        obj->classSlot = -1;
}



/******************************************************************************/
/**************************** API FUNCTIONS ***********************************/
/******************************************************************************/

//~ /**
 //~ * register an object
 //~ *
 //~ * @param p NftPrefs context
 //~ * @param className name of class
 //~ * @param obj pointer to object to register
 //~ * @result NFT_SUCCESS or NFT_FAILURE
 //~ */
//~ NftResult nft_prefs_obj_register(NftPrefs *p, const char *className, void *obj)
//~ {
         //~ if(!p || !className || !obj)
                //~ NFT_LOG_NULL(NFT_FAILURE);

        //~ /* find class */
        //~ NftPrefsClassSlot cs;
        //~ if((cs = _class_find_by_name(p, className)) < 0)
        //~ {
                //~ NFT_LOG(L_ERROR, "Unknown class \"%s\"", className);
                //~ return NFT_FAILURE;
        //~ }
        
        //~ /* get a empty list entry */
        //~ NftPrefsObj *o;
	//~ if(!(o = _obj_find_free(_class_get(p, cs))))
	//~ {
		//~ NFT_LOG(L_ERROR, "Couldn't find free slot in NftPrefsObj list");
		//~ return NFT_FAILURE;
	//~ }

        
        //~ /* register new node */
        //~ o->object = obj;
        //~ o->classSlot = cs;
        
        
        //~ /* another class registered */
        //~ p->classes[cs].obj_count++;

        //~ return NFT_SUCCESS;
//~ }


//~ /**
 //~ * unregister object
 //~ *
 //~ * @param p NftPrefs context
 //~ * @param className name of class
 //~ * @param obj pointer to object to unregister
 //~ */
//~ void nft_prefs_obj_unregister(NftPrefs *p, const char *className, void *obj)
//~ {
        //~ if(!p || !className || !obj)
                //~ return;

        //~ /* find class */
        //~ NftPrefsClassSlot cs;
        //~ if((cs = _class_find_by_name(p, className)) < 0)
        //~ {
                //~ NFT_LOG(L_ERROR, "Unknown class \"%s\"", className);
                //~ return;
        //~ }
        
        //~ NftPrefsObjSlot os;
        //~ if((os = _obj_find_by_ptr(_class_get(p, cs), obj)) < 0)
                //~ return;

        //~ prefs_obj_free(_obj_get(_class_get(p, cs), os));
//~ }


//~ /**
 //~ * create a NftPrefsNode from a previously registered object 
 //~ *
 //~ * @param p NftPrefs context
 //~ * @param className name of class
 //~ * @param obj pointer to object
 //~ * @param userptr arbitrary pointer that will be passed to NftPrefsFromObjFunc
 //~ * @result newly created NftPrefsNode or NULL
 //~ * @note you should only need to use that from inside a NftPrefsFromObjFunc where it will be freed
 //~ */
//~ NftPrefsNode *nft_prefs_obj_to_node(NftPrefs *p, const char *className, void *obj, void *userptr)
//~ {
        //~ if(!p || !className || !obj)
                //~ NFT_LOG_NULL(NULL);

        //~ /* find object class */
        //~ NftPrefsClassSlot cs;
        //~ if((cs = _class_find_by_name(p, className)) < 0)
                //~ return NULL;

        //~ NftPrefsClass *c = _class_get(p, cs);

        
        //~ /* find object descriptor */
        //~ /*NftPrefsObjSlot os;
        //~ if((os = _obj_find_by_ptr(c, obj)) < 0)
                //~ return NULL;

        //~ NftPrefsObj *o = _obj_get(c, os);*/

        //~ /* new node */
        //~ NftPrefsNode *node;
        //~ if(!(node = xmlNewNode(NULL, BAD_CAST c->name)))
                //~ return NULL;

        //~ /* set name of node */
        //~ xmlNodeSetName(node, BAD_CAST className);
        
        //~ /* call prefsFromObj() registered for this class */
        //~ if(!c->fromObj(p, node, obj, userptr))
                //~ return NULL;

        //~ return node;
//~ }


//~ /**
 //~ * create preferences buffer from current state of object
 //~ * 
 //~ * @param p NftPrefs context
 //~ * @param className name of class
 //~ * @param obj pointer to object
 //~ * @param userptr arbitrary pointer that will be passed to NftPrefsToFromFunc
 //~ * @result string holding xml representation of object (use free() to deallocate)
 //~ */
//~ char *nft_prefs_obj_to_buffer(NftPrefs *p, const char *className, void *obj, void *userptr)
//~ {
        //~ if(!p || !className || !obj)
                //~ NFT_LOG_NULL(NULL);

        

        //~ xmlNode *node;
        //~ if(!(node = nft_prefs_obj_to_node(p, className, obj, userptr)))
             //~ return NULL;

        //~ /* result pointer (xml dump) */
        //~ char *dump = NULL;

        //~ /* create buffer */
        //~ xmlBufferPtr buf;
        //~ if(!(buf = xmlBufferCreate()))
        //~ {
                //~ NFT_LOG(L_ERROR, "failed to xmlBufferCreate()");
                //~ goto _potx_exit;
        //~ }

        //~ /* dump node */
        //~ if(xmlNodeDump(buf, p->doc, node, 0, TRUE) < 0)
        //~ {
                //~ NFT_LOG(L_ERROR, "xmlNodeDump() failed");
                //~ goto _potx_exit;
        //~ }

        //~ /* allocate buffer */
        //~ if(!(dump = malloc(xmlBufferLength(buf)+1)))
        //~ {
                //~ NFT_LOG_PERROR("malloc()");
                //~ goto _potx_exit;
        //~ }

        //~ /* copy buffer */
        //~ strncpy(dump, (char *) xmlBufferContent(buf), xmlBufferLength(buf));
        //~ dump[xmlBufferLength(buf)] = '\0';

//~ _potx_exit:
        //~ xmlBufferFree(buf);
        //~ xmlFreeNode(node);
        
        //~ return dump;
//~ }


//~ /**
 //~ * create preferences file from current state of object
 //~ *
 //~ * @param p NftPrefs context
 //~ * @param className name of class 
 //~ * @param obj the object to snapshot
 //~ * @param filename full path of file to write to
 //~ * @param userptr arbitrary function that will be passed to NftPrefsFromObjFunc
 //~ * @result newly created object or NULL
 //~ */
//~ NftResult nft_prefs_obj_to_file(NftPrefs *p, const char *className, void *obj, const char *filename, void *userptr)
//~ {
        //~ if(!p || !className || !obj)
                //~ NFT_LOG_NULL(NFT_FAILURE);

                
        //~ xmlNode *node;
        //~ if(!(node = nft_prefs_obj_to_node(p, className, obj, userptr)))
                //~ return NFT_FAILURE;

        
        //~ /* overall result */
        //~ NftResult r = NFT_FAILURE;

        
        //~ /* create temp xmlDoc */
        //~ xmlDoc *d = NULL;
        //~ if(!(d = xmlNewDoc(BAD_CAST "1.0")))
        //~ {
                //~ NFT_LOG(L_ERROR, "Failed to create new XML doc");
                //~ goto _potb_exit;
        //~ }
        
        //~ /* set node as root element of temporary doc */
        //~ xmlDocSetRootElement(d, node);
                
        //~ /* write document to file */
        //~ if(xmlSaveFormatFileEnc(filename, d, "UTF-8", 1) < 0)
        //~ {
                //~ NFT_LOG(L_ERROR, "Failed to save XML file \"%s\"", filename);
                //~ goto _potb_exit;
        //~ }
        
        //~ /* successfully written file */
        //~ r = NFT_SUCCESS;

        
//~ _potb_exit:
        //~ /* free node */
        //~ xmlUnlinkNode(node);
        //~ xmlFreeNode(node);

        //~ /* free temporary xmlDoc */
        //~ if(d)
                //~ xmlFreeDoc(d);
        
        //~ return r;
//~ }




//~ /**
 //~ * create object from a NftPrefsNode
 //~ *
 //~ * @param p NftPrefs context
 //~ * @param n NftPrefsNode
 //~ * @param userptr arbitrary function that will be passed to NftPrefsToObjFunc
 //~ * @result newly created object or NULL
 //~ * @note you should only need to use that from inside a NftPrefsToObjFunc where it will be freed
 //~ */
//~ void *nft_prefs_obj_from_node(NftPrefs *p, NftPrefsNode *n, void *userptr)
//~ {
        //~ /* find object class */
        //~ NftPrefsClassSlot cs;
        //~ if((cs = _class_find_by_name(p, (char *) n->name)) < 0)
                //~ return NULL;

        //~ NftPrefsClass *c = _class_get(p, cs);

        //~ /* register new object */
        //~ NftPrefsObj *o;
        //~ if(!(o = _obj_find_free(c)))
                //~ return NULL;

        //~ /* remember object class */
        //~ o->classSlot = cs;
        
        //~ /* create object from prefs */
        //~ void *result = NULL;
        //~ if(!(c->toObj(p, &result, n, userptr)))
        //~ {
                //~ NFT_LOG(L_ERROR, "toObj() function failed");
        //~ }

        //~ /* validate */
        //~ if(!(result))
                //~ NFT_LOG(L_DEBUG, "<%s> toObj() function returned successfully but created NULL object", n->name);
        
        //~ /* remember new object */
        //~ o->object = result;

        //~ return result;
//~ }


//~ /**
 //~ * create new object from preferences buffer
 //~ *
 //~ * @param p NftPrefs context
 //~ * @param buffer XML buffer
 //~ * @param bufsize size of XML buffer
 //~ * @param userptr arbitrary function that will be passed to NftPrefsToObjFunc
 //~ * @result newly created object or NULL
 //~ */
//~ void *nft_prefs_obj_from_buffer(NftPrefs *p, char *buffer, size_t bufsize, void *userptr)
//~ {
        //~ if(!p || !buffer)
                //~ NFT_LOG_NULL(NULL);

        
        //~ /* parse XML */
        //~ xmlDocPtr doc;
        //~ if(!(doc = xmlReadMemory(buffer, bufsize, NULL, NULL, 0)))
        //~ {
                //~ NFT_LOG(L_ERROR, "Failed to xmlReadMemory()");
                //~ return NULL;
        //~ }

        
        //~ /* get node */
        //~ xmlNode *node;
        //~ if(!(node = xmlDocGetRootElement(doc)))
        //~ {
                //~ NFT_LOG(L_ERROR, "No root element found in XML");
                //~ return NULL;
        //~ }


        //~ /* create object */
        //~ void *o = nft_prefs_obj_from_node(p, node, userptr);
        

        //~ /* free old doc? */
        //~ if(p->doc)
                //~ xmlFreeDoc(p->doc);

        //~ /* save new doc */
        //~ p->doc = doc;
        
        //~ return o;
//~ }


//~ /**
 //~ * create new object from preferences file
 //~ *
 //~ * @param p NftPrefs context
 //~ * @param filename full path of file
 //~ * @param userptr arbitrary function that will be passed to NftPrefsToObjFunc
 //~ * @result newly created object or NULL
 //~ */
//~ void *nft_prefs_obj_from_file(NftPrefs *p, const char *filename, void *userptr)
//~ {
        //~ if(!p || !filename)
                //~ NFT_LOG_NULL(NULL);

        
        //~ /* parse XML */
        //~ xmlDocPtr doc;
        //~ if(!(doc = xmlReadFile(filename, NULL, 0)))
        //~ {
		//~ NFT_LOG(L_ERROR, "Failed to xmlReadFile(\"%s\")", filename);
                //~ return NULL;
        //~ }


        //~ /* get node */
        //~ xmlNode *node;
        //~ if(!(node = xmlDocGetRootElement(doc)))
        //~ {
                //~ NFT_LOG(L_ERROR, "No root element found in XML");
                //~ return NULL;
        //~ }


        //~ /* create object */
        //~ void *o = nft_prefs_obj_from_node(p, node, userptr);
        

        //~ /* free old doc? */
        //~ if(p->doc)
        //~ {
                //~ xmlFreeDoc(p->doc);
        //~ }
        
        //~ /* save new doc */
        //~ p->doc = doc;
        
        //~ return o;
//~ }

