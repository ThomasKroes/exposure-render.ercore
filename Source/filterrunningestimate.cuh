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

#include "color.h"
#include "geometry.h"
#include "filter.h"

namespace ExposureRender
{

KERNEL void KrnlFilterRunningEstimate()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	GaussianFilter Filter;

	int Range[2][2];

	Range[0][0] = max((int)ceilf(IDx - Filter.Size[0]), 0);
	Range[0][1] = min((int)floorf(IDx + Filter.Size[0]), gpTracer->FrameBuffer.Resolution[0] - 1);
	Range[1][0] = max((int)ceilf(IDy - Filter.Size[1]), 0);
	Range[1][1] = min((int)floorf(IDy + Filter.Size[1]), gpTracer->FrameBuffer.Resolution[1] - 1);

	ColorRGBf Sum, float SumWeight = 0.0f;

	for (int y = Range[1][0]; y <= Range[1][1]; y++)
	{
		for (int x = Range[0][0]; x <= Range[0][1]; x++)
		{
			ColorRGBAuc& C = gpTracer->FrameBuffer.DisplayEstimate(x, y);
			
			const float Weight = Filter.Evaluate(x - (IDx + 0.5f), y - (IDy + 0.5f));

			Sum[0]		+= Weight * (float)C[0];
			Sum[1]		+= Weight * (float)C[1];
			Sum[2]		+= Weight * (float)C[2];
			SumWeight	+= Weight;
		}
	}
	
	if (SumWeight > 0.0f)
	{
		gpTracer->FrameBuffer.TempDisplayEstimate(IDx, IDy)[0] = Sum[0] / SumWeight;
		gpTracer->FrameBuffer.TempDisplayEstimate(IDx, IDy)[1] = Sum[1] / SumWeight;
		gpTracer->FrameBuffer.TempDisplayEstimate(IDx, IDy)[2] = Sum[2] / SumWeight;
		gpTracer->FrameBuffer.TempDisplayEstimate(IDx, IDy)[3] = 255;
	}
}

void FilterRunningEstimate(Tracer& Tracer)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, 16, 8, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlFilterRunningEstimate<<<GridDim, BlockDim>>>()), "Filter running estimate");
}

}
