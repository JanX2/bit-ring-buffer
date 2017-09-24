/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBJX_DS_BITS_H
#define LIBJX_DS_BITS_H

#include <limits.h>

//#include <libcork/core/api.h>
//#include <libcork/core/types.h>


/*-----------------------------------------------------------------------
 * Bit sets
 */

#define BITS_PER_BYTE	8

struct jx_bitset {
	uint8_t *bits;
	size_t  bit_count;
	size_t  byte_count;
};

void
jx_bitset_init(struct jx_bitset *set, size_t bit_count);

void
jx_bitset_deinit(struct jx_bitset *set);

struct jx_bitset *
jx_bitset_new(size_t bit_count);

void
jx_bitset_free(struct jx_bitset *set);

void
jx_bitset_clear(struct jx_bitset *set);

/* Extract the byte that contains a particular bit in an array. */
#define jx_bitset_byte_for_bit(set, i) \
	((set)->bits[(i) / BITS_PER_BYTE])

/* Create a bit mask that extracts a particular bit from the byte that it lives
 * in. */
#define jx_bitset_pos_mask_for_bit(i) \
	(0b10000000 >> ((i) % BITS_PER_BYTE))

/* Create a bit mask that extracts everything except for a particular bit from
 * the byte that it lives in. */
#define jx_bitset_neg_mask_for_bit(i) \
	(~jx_bitset_pos_mask_for_bit(i))

/* Return whether a particular bit is set in a byte array.  Bits are numbered
 * from 0, in a big-endian order. */
#define jx_bitset_get(set, i) \
	((jx_bitset_byte_for_bit(set, i) & jx_bitset_pos_mask_for_bit(i)) != 0)

/* Set (or unset) a particular bit in a byte array.  Bits are numbered
 * from 0, in a big-endian order. */
#define jx_bitset_set(set, i, val) \
	(jx_bitset_byte_for_bit(set, i) = \
	 (jx_bitset_byte_for_bit(set, i) & jx_bitset_neg_mask_for_bit(i)) \
	 | ((val)? jx_bitset_pos_mask_for_bit(i): 0))

/* Return the count of bits in the set. */
#define jx_bitset_get_bit_count(set) \
	((set)->bit_count)

#endif /* LIBJX_DS_BITS_H */
