
#include <stdlib.h>
#include <niftylog.h>
#include "niftyprefs-array.h"






/******************************************************************************/
/**************************** STATIC FUNCTIONS ********************************/
/******************************************************************************/


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

        if(s >= a->arraysize)
        {
                NFT_LOG(L_ERROR, "Requested slot %d from array that only has %d slots", 
                                s, a->arraysize);
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
void nft_array_init(NftArray *a, size_t elementSize)
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
void nft_array_deinit(NftArray *a)
{

        if(!a)
                NFT_LOG_NULL();

        free(a->elements);
    	free(a->buffer);
        a->elements = NULL;
        a->elementcount = 0;
    	a->elementsize = 0;
        a->arraysize = 0;
}


/**
 * NftArray setter
 *
 * @param a NftArray descriptor
 * @param name printable namestring with max. NFT_ARRAY_NAME_MAXLEN 
 */
void nft_array_set_name(NftArray *a, const char *name)
{
	if(!a || !name)
		NFT_LOG_NULL();

    	strncpy(a->name, name, sizeof(a->name));
}


/**
 * NftArray getter
 *
 * @param a NftArray descriptor
 * @result printable name of array or NULL upon error
 */
const char *nft_array_get_name(NftArray *a)
{
	if(!a)
		NFT_LOG_NULL(NULL);

    	return a->name ? a->name : ("[unnamed]");
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
 * allocate a fresh slot from array so the pointer to the slot element can be
 * received with nft_array_get_slot() 
 * 
 * @param a NftArray descriptor
 * @param s pointer where new free slot will be written to 
 * @result NFT_SUCCESS or NFT_FAILURE 
 * @note Use nft_array_free_slot() if you don't need the slot anymore
 */
NftResult nft_array_slot_alloc(NftArray *a, NftArraySlot *s)
{
	if(!a)
                NFT_LOG_NULL(NFT_FAILURE);

        /** increase elementbuffer by this amount of entries if space runs out */
#define NFT_ARRAY_INC	2

        
        /* enough space left? */
        if(a->arraysize <= a->elementcount)
        {
                /* increase element descriptor array by NFT_ARRAY_INC elements */
                if(!(a->elements = realloc(
                                          	a->elements, 
                                           	/* total elements new array can hold */
                                          	(a->arraysize + NFT_ARRAY_INC) *
                                           	/* size of one element descriptor */
                                          	(sizeof(NftElement))
                                           )
                     )
                  )
                {
                        NFT_LOG_PERROR("realloc()");
                        return NFT_FAILURE;
                }

                /* increase element buffer by NFT_ARRAY_INC elements */
	    	if(!(a->buffer = realloc(a->buffer,
		                         (a->arraysize + NFT_ARRAY_INC) *
		                         a->elementsize)))
	    	{
			NFT_LOG_PERROR("realloc()");
                        return NFT_FAILURE;
		}
	    
                /* clear new memory */
                memset(&a->elements[a->arraysize],
                       0, 
                       NFT_ARRAY_INC*sizeof(NftElement));
		memset(&a->buffer[a->elementsize*a->arraysize],
		       0,
		       NFT_ARRAY_INC*a->elementsize);
                
                /* remember new arraysize */
                a->arraysize += NFT_ARRAY_INC;
        }

        
	/* find free slot in list */
	NftArraySlot i;
	for(i=0; i < a->arraysize; i++)
	{
                if(!a->elements[i].occupied)
                {
		    	/* set element as occupied */
			a->elements[i].occupied = TRUE;

			/* another element allocated... */
			a->elementcount++;

		    	/* save slot */
                        *s = i;
		    
                        return NFT_SUCCESS;
                }
	}

        /* huh? */
        NFT_LOG(L_ERROR, "No free slot found in array \"%s\" but buffer was not increased. Something went wrong! Expect fancy stuff.",
                nft_array_get_name(a));
        
	return NFT_FAILURE;
}


/**
 * free array slot so it can be reused 
 *
 * @param a NftArray descriptor
 * @param s array slot to free
 */
void nft_array_slot_free(NftArray *a, NftArraySlot s)
{
	if(!a)
                NFT_LOG_NULL();

        if(!_slot_is_valid(a, s))
                return;

    	/* clear element */
    	memset(&a->buffer[a->elementsize*s], 0, a->elementsize);
    
        /* mark element as unused */
        a->elements[s].occupied = FALSE;

        /* remove one element from array */
        a->elementcount--;
}


//~ /**
 //~ * get first occupied slot from array
 //~ *
 //~ * @param a NftArray descriptor
 //~ * @param s space where first occupied slot in array will be written 
 //~ * @result NFT_SUCCESS if slot was written to *s or NFT_FAILURE otherwise
 //~ */ 
//~ NftArraySlot nft_array_slot_get_first(NftArray *a, NftArraySlot *s)
//~ {

//~ }


/**
 * get element from array
 *
 * @param a NftArray descriptor
 * @param s slot to fetch from
 * @result pointer that was stored at position s in array a or NULL upon error
 */
void *nft_array_get_element(NftArray *a, NftArraySlot s)
{
        if(!a || !a->elements)
                NFT_LOG_NULL(NULL);

        if(!_slot_is_valid(a, s))
                return NULL;

    	if(!a->elements[s].occupied)
    	{
		NFT_LOG(L_ERROR, "requested unallocated slot \"%d\" from array \"%s\".", s, nft_array_get_name(a));
		return NULL;
	}
    
        return &a->buffer[a->elementsize*s];
}


//~ /**
 //~ * find the slot of a certain element by sequentially 
 //~ * walking array and comparing criteria using a finder function
 //~ *
 //~ * @param a NftArray descriptor
 //~ * @param s destination slot of found element
 //~ * @param finder Function to check if an element matches the criterion 
 //~ *        and then returns TRUE and FALSE otherwise
 //~ * @param the criterion that's passed to the finder function
 //~ * @param userptr arbitrary user pointer 
 //~ * @result NFT_SUCCESS if element was found an slot has been written into *s, 
 //~ *         NFT_FAILURE upon error
 //~ */
//~ NftResult nft_array_find_slot(NftArray *a, 
                              //~ NftArraySlot *s, 
                              //~ bool (*finder)(void *element, 
                                             //~ void *criterion, 
                                             //~ void *userptr), 
                              //~ void *criterion, 
                              //~ void *userptr)
//~ {
        //~ if(!a || !s)
                //~ NFT_LOG_NULL(NFT_FAILURE);

        //~ NftArraySlot r;
        //~ for(r = 0; r < a->arraysize; r++)
        //~ {
                //~ /* skip empty element */
                //~ if(!a->elements[r].occupied)
                        //~ continue;

                //~ /* check if element matches */
                //~ if(finder(a->elements[r].ptr, criterion, userptr))
                //~ {
                        //~ *s = r;
                        //~ return NFT_SUCCESS;
                //~ }
        //~ }

        //~ return NFT_FAILURE;
//~ }


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
        for(r = 0; r < a->arraysize; r++)
        {
                /* skip empty element */
                if(!a->elements[r].occupied)
                        continue;
	
	    	if(!(foreach(&a->buffer[a->elementsize*r], userptr)))
			return NFT_FAILURE;
	}

    	return NFT_SUCCESS;
}
