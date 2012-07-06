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

KERNEL void KrnlDvr()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	// Initalize the associated pixel with black
	ColorXYZAf& FrameEstimate = gpTracer->FrameBuffer.FrameEstimate(IDx, IDy);
	
	FrameEstimate = ColorXYZAf::Black();

	// Initialize the random number generator
	RNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));
	
	// Camera ray
	Ray R;

	// Generate
	gpTracer->Camera.Sample(R, Vec2i(IDx, IDy), RNG);
	
	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[0]];

	if (!Volume.BoundingBox.Intersect(R, R.MinT, R.MaxT))
		return;
	
	ColorXYZAf result = ColorXYZAf::Black();

	R.MinT += RNG.Get1() * gStepFactorPrimary;

	Vec3f Gradient;

	unsigned short Intensity;

	ColorXYZf Diffuse;

	float Opacity = 0.0f;

	Vec3f P;

	const int NoSamplesAO = 2;

    while (R.MinT <= R.MaxT)
	{
		// Get sample point
        P = R(R.MinT);

		// Obtain gradient with central differences
		Gradient = Volume.GradientCD(P);

		// Obtain intensity
        Intensity = Volume(P);

		// Get diffuse color
		Diffuse = gpTracer->GetDiffuse(Intensity);

		// Obtain opacity
		Opacity = gpTracer->GetOpacity(Intensity);

		// Move along ray
		R.MinT += gStepFactorPrimary;

		/*
		float ao = 0.f;
        int aocount = 0;

        for (int x = -NoSamplesAO; x < NoSamplesAO; x++)
		{
            for (int y = -NoSamplesAO; y < NoSamplesAO; y++)
			{
				for (int z = -NoSamplesAO; z < NoSamplesAO; z++)
				{
					ao += gpTracer->GetOpacity(Volume(P + Vec3f(x, y, z) * Volume.Spacing));
				}
            }
        }
        
        ao /= powf(NoSamplesAO * 2 + 1, 2.0f);

		Diffuse * (1.0f - ao);
		*/

        Opacity *= gStepFactorPrimary * 200.f;

		// Compositing
        result[0] = result[0] + (1.0f - result[3]) * Opacity * Diffuse[0];
        result[1] = result[1] + (1.0f - result[3]) * Opacity * Diffuse[1];
        result[2] = result[2] + (1.0f - result[3]) * Opacity * Diffuse[2];
        result[3] = result[3] + (1.0f - result[3]) * Opacity;

		// Early ray termination
        if (result[3] >= 1.0f)
		{
            result[3]	= 1.0f;
            break;
        }
    }

    FrameEstimate = result;
}

void Dvr(Tracer& Tracer, Statistics& Statistics)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlDvr<<<GridDim, BlockDim>>>()), "DVR"); 
}

}
