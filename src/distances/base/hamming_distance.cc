/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <cstddef>
#include <iostream>
#include <vector>
#include <bitset>

namespace base {

size_t hamming_distance_builtin(const std::vector<uint8_t>& vec1,
                                const std::vector<uint8_t>& vec2) {
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must be of the same length.");
    }

    size_t distance = 0;
    for (size_t i = 0; i < vec1.size(); ++i) {
        // XOR and count set bits
        distance += __builtin_popcount(vec1[i] ^ vec2[i]);
    }

    return distance;
}

// Function to calculate Hamming distance between two binary vectors
size_t hamming_distance_ref(const uint8_t* vec1, const uint8_t* vec2,
                            size_t size) {
    
    size_t distance = 0;
    for (size_t i = 0; i < size; i++) {
        // XOR the two vectors and count the number of 1s in the result
        uint8_t xor_result = vec1[i] ^ vec2[i];
        // Count the number of set bits (1s)
        distance += std::bitset<8>(xor_result).count();
    }

    return distance;
}

}

