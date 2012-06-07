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

	GaussianFilter Filter(Vec2f(1.0f), 1.0f);

	int Range[2][2];

	Range[0][0] = max((int)ceilf(IDx - Filter.Size[0]), 0);
	Range[0][1] = min((int)floorf(IDx + Filter.Size[0]), gpTracer->FrameBuffer.Resolution[0] - 1);
	Range[1][0] = max((int)ceilf(IDy - Filter.Size[1]), 0);
	Range[1][1] = min((int)floorf(IDy + Filter.Size[1]), gpTracer->FrameBuffer.Resolution[1] - 1);

	ColorXYZf Sum;
	float SumWeight = 0.0f;

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
}

void GaussianFilterFrameEstimate(Tracer& Tracer)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, 32, 8, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlGaussianFilterFrameEstimate<<<GridDim, BlockDim>>>()), "Gaussian filter frame estimate");

	Tracer.FrameBuffer.TempFrameEstimate.Modified();
	Tracer.FrameBuffer.FrameEstimate = Tracer.FrameBuffer.TempFrameEstimate;
}

KERNEL void KrnlGaussianFilterRunningEstimate()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])
		
	GaussianFilter Filter(Vec2f(2.0f), 1.0f);

	int Range[2][2];

	Range[0][0] = max((int)ceilf(IDx - Filter.Size[0]), 0);
	Range[0][1] = min((int)floorf(IDx + Filter.Size[0]), gpTracer->FrameBuffer.Resolution[0] - 1);
	Range[1][0] = max((int)ceilf(IDy - Filter.Size[1]), 0);
	Range[1][1] = min((int)floorf(IDy + Filter.Size[1]), gpTracer->FrameBuffer.Resolution[1] - 1);

	ColorRGBf Sum;
	float SumWeight = 0.0f;

	for (int y = Range[1][0]; y <= Range[1][1]; y++)
	{
		for (int x = Range[0][0]; x <= Range[0][1]; x++)
		{
			const float Weight = Gauss2D(1.1f, x - IDx, y - IDy);

			Sum[0]		+= Weight * (float)gpTracer->FrameBuffer.DisplayEstimate(x, y)[0];
			Sum[1]		+= Weight * (float)gpTracer->FrameBuffer.DisplayEstimate(x, y)[1];
			Sum[2]		+= Weight * (float)gpTracer->FrameBuffer.DisplayEstimate(x, y)[2];
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

void GaussianFilterRunningEstimate(Tracer& Tracer)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, 32, 8, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlGaussianFilterRunningEstimate<<<GridDim, BlockDim>>>()), "Gaussian filter running estimate");
	
	Tracer.FrameBuffer.TempDisplayEstimate.Modified();
	Tracer.FrameBuffer.DisplayEstimate = Tracer.FrameBuffer.TempDisplayEstimate;
}

KERNEL void KrnlBilateralFilterRunningEstimate()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	int Range[2][2];

	/*
	Range[0][0] = max((int)ceilf(IDx - 1), 0);
	Range[0][1] = min((int)floorf(IDx + 1), gpTracer->FrameBuffer.Resolution[0] - 1);
	Range[1][0] = max((int)ceilf(IDy - 1), 0);
	Range[1][1] = min((int)floorf(IDy + 1), gpTracer->FrameBuffer.Resolution[1] - 1);

	ColorRGBf Sum, CenterColor;
	float SumWeight = 0.0f;

	for (int y = Range[1][0]; y <= Range[1][1]; y++)
	{
		for (int x = Range[0][0]; x <= Range[0][1]; x++)
		{
			const ColorRGBf KernelPosColor(gpTracer->FrameBuffer.DisplayEstimate(x, y)[0], gpTracer->FrameBuffer.DisplayEstimate(x, y)[1], gpTracer->FrameBuffer.DisplayEstimate(x, y)[2]);
			
			const float Weight = Gauss2D(1.0f, x - IDx, y - IDy);

			Sum			+= KernelPosColor * Weight;
			SumWeight	+= Weight;
		}
	}

	if (SumWeight > 0.0f)
	{
		CenterColor[0] = Sum[0] / SumWeight;
		CenterColor[1] = Sum[1] / SumWeight;
		CenterColor[2] = Sum[2] / SumWeight;
		CenterColor[3] = CenterColor[3];
	}
	else
	{
		CenterColor[0] = gpTracer->FrameBuffer.DisplayEstimate(IDx, IDy)[0];
		CenterColor[1] = gpTracer->FrameBuffer.DisplayEstimate(IDx, IDy)[1];
		CenterColor[2] = gpTracer->FrameBuffer.DisplayEstimate(IDx, IDy)[2];
		CenterColor[3] = gpTracer->FrameBuffer.DisplayEstimate(IDx, IDy)[3];
	}
	*/

	ColorRGBf Sum, CenterColor; 
	float SumWeight = 0.0f;

	CenterColor[0] = gpTracer->FrameBuffer.DisplayEstimate(IDx, IDy)[0];
	CenterColor[1] = gpTracer->FrameBuffer.DisplayEstimate(IDx, IDy)[1];
	CenterColor[2] = gpTracer->FrameBuffer.DisplayEstimate(IDx, IDy)[2];
	CenterColor[3] = gpTracer->FrameBuffer.DisplayEstimate(IDx, IDy)[3];

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
			const ColorRGBf KernelPosColor(gpTracer->FrameBuffer.DisplayEstimate(x, y)[0], gpTracer->FrameBuffer.DisplayEstimate(x, y)[1], gpTracer->FrameBuffer.DisplayEstimate(x, y)[2]);

			const float SpatialWeight		= Gauss2D(Radius, x - IDx, y - IDy);
			const float GaussianSimilarity	= Gauss2D(1.0f, (KernelPosColor - CenterColor).Length() / 255.0f, 0.0f);
			const float LuminanceFactor		= 1.0f;// - powf(Clamp(KernelPosColor.Luminance() - CenterColor.Luminance(), 0.0f, 1.0f), 2.0f);

			const float Weight = SpatialWeight * GaussianSimilarity * LuminanceFactor;

			Sum			+= KernelPosColor * Weight;
			SumWeight	+= Weight;
		}
	}
	
	ColorRGBAuc& FinalColor = gpTracer->FrameBuffer.TempDisplayEstimate(IDx, IDy);

	if (SumWeight > 0.0f)
	{
		FinalColor[0] = Lerp(1.0f - expf(-0.03f * (float)gpTracer->NoEstimates), Sum[0] / SumWeight, FinalColor[0]);
		FinalColor[1] = Lerp(1.0f - expf(-0.03f * (float)gpTracer->NoEstimates), Sum[1] / SumWeight, FinalColor[1]);
		FinalColor[2] = Lerp(1.0f - expf(-0.03f * (float)gpTracer->NoEstimates), Sum[2] / SumWeight, FinalColor[2]);
		FinalColor[3] = CenterColor[3];
	}
}

void BilateralFilterRunningEstimate(Tracer& Tracer)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, 32, 8, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlBilateralFilterRunningEstimate<<<GridDim, BlockDim>>>()), "Bilateral filter running estimate");
	
	Tracer.FrameBuffer.TempDisplayEstimate.Modified();
	Tracer.FrameBuffer.DisplayEstimate = Tracer.FrameBuffer.TempDisplayEstimate;
}

}
