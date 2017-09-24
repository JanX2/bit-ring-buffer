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
test_bitset_of_size(id self, size_t bit_count)
{
	size_t  i;
	struct jx_bitset  *set = jx_bitset_new(bit_count);
	
	for (i = 0; i < bit_count; i++) {
		jx_bitset_set(set, i, true);
		XCTAssertTrue(jx_bitset_get(set, i), "Unexpected value for bit %zu", i);
	}
	
	for (i = 0; i < bit_count; i++) {
		jx_bitset_set(set, i, false);
		XCTAssertFalse(jx_bitset_get(set, i), "Unexpected value for bit %zu", i);
	}
	
	jx_bitset_free(set);
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
