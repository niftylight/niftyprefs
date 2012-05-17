

/**
 * @file niftyprefs-array.h
 */

/**
 * @defgroup array NftArray
 * @brief simple API to handle arrays
 * @{
 */


#ifndef _NIFTYPREFS_ARRAY_H
#define _NIFTYPREFS_ARRAY_H



#include <stdbool.h>
#include <stdlib.h>
#include <niftylog.h>



/** slot to define position inside array */
typedef size_t NftArraySlot;

/** descriptor for one array element */
typedef struct
{
        /** true if element is occupied, false if it's free */
        bool occupied;
        /** element data */
        char element[];
}NftElement;


/** descriptor to handle arbitrary pointer arrays */
typedef struct _NftArray
{
        /** variable to differ integer types */
        int type;
    	/** size of one element in bytes */
    	size_t elementsize;
        /** amount of elements currently in array */
        size_t elementcount;
        /** amount of elements array can currently hold */
        size_t space;
        /** buffer with pointer to element array */
        NftElement *elements;
}NftArray;




void            nft_array(NftArray *a, size_t elementSize);
void            nft_array_free(NftArray *a);
void            nft_array_set_type(NftArray *a, int type);
int             nft_array_get_type(NftArray *a);
bool            nft_array_is_type(NftArray *a, int type);
ssize_t         nft_array_get_elementcount(NftArray *a);
ssize_t         nft_array_get_space(NftArray *a);
void *          nft_array_fetch_slot(NftArray *a, NftArraySlot s);
NftResult       nft_array_store(NftArray *a, void *p, NftArraySlot *s);
void            nft_array_unstore(NftArray *a, NftArraySlot s);
NftResult       nft_array_find_slot(NftArray *a, NftArraySlot *s, bool (*finder)(void *element, void *criterion, void *userptr), void *criterion, void *userptr);
NftResult	nft_array_foreach_element(NftArray *a, bool (*foreach)(void *element, void *userptr), void *userptr);

#endif /** _NIFTYPREFS_ARRAY_H */

/**
 * @}
 */
