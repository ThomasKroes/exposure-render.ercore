/*
*	@file
*	@author  Thomas Kroes <t.kroes at tudelft.nl>
*	@version 1.0
*	
*	@section LICENSE
*	
*	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "color.h"
#include "geometry.h"
#include "filter.h"

#include <string>

namespace ExposureRender
{

HOST_DEVICE float Mitchell1D(const float& B, const float& C, const float& v)
{
	float av = ::fabsf(v);
    if (av > 1.0f)
    {
        return ((-B - C * 6.0f) * av * av * av +
                (B * 6.0f + C * 30.0f) * av * av +
                (B * -12.0f - C * 48.0f) * av +
                B * 8.0f + C * 24.0f) / 6.0f;
    }
    else
    {
        return ((12.0f - B * 9.0f - C * 6.0f) * av * av * av +
                (B * 12.0f + C * 6.0f - 18.0f) * av * av +
                6.0f - B * 2.0f) / 6.0f;
    }
}

KERNEL void KrnlBilateralFilterRunningEstimate()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	int Range[2][2];

	ColorRGBAuc CenterColor, KernelPosColor; 
	float SumWeight = 0.0f;

	ColorRGBf Sum;

	CenterColor = gpTracer->FrameBuffer.RunningEstimateRGB(Vec2f(IDx, IDy));

	const int Radius = 6;

	Range[0][0] = max((int)ceilf(IDx - Radius), 0);
	Range[0][1] = min((int)floorf(IDx + Radius), gpTracer->FrameBuffer.Resolution[0] - 1);
	Range[1][0] = max((int)ceilf(IDy - Radius), 0);
	Range[1][1] = min((int)floorf(IDy + Radius), gpTracer->FrameBuffer.Resolution[1] - 1);
	
	const float Factor = expf(-0.04f * (float)gpTracer->NoEstimates);

	for (int y = Range[1][0]; y <= Range[1][1]; y += 2)
	{
		for (int x = Range[0][0]; x <= Range[0][1]; x += 2)
		{
			KernelPosColor = gpTracer->FrameBuffer.RunningEstimateRGB(Vec2f(x, y));

			const float Weight = Gauss2D(Radius, 1.0f - NormalizedColorDistance(KernelPosColor, CenterColor), 0.0f) * Gauss2D(Factor * (float)Radius, x - IDx, y - IDy);

			if (Weight > 0.0f)
			{
				Sum[0]		+= KernelPosColor[0] * Weight;
				Sum[1]		+= KernelPosColor[1] * Weight;
				Sum[2]		+= KernelPosColor[2] * Weight;
				SumWeight	+= Weight;
			}
		}
	}
	
	if (SumWeight > 0.0f)
	{
		gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy)[0] = Sum[0] / SumWeight;
		gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy)[1] = Sum[1] / SumWeight;
		gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy)[2] = Sum[2] / SumWeight;
		gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy)[3] = CenterColor[3];
	}
	else
		gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy) = gpTracer->FrameBuffer.RunningEstimateRGB(IDx, IDy);
}

void BilateralFilterRunningEstimate(Tracer& Tracer, Statistics& Statistics)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlBilateralFilterRunningEstimate<<<GridDim, BlockDim>>>()), "Bilateral filter running estimate");
	
	Tracer.FrameBuffer.TempRunningEstimateRGB.Modified();
	Tracer.FrameBuffer.RunningEstimateRGB = Tracer.FrameBuffer.TempRunningEstimateRGB;
}

}
