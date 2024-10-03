/**
 * Portions Copyright (c) 1996-2024, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, The Regents of the University of California
 *
 * The consine_distance base implementation below is written with reference
 * from pgvector's implementation
 *
 */

namespace base {
	float cosine_distance_ref (const float* x, const float* y, size_t d);
}
