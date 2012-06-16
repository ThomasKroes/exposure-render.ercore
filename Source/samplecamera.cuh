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

namespace ExposureRender
{

KERNEL void KrnlSampleCamera()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	// Flag the sample initially as invalid
	gpTracer->FrameBuffer.IDs(IDx, IDy) = -1;

	// Clear the frame estimate
	gpTracer->FrameBuffer.FrameEstimate(IDx, IDy) = ColorXYZAf(0.0f, 0.0f, 0.0f, 1.0f);

	RNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));

	Ray R;

	ColorXYZAf L = ColorXYZAf::Black();

	// Sample camera to get ray
	gpTracer->Camera.Sample(R, Vec2i(IDx, IDy), RNG);

	bool Intersects = false;

	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[0]];

	Intersection Int;

	Box BoundingBox(Volume.BoundingBox.MinP, Volume.BoundingBox.MaxP);

	const bool IntersectVolume = BoundingBox.Intersect(R, R.MinT, R.MaxT);

	if (IntersectVolume)
	{
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

		Intersects = R.MinT < R.MaxT;

		if (Intersects)
		{
			L = ColorXYZAf(1.0f);
			gpTracer->FrameBuffer.IDs(IDx, IDy) = IDk;
			gpTracer->FrameBuffer.Samples(IDx, IDy).P = P;
			gpTracer->FrameBuffer.FrameEstimate(IDx, IDy)[0] = 1.0f;
			gpTracer->FrameBuffer.FrameEstimate(IDx, IDy)[1] = 1.0f;
			gpTracer->FrameBuffer.FrameEstimate(IDx, IDy)[2] = 1.0f;
			gpTracer->FrameBuffer.FrameEstimate(IDx, IDy)[3] = 1.0f;
		}
		else
		{
			gpTracer->FrameBuffer.FrameEstimate(IDx, IDy)[3] = 0.0f;
		}
	}

	/*
	R.MinT = 0.0f;
	R.MaxT = 50000.0f;

	float T = R.MaxT; 
	Vec2f UV;

	Object* pLight = NULL;

	for (int i = 0; i < gpTracer->LightIDs.Count; i++)
	{
		const Object& Light = gpObjects[gpTracer->LightIDs[i]];
		
		Intersection Int;

		if (Light.Visible && Light.Shape.Intersect(R, Int) && Int.T < T && Int.Front)
		{
			T		= Int.T;
			UV		= Int.UV;
			pLight	= &(gpObjects[gpTracer->LightIDs[i]]);
		}
	}

	if (pLight)
	{
		ColorXYZf Le = pLight->Multiplier * EvaluateTexture(pLight->EmissionTextureID, UV);
		
		if (pLight->EmissionUnit == Enums::Power)
			Le /= pLight->Shape.Area;

		gpTracer->FrameBuffer.FrameEstimate(IDx, IDy) = ColorXYZAf(Le[0], Le[1], Le[2], 1.0f);

		gpTracer->FrameBuffer.IDs(IDx, IDy) = IDk;
		gpTracer->FrameBuffer.Samples(IDx, IDy).P = R(T);
	}

	gpTracer->FrameBuffer.Samples(IDx, IDy).UV[0] = IDx;
	gpTracer->FrameBuffer.Samples(IDx, IDy).UV[1] = IDy;
	*/
}

void SampleCamera(Tracer& Tracer, Statistics& Statistics)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlSampleCamera<<<GridDim, BlockDim>>>()), "Sample camera"); 
}

}
