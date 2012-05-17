
#include <stdlib.h>
#include <niftylog.h>
#include "niftyprefs-array.h"






/******************************************************************************/
/**************************** STATIC FUNCTIONS ********************************/
/******************************************************************************/


/** find first free slot in an NftArray */
static NftResult _find_free_slot(NftArray *a, NftArraySlot *s)
{
	if(!a)
		NFT_LOG_NULL(NFT_FAILURE);

/** increase elementbuffer by this amount of entries if space runs out */
#define NFT_ARRAY_INC	128

        
        /* enough space left? */
        if(a->space <= a->elementcount)
        {
                /* increase buffer */
                if(!(a->elements = realloc(a->elements, 
                                          (a->space + NFT_ARRAY_INC)*
                                          sizeof(NftElement))))
                {
                        NFT_LOG_PERROR("realloc()");
                        return NFT_FAILURE;
                }

                
                /* clear new memory */
                memset(&a->elements[a->space],
                       0, 
                       NFT_ARRAY_INC*sizeof(NftElement));

                
                /* remember new listlength */
                a->space += NFT_ARRAY_INC;
        }

        
	/* find free slot in list */
	NftArraySlot i;
	for(i=0; i < a->space; i++)
	{
                if(!a->elements[i].occupied)
                {
                        /* save slot */
                        *s = i;
                        return NFT_SUCCESS;
                }
	}

        /* huh? */
        NFT_LOG(L_ERROR, "No free slot found but buffer was not increased. Something went wrong! Expect fancy stuff.");
        
	return NFT_FAILURE;
}


/**
 * check if NftArraySlot is plausible
 *
 * @param a NftArray descriptor
 * @param s NftArraySlot
 * @result TRUE if slot is plausible, FALSE otherwise
 */
static bool _slot_is_valid(NftArray *a, NftArraySlot s)
{
        if(!a)
                NFT_LOG_NULL(FALSE);

        if(s >= a->space)
        {
                NFT_LOG(L_ERROR, "Requested slot %d from array that only has %d slots", 
                                s, a->space);
                return FALSE;
        }

        return TRUE;
}



/******************************************************************************/
/**************************** API FUNCTIONS ***********************************/
/******************************************************************************/



/**
 * initialize an array descriptor
 *
 * @param a pointer to space that should be initialized to be used as NftArray
 */
void nft_array(NftArray *a, size_t elementSize)
{
        if(!a)
                NFT_LOG_NULL();
        
        memset(a, 0, sizeof(NftArray));

    	a->elementsize = elementSize;
}


/**
 * release all resources used by an array
 *
 * @param a NftArray descriptor
 */
void nft_array_free(NftArray *a)
{
        if(!a)
                NFT_LOG_NULL();

        free(a->elements);
        a->elements = NULL;
        a->elementcount = 0;
    	a->elementsize = 0;
        a->space = 0;
}


/**
 * NftArray setter
 */
void nft_array_set_type(NftArray *a, int type)
{
        if(!a)
                NFT_LOG_NULL();

        a->type = type;
}


/**
 * NftArray getter
 *
 * @param a NftArray descriptor
 * @result type of NftArray or -1 upon failure
 */
int nft_array_get_type(NftArray *a)
{
        if(!a)
                NFT_LOG_NULL(-1);

        return a->type;
}


/**
 * check type of an NftArray
 *
 * @param a NftArray descriptor
 * @param type the type to compare a's type with
 * @result true if "type" and type of NftArray match, false otherwise
 */
bool nft_array_is_type(NftArray *a, int type)
{
        if(!a)
                NFT_LOG_NULL(FALSE);

        return (a->type == type);
}


/**
 * NftArray getter
 *
 * @param a NftArray descriptor
 * @result amount of elements currently in array or -1 upon error
 */
ssize_t nft_array_get_elementcount(NftArray *a)
{
        if(!a)
                NFT_LOG_NULL(-1);

        return a->elementcount;
}


/**
 * NftArray getter
 *
 * @param a NftArray descriptor
 * @result amount of elements array can hold currently or -1 upon error
 */
