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
/*
HOST_DEVICE_NI void SampleLight(const Light& Light, LightSample& LS, SurfaceSample& SS, ScatterEvent& SE, Vec3f& Wi, ColorXYZf& Le)
{
	Light.Shape.Sample(SS, LS.SurfaceUVW);

	Wi = Normalize(SS.P - SE.P);

	Le = Light.Multiplier * EvaluateTexture(Light.TextureID, SS.UV);
	
	if (Light.Shape.GetOneSided() && Dot(SE.P - SS.P, SS.N) < 0.0f)
		Le = ColorXYZf::Black();

	if (Light.EmissionUnit == Enums::Power)
		Le /= Light.Shape.Area;
}

HOST_DEVICE_NI bool IntersectLight(const Light& Light, const Ray& R, ScatterEvent& SE)
{
	Intersection Int;

	if (Light.Shape.Intersect(R, Int))
	{
		SE.T 	= Length(Int.P - R.O);
		SE.P	= Int.P;
		SE.N	= Int.N;
		SE.Wo	= -R.D;
		SE.UV	= Int.UV;
		SE.Le	= Int.Front ? Light.Multiplier * EvaluateTexture(Light.TextureID, SE.UV) : ColorXYZf::Black();
		
		if (Light.EmissionUnit == Enums::Power)
			SE.Le /= Light.Shape.Area;

		return true;
	}

	return false;
}

HOST_DEVICE_NI void IntersectLights(const Ray& R, ScatterEvent& SE, bool RespectVisibility = false)
{
	float T = FLT_MAX; 

	for (int i = 0; i < gpTracer->LightIDs.Count; i++)
	{
		const Light& Light = gpLights[gpTracer->LightIDs[i]];
		
		ScatterEvent LocalSE(Enums::Light);

		LocalSE.ID = i;

		if (RespectVisibility && !Light.Visible)
			continue;

		if (IntersectLight(Light, R, LocalSE) && LocalSE.T < T)
		{
			SE = LocalSE;
			SE.Valid  = true;
			T = LocalSE.T;
		}
	}
}

HOST_DEVICE_NI bool IntersectsLight(const Ray& R)
{
	for (int i = 0; i < gpTracer->LightIDs.Count; i++)
	{
		const Light& Light = gpLights[gpTracer->LightIDs[i]];

		if (Light.Shape.Intersects(R))
			return true;
	}

	return false;
}
*/
KERNEL void KrnlSampleLight(int NoSamples)
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	if (IDk >= NoSamples)
		return;

	const int ID = gpTracer->FrameBuffer.IDs(IDx, IDy);

	Sample& Sample = gpTracer->FrameBuffer.Samples[ID];
	Intersection& Int = Sample.Intersection;

	gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1]) = ColorXYZAf(0.0f);

	RNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));

	const int LightID = gpTracer->LightIDs[(int)floorf(RNG.Get1() * gpTracer->LightIDs.Count)];

	if (LightID < 0)
		return;

	const Object& Light = gpObjects[LightID];
	
	Ray R;

	SurfaceSample SS;

	Light.Shape.Sample(SS, RNG.Get3());

	ColorXYZf Le = Light.Multiplier * EvaluateTexture(Light.EmissionTextureID, SS.UV);

	if (Light.EmissionUnit == Enums::Power)
		Le /= Light.Shape.Area;

	const float LightPdf = DistanceSquared(Int.P, SS.P) / (Light.Shape.Area);

	Le /= LightPdf;

	const float StepSize = gStepFactorShadow * (1.0f + (expf(-Le.Y())) * 5.0f);

	R.O		= Int.P;
	R.D 	= Normalize(SS.P - Int.P);
	R.MinT	= 0.0f;
	R.MaxT	= (Int.P - SS.P).Length();
		
	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[0]];

	bool Occluded = true;

	if (Volume.BoundingBox.Intersect(R, R.MinT, R.MaxT))
	{
		R.MaxT	= (Int.P - SS.P).Length();

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
		gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[0] = 0.0f;
		gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[1] = 0.0f;
		gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[2] = 0.0f;
		gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[3] = 0.0f;
	}
}

void SampleLight(Tracer& Tracer, Statistics& Statistics, int NoSamples)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlSampleLight<<<GridDim, BlockDim>>>(NoSamples)), "Sample light"); 
}

}
