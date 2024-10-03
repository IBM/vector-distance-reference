/**
 * © Copyright IBM Corporation 2024. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/* The intrinsic and optimized version of the hamming distance function
   hamming_distance_ref use the builtin vec_popcnt.  Unfortunately, the
   built-in is only supported on Power 8 and newer systems.  So when compiling
   and running for Power 7 the intrinsic and optimized versions of the
   hamming distance functions must fall back to the base scalar version.
   Set the following #dfine VEC_POPCNT_SUPPORTED to 0 when compiling
   for Power 7 and true for all newer CPUS.  */
#define VEC_POPCNT_SUPPORTED 1   /* 0 - not supported, Power 7;
                                    1 - supported Power 8 and newer.  */

#define GET_TIME_OF_DAY 0        /* Use the gettimeofday call to measure the
                                    time.  The xlc 16 compiler does not
                                    support the chrono library.
                                    0 - Use the chrono calls to get the time.
                                    1 - Use gettimeofday to get the time.  */
