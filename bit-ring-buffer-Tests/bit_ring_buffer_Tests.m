//
//  bit_ring_buffer_Tests.m
//  bit-ring-buffer-Tests
//
//  Created by Jan on 24.09.17.
//  Copyright © 2017 Jan. All rights reserved.
//

#import <XCTest/XCTest.h>
#include <stdbool.h>

#include "bitset.h"


@interface bit_ring_buffer_Tests : XCTestCase

@end

@implementation bit_ring_buffer_Tests

#if 0
- (void)setUp
{
	[super setUp];
	// Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
	// Put teardown code here. This method is called after the invocation of each test method in the class.
	[super tearDown];
}
#endif

static void
test_bitset_with_bit_count(id self, jx_bitset *set) {
	size_t bit_count = jx_bitset_get_bit_count(set);
	
	for (size_t i = 0; i < bit_count; i += 1) {
		jx_bitset_set(set, i, true);
		XCTAssertTrue(jx_bitset_get(set, i), "Unexpected value for bit %zu", i);
		
		XCTAssertEqual(jx_bitset_popcount(set), (i + 1));
	}
	
	const size_t max_popcount = bit_count;

	for (size_t i = 0; i < bit_count; i += 1) {
		jx_bitset_set(set, i, false);
		XCTAssertFalse(jx_bitset_get(set, i), "Unexpected value for bit %zu", i);
		
		XCTAssertEqual(jx_bitset_popcount(set), (max_popcount - i - 1),
					   "Unexpected number of bits set to true for bit count %zu.", bit_count);
	}
	
	jx_bitset_set_all_to_true(set);
	XCTAssertEqual(jx_bitset_popcount(set), bit_count,
				   "Unexpected number of bits after setting all to true.");

}

static void
test_bitset_with_bit_count_alternating_values(id self, jx_bitset *set) {
	size_t bit_count = jx_bitset_get_bit_count(set);
	
	for (size_t i = 0; i < bit_count; i += 2) {
		jx_bitset_set(set, i, true);
		XCTAssertTrue(jx_bitset_get(set, i), "Unexpected value for bit %zu", i);
		
		XCTAssertEqual(jx_bitset_popcount(set), (i/2 + 1));
	}
	
	const size_t max_alternating_popcount = bit_count/2 + bit_count % 2;

	for (size_t i = 0; i < bit_count; i += 2) {
		jx_bitset_set(set, i, false);
		XCTAssertFalse(jx_bitset_get(set, i), "Unexpected value for bit %zu", i);
		
		XCTAssertEqual(jx_bitset_popcount(set), (max_alternating_popcount - i/2 - 1),
					   "Unexpected number of bits set to true for bit count %zu.", bit_count);
	}
}

static void
test_stack_bitset_of_size(id self, size_t bit_count)
{
	jx_bitset set;
	jx_bitset_init(&set, bit_count);
	
	test_bitset_with_bit_count(self, &set);
	jx_bitset_clear(&set);
	test_bitset_with_bit_count_alternating_values(self, &set);
	
	jx_bitset_deinit(&set);
}

static void
test_heap_bitset_of_size(id self, size_t bit_count)
{
	jx_bitset *set = jx_bitset_new(bit_count);
	
	test_bitset_with_bit_count(self, set);
	jx_bitset_clear(set);
	test_bitset_with_bit_count_alternating_values(self, set);

	jx_bitset_free(set);
}

static void
test_bitset_of_size(id self, size_t bit_count)
{
	test_stack_bitset_of_size(self, bit_count);
	test_heap_bitset_of_size(self, bit_count);
}

- (void)testBitset
{
	test_bitset_of_size(self, 1);
	test_bitset_of_size(self, 2);
	test_bitset_of_size(self, 3);
	test_bitset_of_size(self, 4);
	test_bitset_of_size(self, 5);
	test_bitset_of_size(self, 6);
	test_bitset_of_size(self, 7);
	test_bitset_of_size(self, 8);
	test_bitset_of_size(self, 9);
	test_bitset_of_size(self, 10);
	test_bitset_of_size(self, 11);
	test_bitset_of_size(self, 12);
	test_bitset_of_size(self, 13);
	test_bitset_of_size(self, 14);
	test_bitset_of_size(self, 15);
	test_bitset_of_size(self, 16);
	test_bitset_of_size(self, 31);
	test_bitset_of_size(self, 32);
	test_bitset_of_size(self, 63);
	test_bitset_of_size(self, 64);
	test_bitset_of_size(self, 65535);
	test_bitset_of_size(self, 65536);
	test_bitset_of_size(self, 65537);
}

#if 0
- (void)testPerformanceExample {
	// This is an example of a performance test case.
	[self measureBlock:^{
		// Put the code you want to measure the time of here.
	}];
}
#endif

@end
