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

KERNEL void KrnlSampleLight(int NoSamples)
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	if (IDk >= NoSamples)
		return;

	const int ID = gpTracer->FrameBuffer.IDs(IDx, IDy);

	Sample& Sample = gpTracer->FrameBuffer.Samples[ID];

	gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1]) = ColorXYZAf(0.0f);

	RNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));

	const int LightID = gpTracer->LightIDs[(int)floorf(RNG.Get1() * gpTracer->LightIDs.Count)];

	if (LightID < 0)
		return;

	const Light& Light = gpLights[LightID];
	
	Ray R;

	SurfaceSample SS;

	Light.Shape.Sample(SS, RNG.Get3());

	ColorXYZf Le = Light.Multiplier * EvaluateTexture(Light.TextureID, SS.UV);

	if (Light.EmissionUnit == Enums::Power)
		Le /= Light.Shape.Area;

	const float LightPdf = DistanceSquared(Sample.P, SS.P) / (Light.Shape.Area);

	Le /= LightPdf;

	const float StepSize = gStepFactorShadow * (1.0f + (expf(-Le.Y())) * 5.0f);

	R.O		= Sample.P;
	R.D 	= Normalize(SS.P - Sample.P);
	R.MinT	= 0.0f;
	R.MaxT	= (Sample.P - SS.P).Length();
		
	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[0]];

	/**/
	Intersection Int;
		
	Box BoundingBox(Volume.BoundingBox.MinP, Volume.BoundingBox.MaxP);

	const bool IntersectVolume = BoundingBox.Intersect(R, R.MinT, R.MaxT);

	R.MaxT	= (Sample.P - SS.P).Length();

	bool Occluded = true;

	if (IntersectVolume)
	{
		const float S	= -log(RNG.Get1()) / gDensityScale;
		float Sum		= 0.0f;

		R.MinT += RNG.Get1() * StepSize;

		while (Sum < S)
		{
			if (R.MinT > R.MaxT)
			{
				Occluded = false;
				break;
			}

			Sum		+= gDensityScale * gpTracer->GetOpacity(Volume(R(R.MinT), 0)) * StepSize;
			R.MinT	+= StepSize;
		}

		if (!Occluded)
		{
			gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[0] = Le[0];
			gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[1] = Le[1];
			gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[2] = Le[2];
			gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[3] = 1.0f;
		}
		else
		{
			gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[3] = 0.0f;
		}
	}
}

void SampleLight(Tracer& Tracer, Statistics& Statistics, int NoSamples)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlSampleLight<<<GridDim, BlockDim>>>(NoSamples)), "Sample light"); 
}

}
