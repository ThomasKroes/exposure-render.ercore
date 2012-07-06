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

#include "macros.cuh"
#include "intersect.cuh"

#include "textures.h"

namespace ExposureRender
{

KERNEL void KrnlSampleCamera()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	// Get current sample
	Sample& Sample = gpTracer->FrameBuffer.Samples(IDx, IDy);

	// Sample ID reference
	int& SampleID = gpTracer->FrameBuffer.IDs(IDx, IDy);

	SampleID = -1;

	// Set the associated film plane UV coordinates
	Sample.UV[0] = IDx;
	Sample.UV[1] = IDy;
	
	// Initalize the associated pixel with black
	ColorXYZAf& FrameEstimate = gpTracer->FrameBuffer.FrameEstimate(IDx, IDy);
	
	FrameEstimate = ColorXYZAf::Black();

	// Initialize the random number generator
	RNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));
	
	// Camera ray
	Ray R;

	// Generate
	gpTracer->Camera.Sample(R, Vec2i(IDx, IDy), RNG);
	
	// Reset the sample intersection
	Sample.Intersection = Intersection();

	// Intersections
	if (Intersect(R, RNG, Sample.Intersection))
	{
		if (Sample.Intersection.ScatterType == Enums::Light)
		{
			ColorXYZf Le = gpObjects[Sample.Intersection.ID].Multiplier * EvaluateTexture(gpObjects[Sample.Intersection.ID].EmissionTextureID, Sample.Intersection.UV);
		
			if (gpObjects[Sample.Intersection.ID].EmissionUnit == Enums::Power)
				Le /= gpObjects[Sample.Intersection.ID].Shape.Area;

			FrameEstimate[0] = Le[0];
			FrameEstimate[1] = Le[1];
			FrameEstimate[2] = Le[2];

			SampleID = -1;
		}
		else
		{
			SampleID = IDk;
		}
	}

	// Adjust alpha
	FrameEstimate[3] = Sample.Intersection.Valid ? 1.0f : 0.0f;
}

void SampleCamera(Tracer& Tracer, Statistics& Statistics)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlSampleCamera<<<GridDim, BlockDim>>>()), "Sample camera"); 
}

}
