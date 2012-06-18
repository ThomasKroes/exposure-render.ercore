/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#ifdef __CUDACC__
	#include <host_defines.h>
#endif

#include <float.h>
#include <algorithm>
#include <math.h>

using namespace std;

#ifdef _EXPORTING
	#define EXPOSURE_RENDER_DLL    __declspec(dllexport)
#else
	#define EXPOSURE_RENDER_DLL    __declspec(dllimport)
#endif

namespace ExposureRender
{

#ifdef __CUDACC__
	#define KERNEL						__global__
	#define HOST						__host__
	#define DEVICE						__device__
	#define DEVICE_NI					DEVICE __noinline__
	#define HOST_DEVICE					HOST DEVICE 
	#define HOST_DEVICE_NI				HOST_DEVICE __noinline__
	#define CONSTANT_DEVICE				__constant__ __device__
#else
	#define KERNEL
	#define HOST
	#define DEVICE
	#define DEVICE_NI
	#define HOST_DEVICE
	#define HOST_DEVICE_NI
	#define CONSTANT_DEVICE
#endif

#define PI_F						3.141592654f	
#define HALF_PI_F					1.570796326f
#define QUARTER_PI_F				0.785398163f
#define TWO_PI_F					6.283185307f
#define INV_PI_F					0.318309886f
#define INV_TWO_PI_F				0.159154943f
#define FOUR_PI_F					12,56637061f
#define INV_FOUR_PI_F				0.079577471f
#define RAD_F						57.29577951f
#define	RAY_EPS						0.0001f
#define RAY_EPS_2					0.0002f
#define ONE_OVER_255				0.003921568f
#define	MAX_CHAR_SIZE				256
#define MAX_NO_TF_NODES				128
#define MAX_NO_VOLUMES				8
#define MAX_NO_TIMINGS				64
#define BLOCK_W						16
#define BLOCK_H						8
#define BLOCK_SIZE					128

#ifdef _MSC_VER
	#define DEBUG_BREAK __debugbreak();
#else
	#define DEBUG_BREAK
#endif

}
