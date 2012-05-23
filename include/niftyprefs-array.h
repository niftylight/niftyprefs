

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
}NftElement;


/** maximum length of NftArray->name */
#define NFT_ARRAY_NAME_MAXLEN 64

/** descriptor to handle arbitrary pointer arrays */
typedef struct _NftArray
{
        /** optional variable to differ array types */
        int type;
    	/** optional printable name of this array */
    	char name[NFT_ARRAY_NAME_MAXLEN];
    	/** size of one element in bytes */
    	size_t elementsize;
        /** amount of elements currently in array */
        size_t elementcount;
        /** amount of elements array can currently hold (size of NftArray->elements) */
        size_t arraysize;
        /** NftElement array with pointers to elements inside buffer */
        NftElement *elements;
    	/** buffer for actal elements. large enough to hold space*elementsize bytes */
    	char *buffer;
}NftArray;




void            nft_array_init(NftArray *a, size_t elementSize);
void            nft_array_deinit(NftArray *a);

void		nft_array_set_name(NftArray *a, const char *name);
void            nft_array_set_type(NftArray *a, int type);
bool            nft_array_is_type(NftArray *a, int type);

ssize_t         nft_array_get_elementcount(NftArray *a);
const char *	nft_array_get_name(NftArray *a);
int             nft_array_get_type(NftArray *a);

NftResult 	nft_array_slot_alloc(NftArray *a, NftArraySlot *s);
void		nft_array_slot_free(NftArray *a, NftArraySlot s);
void *          nft_array_get_element(NftArray *a, NftArraySlot s);
		                           
NftResult       nft_array_find_slot(NftArray *a, NftArraySlot *s, 
                                    bool (*finder)(void *element, void *criterion, void *userptr), 
                                    void *criterion, void *userptr);
NftResult	nft_array_foreach_element(NftArray *a, 
	                                  bool (*foreach)(void *element, void *userptr), 
	                                  void *userptr);

#endif /** _NIFTYPREFS_ARRAY_H */

/**
 * @}
 */
