/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013-2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>

#include "bitset.h"


#if JX_BITSET_USE_INLINE_STORAGE
#define jx_bitset_uint8_pointer_for_inline_storage(set) \
    ((uint8_t *)&(set->bits_inline))
#endif

static size_t
bytes_needed(size_t bit_count)
{
	// We need one byte for every bit…
	size_t bytes_needed = bit_count / JX_BITSET_BITS_PER_BYTE;
	// Plus one extra for the leftovers that don’t fit into a whole byte.
	bytes_needed += ((bit_count % JX_BITSET_BITS_PER_BYTE) > 0);
	
	return bytes_needed;
}

void
jx_bitset_init(struct jx_bitset *set, size_t bit_count)
{
	set->bit_count = bit_count;
	set->byte_count = bytes_needed(bit_count);
    
#if JX_BITSET_USE_INLINE_STORAGE
    if (bit_count > JX_BITSET_USE_INLINE_STORAGE_COUNT)
#endif
    {
        set->bits = calloc(set->byte_count, sizeof(uint8_t));
        
    }
#if JX_BITSET_USE_INLINE_STORAGE
    else {
        set->bits = jx_bitset_uint8_pointer_for_inline_storage(set);
    }
#endif

	jx_bitset_clear(set);
}

struct jx_bitset *
jx_bitset_new(size_t bit_count)
{
	struct jx_bitset *set = malloc(sizeof(struct jx_bitset));
	
	jx_bitset_init(set, bit_count);
	
	return set;
}

void
jx_bitset_deinit(struct jx_bitset *set)
{
#if JX_BITSET_USE_INLINE_STORAGE
    if (set->bits != jx_bitset_uint8_pointer_for_inline_storage(set))
#endif
    {
        free(set->bits);
    }
}

void
jx_bitset_free(struct jx_bitset *set)
{
	jx_bitset_deinit(set);
	free(set);
}

void
jx_bitset_clear(struct jx_bitset *set)
{
	memset(set->bits, 0, set->byte_count);
}
