
/**
 * @file niftyprefs-obj.h
 */

/**
 * @defgroup prefs_obj NftPrefsObj
 * @brief API to handle PrefsObjects
 * @{
 */

#ifndef _NIFTYPREFS_OBJ_H
#define _NIFTYPREFS_OBJ_H


#include "niftyprefs.h"




/** array of objects */
typedef NftArray NftPrefsObjs;

/** an object descriptor that holds various properties about an object */
typedef struct _NftPrefsObj NftPrefsObj;


/** a node that holds various properties about an object (e.g. if your object
    reflects persons, you might have one PrefsObj for Alice and one for Bob) */
struct _NftPrefsObj
{
        /** object */
        void *object;
        /** slot in class-array of the NftPrefs 
            context this object belongs to */
        NftArraySlot classSlot;
};


/** 
 * function that creates a config-node for a certain object 
 *
 * @param p current NftPrefs context
 * @param newNode newly created empty node that will be filled by the function
 * @param obj the object to process
 * @param userptr arbitrary pointer defined upon registering the object class
 * @result NFT_SUCCESS or NFT_FAILURE (processing will be aborted upon failure)
 */
typedef NftResult (NftPrefsFromObjFunc)(NftPrefs *p, NftPrefsNode *newNode, 
                                        void *obj, void *userptr);


/** 
 * function that allocates a new object from a config-node 
 *
 * @param p current NftPrefs context
 * @param newObj space to put the pointer to a newly allocated object
 * @param node the preference node describing the object that's about to be created
 * @param userptr arbitrary pointer defined upon registering the object class
 * @result NFT_SUCCESS or NFT_FAILURE (processing will be aborted upon failure)
 */
typedef NftResult (NftPrefsToObjFunc)(NftPrefs *p, void **newObj, 
                                      NftPrefsNode *node, void *userptr);



//~ NftResult       nft_prefs_obj_register(NftPrefs *p, const char *className, void *obj);
//~ void            nft_prefs_obj_unregister(NftPrefs *p, const char *className, void *obj);

//~ void *          nft_prefs_obj_from_file(NftPrefs *p, const char *filename, void *userptr);
//~ void *          nft_prefs_obj_from_buffer(NftPrefs *p, char *buffer, size_t bufsize, void *userptr);
//~ void *          nft_prefs_obj_from_node(NftPrefs *p, NftPrefsNode *n, void *userptr);

//~ NftResult       nft_prefs_obj_to_file(NftPrefs *p, const char *className, void *obj, const char *filename, void *userptr);
//~ char *          nft_prefs_obj_to_buffer(NftPrefs *p, const char *className, void *obj, void *userptr);
//~ NftPrefsNode *  nft_prefs_obj_to_node(NftPrefs *p, const char *className, void *obj, void *userptr);


#endif /** _NIFTYPREFS_OBJ_H */


/**
 * @}
 */
