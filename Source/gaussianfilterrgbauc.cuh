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

KERNEL void KrnlGaussianFilterHorizontalRGBAuc(int Radius, Buffer2D<ColorRGBAuc>* Input, Buffer2D<ColorRGBAuc>* Output)
{
	KERNEL_2D(Input->GetResolution()[0], Input->GetResolution()[1])
		
	const int Range[2] = 
	{
		max((int)ceilf(IDx - Radius), 0),
		min((int)floorf(IDx + Radius), Input->GetResolution()[0] - 1)
	};

	ColorRGBAf Sum;

	float SumWeight = 0.0f;

	for (int x = Range[0]; x <= Range[1]; x++)
	{
		const float Weight = gpTracer->GaussianFilterTables.Weight(Radius, Radius + (IDx - x), Radius + (IDy - y));

		Sum[0]		+= Weight * (*Input)(x, IDy)[0];
		Sum[1]		+= Weight * (*Input)(x, IDy)[1];
		Sum[2]		+= Weight * (*Input)(x, IDy)[2];
		Sum[3]		+= Weight * (*Input)(x, IDy)[3];
		SumWeight	+= Weight;
	}
	
	if (SumWeight > 0.0f)
	{
		(*Output)(IDx, IDy)[0] = Sum[0] / SumWeight;
		(*Output)(IDx, IDy)[1] = Sum[1] / SumWeight;
		(*Output)(IDx, IDy)[2] = Sum[2] / SumWeight;
		(*Output)(IDx, IDy)[3] = Sum[3] / SumWeight;
	}
	else
		(*Output)(IDx, IDy) = (*Input)(IDx, IDy);
}

KERNEL void KrnlGaussianFilterVerticalRGBAuc(int Radius, Buffer2D<ColorRGBAuc>* Input, Buffer2D<ColorRGBAuc>* Output)
{
	KERNEL_2D(Input->GetResolution()[0], Input->GetResolution()[1])
		
	const int Range[2] =
	{
		max((int)ceilf(IDy - Radius), 0),
		min((int)floorf(IDy + Radius), Input->GetResolution()[1] - 1)
	};

	ColorRGBAf Sum;

	float SumWeight = 0.0f;

	for (int y = Range[0]; y <= Range[1]; y++)
	{
		const float Weight = gpTracer->GaussianFilterTables.Weight(Radius, Radius + (IDx - x), Radius + (IDy - y));

		Sum[0]		+= Weight * (*Input)(IDx, y)[0];
		Sum[1]		+= Weight * (*Input)(IDx, y)[1];
		Sum[2]		+= Weight * (*Input)(IDx, y)[2];
		Sum[3]		+= Weight * (*Input)(IDx, y)[3];
		SumWeight	+= Weight;
	}
	
	if (SumWeight > 0.0f)
	{
		(*Output)(IDx, IDy)[0] = Sum[0] / SumWeight;
		(*Output)(IDx, IDy)[1] = Sum[1] / SumWeight;
		(*Output)(IDx, IDy)[2] = Sum[2] / SumWeight;
		(*Output)(IDx, IDy)[3] = Sum[3] / SumWeight;
	}
	else
		(*Output)(IDx, IDy) = (*Input)(IDx, IDy);
}

void GaussianFilterRGBAuc(Tracer& Tracer, Statistics& Statistics, int Radius, Buffer2D<ColorRGBAuc>& Input)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	
	Buffer2D<ColorRGBAuc> Output("Output", Enums::Device);

	Output.Resize(Vec2i(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1]));

	Buffer2D<ColorRGBAuc>* pInput = NULL;
	Buffer2D<ColorRGBAuc>* pOutput = NULL;

	Cuda::Allocate(pInput);
	Cuda::Allocate(pOutput);
	
	Cuda::MemCopyHostToDevice(&Input, pInput);
	Cuda::MemCopyHostToDevice(&Output, pOutput);

	LAUNCH_CUDA_KERNEL_TIMED((KrnlGaussianFilterHorizontalRGBAuc<<<GridDim, BlockDim>>>(Radius, pInput, pOutput)), "Gaussian filter horizontal");
	LAUNCH_CUDA_KERNEL_TIMED((KrnlGaussianFilterVerticalRGBAuc<<<GridDim, BlockDim>>>(Radius, pOutput, pInput)), "Gaussian filter vertical");

	Cuda::Free(pInput);
	Cuda::Free(pOutput);
}

}
