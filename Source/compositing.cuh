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

#include "geometry.h"
#include "utilities.h"

namespace ExposureRender
{

KERNEL void KrnlBlendRGBAuc(Buffer2D<ColorRGBAuc>* pInputA, Buffer2D<ColorRGBAuc>* pInputB)
{
	KERNEL_2D(pInputA->GetResolution()[0], pInputA->GetResolution()[1])
	
	const Vec2f NormalizedUV((float)IDx / (float)pInputB->GetResolution()[0], (float)IDy / (float)pInputB->GetResolution()[1]);

	(*pInputA)(IDx, IDy).BlendWithForeground((*pInputB)(NormalizedUV, true));
}

void BlendRGBAuc(Tracer& Tracer, Statistics& Statistics, Buffer2D<ColorRGBAuc>& InputA, Buffer2D<ColorRGBAuc>& InputB)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	
	Buffer2D<ColorRGBAuc>* pInputA = NULL;
	Buffer2D<ColorRGBAuc>* pInputB = NULL;

	Cuda::Allocate(pInputA);
	Cuda::Allocate(pInputB);
	
	Cuda::MemCopyHostToDevice(&InputA, pInputA);
	Cuda::MemCopyHostToDevice(&InputB, pInputB);

	LAUNCH_CUDA_KERNEL_TIMED((KrnlBlendRGBAuc<<<GridDim, BlockDim>>>(pInputA, pInputB)), "Blend RGBAuc");

	Cuda::Free(pInputA);
	Cuda::Free(pInputB);
}

}
