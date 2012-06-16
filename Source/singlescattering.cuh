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

#include <thrust/remove.h>

namespace ExposureRender
{

KERNEL void KrnlSingleScattering()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	gpTracer->FrameBuffer.IDs(IDx, IDy) = -1;
	gpTracer->FrameBuffer.FrameEstimate(IDx, IDy) = ColorXYZAf(0.0f, 0.0f, 0.0f, 1.0f);

	CRNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));

	Ray R;

	ColorXYZAf L = ColorXYZAf::Black();

	SampleCamera(gpTracer->Camera, R, IDx, IDy, RNG);

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
		}
	}

	R.MinT = 0.0f;
	R.MaxT = 50000.0f;

	float T = R.MaxT; 
	Vec2f UV;

	Light* pLight = NULL;

	for (int i = 0; i < gpTracer->LightIDs.Count; i++)
	{
		const Light& Light = gpLights[gpTracer->LightIDs[i]];
		
		Intersection Int;

		if (Light.Visible && Light.Shape.Intersect(R, Int) && Int.T < T && Int.Front)
		{
			T		= Int.T;
			UV		= Int.UV;
			pLight	= &(gpLights[gpTracer->LightIDs[i]]);
		}
	}

	if (pLight)
	{
		ColorXYZf Le = pLight->Multiplier * EvaluateTexture(pLight->TextureID, UV);
		
		if (pLight->EmissionUnit == Enums::Power)
			Le /= pLight->Shape.Area;

		gpTracer->FrameBuffer.FrameEstimate(IDx, IDy) = ColorXYZAf(Le[0], Le[1], Le[2], 1.0f);

		gpTracer->FrameBuffer.IDs(IDx, IDy) = IDk;
		gpTracer->FrameBuffer.Samples(IDx, IDy).P = R(T);
	}

	gpTracer->FrameBuffer.Samples(IDx, IDy).UV[0] = IDx;
	gpTracer->FrameBuffer.Samples(IDx, IDy).UV[1] = IDy;
}

struct IsInvalid
{
	HOST_DEVICE bool operator()(const int& Value)
	{
		return Value < 0;
	}
};

KERNEL void KrnlConnect(int NoSamples)
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	if (IDk >= NoSamples)
		return;

	CRNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));

	const int LightID = gpTracer->LightIDs[(int)floorf(RNG.Get1() * gpTracer->LightIDs.Count)];

	if (LightID < 0)
		return;

	const Light& Light = gpLights[LightID];
	
	Ray R;

	SurfaceSample SS;

	Light.Shape.Sample(SS, RNG.Get3());

	const int ID = gpTracer->FrameBuffer.IDs(IDx, IDy);

	Sample& Sample = gpTracer->FrameBuffer.Samples[ID];

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

		R.MinT += RNG.Get1() * gStepFactorShadow;

		while (Sum < S)
		{
			if (R.MinT > R.MaxT)
			{
				Occluded = false;
				break;
			}

			Sum		+= gDensityScale * gpTracer->GetOpacity(Volume(R(R.MinT), 0)) * gStepFactorShadow;
			R.MinT	+= gStepFactorShadow;
		}

		if (!Occluded)
		{
			const float LightPdf = (R.MaxT * R.MaxT) / (AbsDot(R.D, SS.N) * Light.Shape.Area);

			gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1]) = ColorXYZAf(1.0f, 1.0f, 1.0f, 1.0f);// / LightPdf;
		}
	}
	
}

void SingleScattering(Tracer& Tracer, Statistics& Statistics)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlSingleScattering<<<GridDim, BlockDim>>>()), "Single scattering"); 

	thrust::device_ptr<int> DevicePtr(Tracer.FrameBuffer.IDs.GetData()); 

	thrust::device_ptr<int> DevicePtrEnd = thrust::remove_if(DevicePtr, DevicePtr + Tracer.FrameBuffer.IDs.GetNoElements(), IsInvalid());

	const int NoSamples = DevicePtrEnd - DevicePtr;

	Statistics = Timing("No. Samples", NoSamples);

	if (NoSamples > 0 && Tracer.LightIDs.Count > 0)
	{
		LAUNCH_CUDA_KERNEL_TIMED((KrnlConnect<<<GridDim, BlockDim>>>(NoSamples)), "Sample light"); 
		LAUNCH_CUDA_KERNEL_TIMED((KrnlConnect<<<GridDim, BlockDim>>>(NoSamples)), "Sample shader"); 
	}
}

}
