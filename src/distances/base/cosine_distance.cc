/**
 * Portions Copyright (c) 1996-2024, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, The Regents of the University of California
 *
 * The consine_distance base implementation below is written with reference
 * from pgvector's implementation
 *
 */
#include <iostream>
#include "cosine_distance.h"

#include <cmath>

namespace base {

float
cosine_distance_ref (const float* x, const float* y, size_t d)
{
    float res = 0.0, dotpdt = 0.0, mag_vx = 0.0, mag_vy=0.0;
    for (size_t i = 0; i < d; i++)
        {
            dotpdt += x[i] * y[i];
            mag_vx += x[i] * x[i];
            mag_vy += y[i] * y[i];
        }
    res = 1.0f - (dotpdt/(sqrt(mag_vx * mag_vy)));
    return res;

}
} // namespace base 
