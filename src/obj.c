
#include <stdlib.h>
#include "niftyprefs.h"
#include "prefs.h"
#include "class.h"





/** a node that holds various properties about an object (e.g. if your object
    reflects persons, you might have one PrefsObj for Alice and one for Bob) */
struct _NftPrefsObj
{
        /** object */
        void *object;
        /** NftPrefsClass this object belongs to */
        NftPrefsClass *klass;
};



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


/** finder for nft_array_find_slot() */
static bool _obj_find_by_ptr(void *element, void *criterion, void *userptr)
{
	if(!element || !criterion)
		NFT_LOG_NULL(-1);
        
	NftPrefsObj *o = element;

    	return (o->object == criterion);
}

/******************************************************************************/
/**************************** PRIVATE FUNCTIONS *******************************/
/******************************************************************************/

/** initialize obj array */
NftResult prefs_obj_init_array(NftPrefsObjs *o)
{
	/* initialize class-array */
	return nft_array_init(o, sizeof(NftPrefsObj));
}


/** free all resources of one NftPrefsObj */
void prefs_obj_free(NftPrefsObj *obj)
{
        if(!obj)
               return;

        /* invalidate obj descriptor */
        obj->object = NULL;
        obj->klass = NULL;
}



/******************************************************************************/
/**************************** API FUNCTIONS ***********************************/
/******************************************************************************/

/**
 * register an object
 *
 * @param p NftPrefs context
 * @param className name of class
 * @param obj pointer to object to register
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_obj_register(NftPrefs *p, const char *className, void *obj)
{
         if(!p || !className || !obj)
                NFT_LOG_NULL(NFT_FAILURE);

        /* find class */
    	NftPrefsClass *c;
    	if(!(c = prefs_class_find_by_name(prefs_classes(p), className)))
        {
                NFT_LOG(L_ERROR, "Unknown class \"%s\"", className);
                return NFT_FAILURE;
        }

    	/** allocate new slot in objects array */
	NftArraySlot s;
	if(!(nft_array_slot_alloc(prefs_class_objects(c), &s)))
	{
		NFT_LOG(L_ERROR, "Failed to allocate new slot");
		return NFT_FAILURE;
	}
    
        /* get empty array element */
        NftPrefsObj *o;
    	if(!(o = nft_array_get_element(prefs_class_objects(c), s)))
    	{
		NFT_LOG(L_ERROR, "Failed to get element from array");
		nft_array_slot_free(prefs_class_objects(c), s);
		return NFT_FAILURE;
	}
    
             
        /* register new node */
        o->object = obj;
        o->klass = c;
        
        return NFT_SUCCESS;
}


/**
 * unregister object
 *
 * @param p NftPrefs context
 * @param className name of class
 * @param obj pointer to object to unregister
 */
void nft_prefs_obj_unregister(NftPrefs *p, const char *className, void *obj)
{
        if(!p || !className || !obj)
                return;

        /* find class */
        NftPrefsClass *c;
        if((c = prefs_class_find_by_name(prefs_classes(p), className)) < 0)
        {
                NFT_LOG(L_ERROR, "Unknown class \"%s\"", className);
                return;
        }


    	/* find object in array */
    	NftArraySlot slot;
        if(nft_array_find_slot(prefs_class_objects(c), &slot, _obj_find_by_ptr, (void *) obj, NULL))
        {
                NFT_LOG(L_ERROR, "Object \"%p\" not found", obj);
	    	return;
        }

    	/* get object */
        NftPrefsObj *o;
    	if(!(o = nft_array_get_element(prefs_class_objects(c), slot)))
    	{
		NFT_LOG(L_ERROR, "Null object?");
		return;
	}

        prefs_obj_free(o);
}


/**
 * create a NftPrefsNode from a previously registered object 
 *
 * @param p NftPrefs context
 * @param className name of class
 * @param obj pointer to object
 * @param userptr arbitrary pointer that will be passed to NftPrefsFromObjFunc
 * @result newly created NftPrefsNode or NULL
 * @note you should only need to use that from inside a NftPrefsFromObjFunc where it will be freed
 */
NftPrefsNode *nft_prefs_obj_to_node(NftPrefs *p, const char *className, void *obj, void *userptr)
{
        if(!p || !className || !obj)
                NFT_LOG_NULL(NULL);

        /* find class */
        NftPrefsClass *c;
        if((c = prefs_class_find_by_name(prefs_classes(p), className)) < 0)
        {
                NFT_LOG(L_ERROR, "Unknown class \"%s\"", className);
                return NULL;
        }

        
        /* find object descriptor */
        /*NftPrefsObjSlot os;
        if((os = _obj_find_by_ptr(c, obj)) < 0)
                return NULL;

        NftPrefsObj *o = _obj_get(c, os);*/

        /* new node */
        NftPrefsNode *node;
        if(!(node = xmlNewNode(NULL, BAD_CAST className)))
                return NULL;

        /* set name of node */
        xmlNodeSetName(node, BAD_CAST className);
        
        /* call prefsFromObj() registered for this class */
        if(!prefs_class_fromObj(c)(p, node, obj, userptr))
                return NULL;

        return node;
}


/**
 * create preferences buffer from current state of object
 * 
 * @param p NftPrefs context
 * @param className name of class
 * @param obj pointer to object
 * @param userptr arbitrary pointer that will be passed to NftPrefsToFromFunc
 * @result string holding xml representation of object (use free() to deallocate)
 */
