

#include "niftyprefs-class.h"
#include "obj.h"
#include "prefs.h"



/** a class of PrefsObjects (e.g. if your object is "Person", 
    you have one "Person" class) */
struct _NftPrefsClass
{
        /** name of object class */
        char name[NFT_PREFS_MAX_CLASSNAME+1];
        /** callback to create a new object from preferences (or NULL) */
        NftPrefsToObjFunc *toObj;
        /** callback to create preferences from the current object state (or NULL) */
        NftPrefsFromObjFunc *fromObj;
        /** array of registered NftPrefsObjs */
        NftPrefsObjs objects;
};



/******************************************************************************/
/**************************** STATIC FUNCTIONS ********************************/
/******************************************************************************/

/** helper function for nft_array_foreach_element() */
static bool _obj_free(void *element, void *userptr)
{
	size_t *i = userptr;

    	prefs_obj_free(element);
    	*i = *i+1;

    	return TRUE;
}


//~ /** find first unallocated NftPrefsClass entry in list */
//~ static NftPrefsClass *_class_find_free(NftPrefs *p)
//~ {
	//~ if(!p)
		//~ NFT_LOG_NULL(NULL);

        //~ /** increase list by this amount of entries if space runs out */
//~ #define NFT_PREFS_CLASSBUF_INC	64

        
        //~ /* enough space left? */
        //~ if(p->class_list_length <= p->class_count)
        //~ {
                //~ /* increase buffer */
                //~ if(!(p->classes = realloc(p->classes, 
                                          //~ (p->class_list_length + NFT_PREFS_CLASSBUF_INC)*
                                          //~ sizeof(NftPrefsClass))))
                //~ {
                        //~ NFT_LOG_PERROR("realloc()");
                        //~ return NFT_FAILURE;
                //~ }

                //~ /* clear new memory */
                //~ memset(&p->classes[p->class_list_length],
                       //~ 0, 
                       //~ NFT_PREFS_CLASSBUF_INC*sizeof(NftPrefsClass));

                //~ /* remember new listlength */
                //~ p->class_list_length += NFT_PREFS_CLASSBUF_INC;

        //~ }
        
	//~ /* find free in list */
	//~ int i;
	//~ for(i=0; i < p->class_list_length; i++)
	//~ {
		//~ /** looking for name = NULL index? */
		//~ if(p->classes[i].name[0] == '\0')
		//~ {
			//~ return &p->classes[i];
		//~ }
	//~ }

	//~ return NULL;
//~ }


/** find class by name */
static bool _class_find_by_name(void *element, void *criterion, void *userptr)
{
	if(!element || !criterion)
		NFT_LOG_NULL(FALSE);


    	NftPrefsClass *c = element;
    	const char *name = criterion;

    	return (strcmp(c->name, name) == 0);
}


/******************************************************************************/
/**************************** PRIVATE FUNCTIONS *******************************/
/******************************************************************************/

/** initialize class array */
NftResult prefs_class_init_array(NftPrefsClasses *a)
{
	/* initialize class-array */
	return nft_array_init(a, sizeof(NftPrefsClass));
}


/** free all resources of one NftPrefsClass */
void prefs_class_free(NftPrefsClass *klass)
{
        if(!klass)
                return;


	/* free all objects */
	size_t count = 0;
	nft_array_foreach_element(&klass->objects, _obj_free, &count);

	/* free object array */
	nft_array_deinit(&klass->objects);
    
	/* give some info */
	if(count > 0)
		NFT_LOG(L_DEBUG, "Deallocated %d stale object(s) when deallocating class \"%s\"", count, klass->name);

        /* invalidate class */
        klass->name[0] = '\0';
        klass->fromObj = NULL;
        klass->toObj = NULL;
}


/******************************************************************************/
/**************************** API FUNCTIONS ***********************************/
/******************************************************************************/

/**
 * register object class
 *
 * @param a NftArray of NftPrefsClasses
 * @param className unique name of this class
 * @param toObj pointer to NftPrefsToObjFunc used by this class
 * @param fromObj pointer to NftPrefsFromObjFunc used by this class
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_prefs_class_register(NftPrefs *p, const char *className, 
                                       NftPrefsToObjFunc *toObj, 
                                       NftPrefsFromObjFunc *fromObj)
{
        if(!p || !className)
                NFT_LOG_NULL(NFT_FAILURE);

        
        if(strlen(className) == 0)
        {
                NFT_LOG(L_ERROR, "class name may not be empty");
                return NFT_FAILURE;
        }

    	/* find class in array */
    	NftArraySlot slot;
        if(nft_array_find_slot(prefs_classes(p), &slot, _class_find_by_name, (void *) className, NULL))
        {
                NFT_LOG(L_ERROR, "class named \"%s\" already registered", className);
                return NFT_FAILURE;
        }
    
    	/** allocate new slot in class array */
	NftArraySlot s;
	if(!(nft_array_slot_alloc(prefs_classes(p), &s)))
	{
		NFT_LOG(L_ERROR, "Failed to allocate new slot");
		return NFT_FAILURE;
	}
    
        /* get empty array element */
        NftPrefsClass *n;
    	if(!(n = nft_array_get_element(prefs_classes(p), s)))
    	{
		NFT_LOG(L_ERROR, "Failed to get element from array");
		nft_array_slot_free(prefs_classes(p), s);
		return NFT_FAILURE;
	}
            
        /* register new class */
        strncpy(n->name, className, NFT_PREFS_MAX_CLASSNAME);
        n->toObj = toObj;
        n->fromObj = fromObj;
    	nft_array_init(&n->objects, sizeof(NftPrefsObj));
                
        return NFT_SUCCESS;
}


/**
 * unregister class from current context
 *
 * @param p NftPrefs context
 * @param className name of class
 */
void nft_prefs_class_unregister(NftPrefs *p, const char *className)
{
        if(!p || !className)
                NFT_LOG_NULL();

        /* find class in array */
    	NftArraySlot slot;
        if(nft_array_find_slot(prefs_classes(p), &slot, _class_find_by_name, (void *) className, NULL))
        {
                NFT_LOG(L_ERROR, 
                        "tried to unregister class \"%s\" that is not registered.", 
                        className);
	    	return;
        }
    
        /* find class */
        NftPrefsClass *klass;
    	if(!(klass = nft_array_get_element(prefs_classes(p), slot)))
	{
		NFT_LOG(L_ERROR, "attempted to free NULL class");
	    	return;
	}

        
        /* free class */
        prefs_class_free(klass);
               
}


