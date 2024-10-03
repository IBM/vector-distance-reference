# vector-on-power-reference

This repository contains IBM Power based optimizations for vector search based on the vector
search functions from FAISS, knowhere and PGVector libraries.

## Structure of the repo

- **src** - Folder contains helper functions, test framework and distance computation kernels. 
Inside **distances** folder you will be able to see
  - base  - Includes reference code sourced from [PGVector](https://github.com/pgvector/pgvector) database, [FAISS](https://github.com/facebookresearch/faiss/blob/main/INSTALL.md) and [knowhere](https://github.com/zilliztech/knowhere) libraries
  - optimized - Power Optimized version of distance computation kernels using code change leveraging vector data types
  - intrinsic - Intrinsic based optimization - by using IBM specific vector built-in functions and the AltiVec built-in functions with IBM extensions for increased performance
- **MakefileAIX** - Makefile for building the code in AIX with the IBM Open XL C/C++ compiler
- **Makefile** - Makefile for building the code in Linux on RHEL with the GCC compiler
- Other license files and README.md

## Building the repo in an RHEL environment


1. Use the Makefile for Linux  and Run `make` command in the same folder as the Makefile - This will create an executable *test* inside *bin* directory
2. Run `./bin/test -h` to see the options to run the test
utility
3. Example Run command : ` ./bin/test -s 32 ` for testing dimension of 32 - Once you run this, the results will be inside "Results" directory  with file names test_result and test_time suffixed with timestamp.


To clean all object files run `make clean`

**Reference for optimizations are provided in two formats:**
1. **Compiler intrinsic based optimization**

   Path: **src/distances/intrinsic/** <br>
   To trigger intrinsic based optimization code, specify 
   `--run_intrinsic_code` command line option. For example,

        ./bin/test -s 32 --run_intrinsic_code 

2. **Code based optimization using vector data types**

    Path: **src/distances/optimized/** <br>
    To trigger source code optimized version, specify 
    `--run_optimized_code` command line option <br>

        ./bin/test -s 32 --run_optimized_code 


## Building the repo in an AIX environment

Prerequisites : Install `make` and IBM Clang from AIX toolchain and export their installation path to PATH variable
Note: Currently we only support IBM Clang 17.1.2

1. Use the Makefile for AIX  and Run `gmake -f MakefileAIX` command in the 
same folder as the Makefile - This will create an executable 
*test* inside *bin* directory
2. Follow steps 2 and 3 from RHEL environment commands

## Run the executable to test functionality

Running `./bin/test` will generate two text files inside *Results* folder

1. test_results.txt 
2. test_time.txt 

By default, the **test_results.txt** file shows the functional behavior of both **base** and **optimized** function results and it is expected to have both results same. If you run with `--run_optimized_code` the functional behavior of both **base** and **intrinsic** based implementations are compared.

Sample output - test_results.txt
        
        Original code execution results.
        Function name   array size
	                        32
        Euclidean:
        fvec_L2sqr_ref  6.11873e+06

        PowerPC code execution results.
        Function name   array size
	                        32
        Euclidean:
        fvec_L2sqr_ref  6.11873e+06

This indicates that we have run Euclidean `fvec_L2sqr_ref` function with dimension 32. We have results of original code execution v/s PowerPC(PPC) code execution listed.

The **test_time.txt** shows the execution time differences between **base** and **optimized** functions or between **base** and **intrinsic** based on the flags used.

Sample output - test_time.txt
        
        Original code execution time in ns.
        Function name 	 array size
                             32	
        Euclidean:
        fvec_L2sqr_ref        219456057

        Optimized PowerPC code execution time in ns.
        Function name 	 array size
                             32	       
        Euclidean:
        fvec_L2sqr_ref_ppc    86881094

        Percentage of optimized PPC execution time versus original time.

        Function name 	 array size
                             32	
        Euclidean:
        fvec_L2sqr_ref_ppc    39.59%  (2.53 times)


This files shows the time taken of original code execution v/s PowerPC code execution compared. The results shows that PPC based optimized code takes only *39.59%* or *2.53X* of the original code execution time.


Notes

1. The intrinsic and optimized versions of the Hamming distance test make use of the overloaded PowerPC builtin vec_popcnt().  The vec_popcnt() is only supported on Power 8 and newer CPUS.  If you wish to compile and/or run on Power 7, you must set the `#define VEC_POPCNT_SUPPORTED` in main-supported.h to 0 to get the code to compile.  Setting VEC_POPCNT_SUPPORTED to 0 will revert the optimized Hamming distance code to the base version.  Additionally the `-mcpu=pwr7`  flag will be needed in the makefile to compile for Power7.
2. You can add all the compilation flags required in Makefile including `-mcpu` and `-mvsx` depending on the use case.



