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
#include "singlescattering.h"

namespace ExposureRender
{

KERNEL void KrnlSingleScattering()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	CRNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));

	__shared__ Ray R[BLOCK_SIZE];
	__shared__ Vec3f P[BLOCK_SIZE];

	__syncthreads();

	ColorXYZAf L = ColorXYZAf::Black();

	SampleCamera(gpTracer->Camera, R[IDt], IDx, IDy, RNG);

	bool Intersects = false;

	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[0]];

	Intersection Int;

	Box BoundingBox(Volume.BoundingBox.MinP, Volume.BoundingBox.MaxP);

	if (BoundingBox.Intersect(R[IDt], R[IDt].MinT, R[IDt].MaxT))
	{
		const float S	= -log(RNG.Get1()) / gDensityScale;
		float Sum		= 0.0f;

		float Intensity = 0.0f;

		R[IDt].MinT += RNG.Get1() * gStepFactorPrimary;

		while (Sum < S)
		{
			if (R[IDt].MinT >= R[IDt].MaxT)
				break;

			P[IDt]			= R[IDt].O + R[IDt].MinT * R[IDt].D;
			Intensity		= Volume(P[IDt], 0);
			Sum				+= gDensityScale * gpTracer->GetOpacity(Intensity) * gStepFactorPrimary;
			R[IDt].MinT		+= gStepFactorPrimary;
		}

		Intersects = R[IDt].MinT < R[IDt].MaxT;
	}

	R[IDt].MinT = 0.0f;
	R[IDt].MaxT = FLT_MAX;

	ScatterEvent SE;

	IntersectLights(R[IDt], SE);

	if (SE.Valid)
		L = ColorXYZAf(SE.Le[0], SE.Le[1], SE.Le[2], 1.0f);

	if (Intersects)
		L = ColorXYZAf(1.0f);

	gpTracer->FrameBuffer.FrameEstimate(IDx, IDy) = L;
}

void SingleScattering(Tracer& Tracer, Statistics& Statistics)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlSingleScattering<<<GridDim, BlockDim>>>()), "Single scattering"); 
}

}
