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

KERNEL void KrnlGaussianFilterFrameEstimate()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	int Range[2][2];

	Range[0][0] = max((int)ceilf(IDx - 1), 0);
	Range[0][1] = min((int)floorf(IDx + 1), gpTracer->FrameBuffer.Resolution[0] - 1);
	Range[1][0] = max((int)ceilf(IDy - 1), 0);
	Range[1][1] = min((int)floorf(IDy + 1), gpTracer->FrameBuffer.Resolution[1] - 1);

	ColorXYZAf Sum;
	float SumWeight	= 0.0f;

	for (int y = Range[1][0]; y <= Range[1][1]; y++)
	{
		for (int x = Range[0][0]; x <= Range[0][1]; x++)
		{
			const float Weight = Gauss2D(1.0f, x - IDx, y - IDy);

			Sum			+= Weight * gpTracer->FrameBuffer.FrameEstimate(x, y);
			SumWeight	+= Weight;
		}
	}
	
	if (SumWeight > 0.0f)
		gpTracer->FrameBuffer.TempFrameEstimate(IDx, IDy) = Sum / SumWeight;
	else
		gpTracer->FrameBuffer.TempFrameEstimate(IDx, IDy) = gpTracer->FrameBuffer.FrameEstimate(IDx, IDy);
}

void GaussianFilterFrameEstimate(Tracer& Tracer)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, 32, 8, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlGaussianFilterFrameEstimate<<<GridDim, BlockDim>>>()), "Gaussian filter frame estimate");

	Tracer.FrameBuffer.TempFrameEstimate.Modified();
	Tracer.FrameBuffer.FrameEstimate = Tracer.FrameBuffer.TempFrameEstimate;

	Tracer.FrameBuffer.TempAlpha.Modified();
	Tracer.FrameBuffer.Alpha = Tracer.FrameBuffer.TempAlpha;
}

KERNEL void KrnlGaussianFilterRunningEstimate()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])
		
	int Range[2][2];

	Range[0][0] = max((int)ceilf(IDx - 1), 0);
	Range[0][1] = min((int)floorf(IDx + 1), gpTracer->FrameBuffer.Resolution[0] - 1);
	Range[1][0] = max((int)ceilf(IDy - 1), 0);
	Range[1][1] = min((int)floorf(IDy + 1), gpTracer->FrameBuffer.Resolution[1] - 1);

	ColorRGBf Sum;
	float SumWeight = 0.0f;

	for (int y = Range[1][0]; y <= Range[1][1]; y++)
	{
		for (int x = Range[0][0]; x <= Range[0][1]; x++)
		{
			const float Weight = Gauss2D(1.0f, x - IDx, y - IDy);

			Sum			+= Weight * gpTracer->FrameBuffer.RunningEstimateRGB(x, y);
			SumWeight	+= Weight;
		}
	}
	
	if (SumWeight > 0.0f)
		gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy) = Sum / SumWeight;
	else
		gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy) = gpTracer->FrameBuffer.RunningEstimateRGB(IDx, IDy);
}

void GaussianFilterRunningEstimate(Tracer& Tracer)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, 32, 8, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlGaussianFilterRunningEstimate<<<GridDim, BlockDim>>>()), "Gaussian filter running estimate");
	
	Tracer.FrameBuffer.TempRunningEstimateRGB.Modified();
	Tracer.FrameBuffer.RunningEstimateRGB = Tracer.FrameBuffer.TempRunningEstimateRGB;
}

KERNEL void KrnlBilateralFilterRunningEstimate()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	int Range[2][2];

	ColorRGBf Sum, CenterColor; 
	float SumWeight = 0.0f;

	CenterColor[0] = gpTracer->FrameBuffer.RunningEstimateRGB(IDx, IDy)[0];
	CenterColor[1] = gpTracer->FrameBuffer.RunningEstimateRGB(IDx, IDy)[1];
	CenterColor[2] = gpTracer->FrameBuffer.RunningEstimateRGB(IDx, IDy)[2];

	Sum = ColorRGBf::Black();
	SumWeight = 0.0f;
		
	const int Radius = 6;

	Range[0][0] = max((int)ceilf(IDx - Radius), 0);
	Range[0][1] = min((int)floorf(IDx + Radius), gpTracer->FrameBuffer.Resolution[0] - 1);
	Range[1][0] = max((int)ceilf(IDy - Radius), 0);
	Range[1][1] = min((int)floorf(IDy + Radius), gpTracer->FrameBuffer.Resolution[1] - 1);
	
	for (int y = Range[1][0]; y <= Range[1][1]; y += 2)
	{
		for (int x = Range[0][0]; x <= Range[0][1]; x += 2)
		{
			const ColorRGBf KernelPosColor = gpTracer->FrameBuffer.RunningEstimateRGB(x, y);

			const float SpatialWeight		= Gauss2D(Radius, x - IDx, y - IDy);
			const float GaussianSimilarity	= Gauss2D(1.0f, (KernelPosColor - CenterColor).Length() / 255.0f, 0.0f);
			const float LuminanceFactor		= 1.0f;// - powf(Clamp(KernelPosColor.Luminance() - CenterColor.Luminance(), 0.0f, 1.0f), 2.0f);

			const float Weight = SpatialWeight * GaussianSimilarity * LuminanceFactor;

			Sum			+= KernelPosColor * Weight;
			SumWeight	+= Weight;
		}
	}
	
	if (SumWeight > 0.0f)
	{
		gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy)[0] = Lerp(1.0f - expf(-0.03f * (float)gpTracer->NoEstimates), Sum[0] / SumWeight, gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy)[0]);
		gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy)[1] = Lerp(1.0f - expf(-0.03f * (float)gpTracer->NoEstimates), Sum[1] / SumWeight, gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy)[1]);
		gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy)[2] = Lerp(1.0f - expf(-0.03f * (float)gpTracer->NoEstimates), Sum[2] / SumWeight, gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy)[2]);
	}
	else
		gpTracer->FrameBuffer.TempRunningEstimateRGB(IDx, IDy) = gpTracer->FrameBuffer.RunningEstimateRGB(IDx, IDy);
}

void BilateralFilterRunningEstimate(Tracer& Tracer)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, 32, 8, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlBilateralFilterRunningEstimate<<<GridDim, BlockDim>>>()), "Bilateral filter running estimate");
	
	Tracer.FrameBuffer.TempRunningEstimateRGB.Modified();
	Tracer.FrameBuffer.RunningEstimateRGB = Tracer.FrameBuffer.TempRunningEstimateRGB;
}

}
