
// GAUSSIAN BLUR 1D KERNELS  
//---------------------------------------------------------------------------------------------------------------------------------- 
// src: http://dev.theomader.com/gaussian-kernel-calculator/
// Sigma: 1.7515
// Kernel Size: [9, 21]: odd numbers
//
// App should define the following:
// #define KERNEL_DIMENSION 5
//----------------------------------------------------------------------------------------------------------------------------------


// KERNEL_RANGE is this: center + half width of the kernel
//
// consider a blur kernel 5x5 - '*' indicates the center of the kernel
//
// x   x   x   x   x
// x   x   x   x   x
// x   x   x*  x   x
// x   x   x   x   x
// x   x   x   x   x
//
//
// separate into 1D kernels
//
// x   x   x*  x   x
//         ^-------^
//        KERNEL_RANGE
//
#ifndef KERNEL_RANGE
#define KERNEL_RANGE  ((KERNEL_DIMENSION - 1) / 2) + 1
#endif

const half KERNEL_WEIGHTS[KERNEL_RANGE] =
//----------------------------------------------------------------------------------------------------------------------------------
#if KERNEL_RANGE == 2
{ 0.369459, 0.31527 };
#endif
#if KERNEL_RANGE == 3
{ 0.265458, 0.226523, 0.140748 };
#endif
#if KERNEL_RANGE == 4
{ 0.235473, 0.200936, 0.12485, 0.056477 };
#endif
#if KERNEL_RANGE == 5
	#if USE_LEARNOPENGL_KERNEL
		{ 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
	#else
		{ 0.22703, 0.193731, 0.120373, 0.054452, 0.017929 };
	#endif
#endif
#if KERNEL_RANGE == 6
{ 0.225096, 0.192081, 0.119348, 0.053988, 0.017776, 0.004259 };
#endif
#if KERNEL_RANGE == 7
{ 0.224762, 0.191796, 0.119171, 0.053908, 0.01775, 0.004253, 0.000741 };
#endif
#if KERNEL_RANGE == 8
{ 0.22472, 0.19176, 0.119148, 0.053898, 0.017747, 0.004252, 0.000741, 0.000094 };
#endif
#if KERNEL_RANGE == 9
{ 0.224716, 0.191757, 0.119146, 0.053897, 0.017746, 0.004252, 0.000741, 0.000094, 0.000009 };
#endif
#if KERNEL_RANGE == 10
{ 0.224716, 0.191756, 0.119146, 0.053897, 0.017746, 0.004252, 0.000741, 0.000094, 0.000009, 0.000001 };
#endif
#if KERNEL_RANGE == 11
{ 0.224716, 0.191756, 0.119146, 0.053897, 0.017746, 0.004252, 0.000741, 0.000094, 0.000009, 0.000001, 0 };
#endif
//----------------------------------------------------------------------------------------------------------------------------------