char *nft_prefs_obj_to_buffer(NftPrefs *p, const char *className, void *obj, void *userptr)
{
        if(!p || !className || !obj)
                NFT_LOG_NULL(NULL);

        

        xmlNode *node;
        if(!(node = nft_prefs_obj_to_node(p, className, obj, userptr)))
             return NULL;

        /* result pointer (xml dump) */
        char *dump = NULL;

        /* create buffer */
        xmlBufferPtr buf;
        if(!(buf = xmlBufferCreate()))
        {
                NFT_LOG(L_ERROR, "failed to xmlBufferCreate()");
                goto _potx_exit;
        }

        /* dump node */
        if(xmlNodeDump(buf, prefs_doc(p), node, 0, TRUE) < 0)
        {
                NFT_LOG(L_ERROR, "xmlNodeDump() failed");
                goto _potx_exit;
        }

        /* allocate buffer */
        if(!(dump = malloc(xmlBufferLength(buf)+1)))
        {
                NFT_LOG_PERROR("malloc()");
                goto _potx_exit;
        }

        /* copy buffer */
        strncpy(dump, (char *) xmlBufferContent(buf), xmlBufferLength(buf));
        dump[xmlBufferLength(buf)] = '\0';

_potx_exit:
        xmlBufferFree(buf);
        xmlFreeNode(node);
        
        return dump;
}


/**
 * create preferences file from current state of object
 *
 * @param p NftPrefs context
 * @param className name of class 
 * @param obj the object to snapshot
 * @param filename full path of file to write to
 * @param userptr arbitrary function that will be passed to NftPrefsFromObjFunc
 * @result newly created object or NULL
 */
NftResult nft_prefs_obj_to_file(NftPrefs *p, const char *className, void *obj, const char *filename, void *userptr)
{
        if(!p || !className || !obj)
                NFT_LOG_NULL(NFT_FAILURE);

                
        xmlNode *node;
        if(!(node = nft_prefs_obj_to_node(p, className, obj, userptr)))
                return NFT_FAILURE;

        
        /* overall result */
        NftResult r = NFT_FAILURE;

        
        /* create temp xmlDoc */
        xmlDoc *d = NULL;
        if(!(d = xmlNewDoc(BAD_CAST "1.0")))
        {
                NFT_LOG(L_ERROR, "Failed to create new XML doc");
                goto _potb_exit;
        }
        
        /* set node as root element of temporary doc */
        xmlDocSetRootElement(d, node);
                
        /* write document to file */
        if(xmlSaveFormatFileEnc(filename, d, "UTF-8", 1) < 0)
        {
                NFT_LOG(L_ERROR, "Failed to save XML file \"%s\"", filename);
                goto _potb_exit;
        }
        
        /* successfully written file */
        r = NFT_SUCCESS;

        
_potb_exit:
        /* free node */
        xmlUnlinkNode(node);
        xmlFreeNode(node);

        /* free temporary xmlDoc */
        if(d)
                xmlFreeDoc(d);
        
        return r;
}




/**
 * create object from a NftPrefsNode
 *
 * @param p NftPrefs context
 * @param n NftPrefsNode
 * @param userptr arbitrary function that will be passed to NftPrefsToObjFunc
 * @result newly created object or NULL
 * @note you should only need to use that from inside a NftPrefsToObjFunc where it will be freed
 */
void *nft_prefs_obj_from_node(NftPrefs *p, NftPrefsNode *n, void *userptr)
{
        /* find object class */
        NftPrefsClass *c;
        if((c = prefs_class_find_by_name(prefs_classes(p), (const char *) n->name)) < 0)
        {
                NFT_LOG(L_ERROR, "Unknown class \"%s\"", n->name);
                return NULL;
        }

        
        /* create object from prefs */
        void *result = NULL;
        if(!(prefs_class_toObj(c)(p, &result, n, userptr)))
        {
                NFT_LOG(L_ERROR, "toObj() function failed");
        }

        /* validate */
        if(!(result))
                NFT_LOG(L_DEBUG, "<%s> toObj() function returned successfully but created NULL object", n->name);

    	/* register new object */
    	if(!(nft_prefs_obj_register(p, (const char *) n->name, result)))
    	{
		NFT_LOG(L_ERROR, "Failed to register new \"%s\" object", n->name);
	}
    
        return result;
}


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


/**
 * create new object from preferences file
 *
 * @param p NftPrefs context
 * @param filename full path of file
 * @param userptr arbitrary function that will be passed to NftPrefsToObjFunc
 * @result newly created object or NULL
 */
void *nft_prefs_obj_from_file(NftPrefs *p, const char *filename, void *userptr)
{
        if(!p || !filename)
                NFT_LOG_NULL(NULL);

        
        /* parse XML */
        xmlDocPtr doc;
        if(!(doc = xmlReadFile(filename, NULL, 0)))
        {
		NFT_LOG(L_ERROR, "Failed to xmlReadFile(\"%s\")", filename);
                return NULL;
        }


        /* get node */
        xmlNode *node;
        if(!(node = xmlDocGetRootElement(doc)))
        {
                NFT_LOG(L_ERROR, "No root element found in XML");
                return NULL;
        }


        /* create object */
        void *o = nft_prefs_obj_from_node(p, node, userptr);
        

        /* free old doc? */
        if(prefs_doc(p))
        {
                xmlFreeDoc(prefs_doc(p));
        }
        
        /* save new doc */
        prefs_doc_set(p,doc);
        
        return o;
}

