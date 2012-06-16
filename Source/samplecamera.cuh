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
#include "samplebrdf.cuh"

namespace ExposureRender
{

KERNEL void KrnlSampleCamera()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	gpTracer->FrameBuffer.IDs(IDx, IDy) = -1;
	
	Sample& Sample = gpTracer->FrameBuffer.Samples(IDx, IDy);

	Sample.UV[0] = IDx;
	Sample.UV[1] = IDy;

	gpTracer->FrameBuffer.FrameEstimate(IDx, IDy) = ColorXYZAf(0.0f, 0.0f, 0.0f, 1.0f);

	RNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));
	
	Ray R;

	gpTracer->Camera.Sample(R, Vec2i(IDx, IDy), RNG);
	
	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[0]];

	Intersection Ints[2];

	IntersectObjects(R, Ints[0]);

	float MinT = 0.0f, MaxT = 0.0f;

	if (Volume.BoundingBox.Intersect(R, MinT, MaxT))
	{
		R.MinT = MinT;
		R.MaxT = MaxT;

		const float S	= -log(RNG.Get1()) / gDensityScale;
		float Sum		= 0.0f;
		float Intensity = 0.0f;
		Vec3f P;

		R.MinT += RNG.Get1() * gStepFactorPrimary;

		while (Sum < S)
		{
			if (R.MinT >= R.MaxT)
				break;

			P			= R(R.MinT);
			Intensity	= Volume(P, 0);
			Sum			+= gDensityScale * gpTracer->GetOpacity(Intensity) * gStepFactorPrimary;
			R.MinT		+= gStepFactorPrimary;
		}

		if (R.MinT < R.MaxT)
		{
			Ints[1].Valid		= true;
			Ints[1].T			= R.MinT;
			Ints[1].P			= P;
			Ints[1].Intensity	= Intensity;
		}
	}

	Intersection& Int = gpTracer->FrameBuffer.Samples(IDx, IDy).Intersection;

	Int.Valid	= false;
	Int.T		= FLT_MAX;

	for (int i = 0; i < 2; i++)
	{
		if (Ints[i].Valid && Ints[i].T < Int.T)
			Int = Ints[i];
	}

	if (Int.Valid && Int.ScatterType != Enums::Light)
		gpTracer->FrameBuffer.IDs(IDx, IDy) = IDk;

	if (Int.Valid && Int.ScatterType == Enums::Light)
	{
		ColorXYZf Le = gpObjects[Int.ID].Multiplier * EvaluateTexture(gpObjects[Int.ID].EmissionTextureID, Int.UV);
		
		if (gpObjects[Int.ID].EmissionUnit == Enums::Power)
			Le /= gpObjects[Int.ID].Shape.Area;

		gpTracer->FrameBuffer.FrameEstimate(IDx, IDy)[0] = Le[0];
		gpTracer->FrameBuffer.FrameEstimate(IDx, IDy)[1] = Le[1];
		gpTracer->FrameBuffer.FrameEstimate(IDx, IDy)[2] = Le[2];
	}

	gpTracer->FrameBuffer.FrameEstimate(IDx, IDy)[3] = Int.Valid ? 1.0f : 0.0f;
}

void SampleCamera(Tracer& Tracer, Statistics& Statistics)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlSampleCamera<<<GridDim, BlockDim>>>()), "Sample camera"); 
}

}