ssize_t nft_array_get_space(NftArray *a)
{
        if(!a)
                NFT_LOG_NULL(-1);

        return a->space;
}


/**
 * get element from array
 *
 * @param a NftArray descriptor
 * @param s slot to fetch from
 * @result pointer that was stored at position s in array a or NULL upon error
 */
void *nft_array_fetch_slot(NftArray *a, NftArraySlot s)
{
        if(!a || !a->elements)
                NFT_LOG_NULL(NULL);

        if(!_slot_is_valid(a, s))
                return NULL;
        
        return a->elements[s].element;
}


/**
 * store pointer as element in array
 *
 * @param a NftArray descriptor
 * @param p pointer to store in array
 * @param s space for slot where pointer has been put or NULL to throw result away
 * @result NFT_SUCCESS or NFT_FAILURE
 */
NftResult nft_array_store(NftArray *a, void *p, NftArraySlot *s)
{
        if(!a)
                NFT_LOG_NULL(NFT_FAILURE);

        /** find a free slot */
        NftArraySlot slot;
        if(!(_find_free_slot(a, &slot)))
                return NFT_FAILURE;

        /* store element */
        a->elements[slot].ptr = p;
        a->elements[slot].occupied = TRUE;

        /* another element stored... */
        a->elementcount++;

        /* save slot */
        if(s)
                *s = slot;
        
        return NFT_SUCCESS;
}


/**
 * release element in an array (so the space can be used for a new element)
 *
 * @param a NftArray descriptor
 * @param s NftArraySlot to unstore
 */
void nft_array_unstore(NftArray *a, NftArraySlot s)
{
        if(!a)
                NFT_LOG_NULL();

        if(!_slot_is_valid(a, s))
                return;

        /* mark element as unused */
        a->elements[s].occupied = FALSE;

        /* remove one element from array */
        a->elementcount--;
}


/**
 * find the slot of a certain element by sequentially 
 * walking array and comparing criteria using a finder function
 *
 * @param a NftArray descriptor
 * @param s destination slot of found element
 * @param finder Function to check if an element matches the criterion 
 *        and then returns TRUE and FALSE otherwise
 * @param the criterion that's passed to the finder function
 * @param userptr arbitrary user pointer 
 * @result NFT_SUCCESS if element was found an slot has been written into *s, 
 *         NFT_FAILURE upon error
 */
NftResult nft_array_find_slot(NftArray *a, 
                              NftArraySlot *s, 
                              bool (*finder)(void *element, 
                                             void *criterion, 
                                             void *userptr), 
                              void *criterion, 
                              void *userptr)
{
        if(!a || !s)
                NFT_LOG_NULL(NFT_FAILURE);

        NftArraySlot r;
        for(r = 0; r < a->space; r++)
        {
                /* skip empty element */
                if(!a->elements[r].occupied)
                        continue;

                /* check if element matches */
                if(finder(a->elements[r].ptr, criterion, userptr))
                {
                        *s = r;
                        return NFT_SUCCESS;
                }
        }

        return NFT_FAILURE;
}


/**
 * execute function upon each element in an array
 *
 * @param a NftArray descriptor
 * @param foreach function that gets called for each element. 
 *        If it returns FALSE execution stops at the current element.
 * @param userptr arbitrary user pointer
 * @result NFT_SUCCESS if all foreach functions returned TRUE, NFT_FAILURE otherwise
 */ 
NftResult nft_array_foreach_element(NftArray *a, 
                                    bool (*foreach)(void *element, 
                                                    void *userptr), 
                                    void *userptr)
{
	if(!a)
		NFT_LOG_NULL(NFT_FAILURE);

	NftArraySlot r;
        for(r = 0; r < a->space; r++)
        {
                /* skip empty element */
                if(!a->elements[r].occupied)
                        continue;
	
	    	if(!(foreach(a->elements[r].ptr, userptr)))
			return NFT_FAILURE;
	}

    	return NFT_SUCCESS;
}
