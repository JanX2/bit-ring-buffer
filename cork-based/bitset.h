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


/*-----------------------------------------------------------------------
 * Bit sets
 */

#define JX_BITSET_BITS_PER_BYTE	8
#define JX_BITSET_INVERT_BIT_ORDER 1
#define JX_BITSET_USE_INLINE_STORAGE 1

#define JX_BITSET_INLINE_STORAGE_SIZE (sizeof(size_t))
#define JX_BITSET_INLINE_STORAGE_COUNT (JX_BITSET_INLINE_STORAGE_SIZE * JX_BITSET_BITS_PER_BYTE)

struct jx_bitset {
	uint8_t *bits;
#if JX_BITSET_USE_INLINE_STORAGE
	size_t  bits_inline;
#endif
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

/* Return the number of 1-bits in the set. */
int
jx_bitset_popcount(struct jx_bitset *set);

/* Extract the byte that contains a particular bit in an array. */
#define jx_bitset_byte_for_bit(set, i) \
	((set)->bits[(i) / JX_BITSET_BITS_PER_BYTE])

#if JX_BITSET_INVERT_BIT_ORDER
#define JX_BITSET_SINGLE_BIT_MASK	0b00000001
#define JX_BITSET_SINGLE_BIT_SHIFT	<<
#else
#define JX_BITSET_SINGLE_BIT_MASK	0b10000000
#define JX_BITSET_SINGLE_BIT_SHIFT	>>
#endif

/* Create a bit mask that extracts a particular bit from the byte that it lives
 * in. */
#define jx_bitset_pos_mask_for_bit(i) \
	(JX_BITSET_SINGLE_BIT_MASK JX_BITSET_SINGLE_BIT_SHIFT ((i) % JX_BITSET_BITS_PER_BYTE))

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
