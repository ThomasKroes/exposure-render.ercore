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
#include "textures.h"
#include "shader.h"

namespace ExposureRender
{

DEVICE void IntersectObjects(const Ray& R, Intersection& Int)
{
	float NearestT = FLT_MAX;

	Intersection LocalInt;

	for (int i = 0; i < gpTracer->ObjectIDs.Count; i++)
	{
		const Object& Object = gpObjects[gpTracer->ObjectIDs[i]];
		
		if (Object.Visible && Object.Shape.Intersect(R, LocalInt) && LocalInt.T < NearestT && LocalInt.Front)
		{
			NearestT			= LocalInt.T;
			Int					= LocalInt;
			Int.Valid			= true;
			Int.ScatterType		= Object.Emitter ? Enums::Light : Enums::Object;
			Int.ID				= i;
			Int.Wo				= -R.D;
		}
	}
}

KERNEL void KrnlSampleBrdf(int NoSamples)
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	if (IDk >= NoSamples)
		return;

	const int ID = gpTracer->FrameBuffer.IDs(IDx, IDy);

	Sample& Sample = gpTracer->FrameBuffer.Samples[ID];
	
	RNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(Sample.UV[0], Sample.UV[1]), &gpTracer->FrameBuffer.RandomSeeds2(Sample.UV[0], Sample.UV[1]));

	Ray R;
	
	R.O		= Sample.Intersection.P;
	R.MinT	= gStepFactorShadow;
	R.MaxT	= 1000.0f;

	Shader Shader;

	GetShader(Sample.Intersection, Shader, RNG);

	float ShaderPdf = 0.0f;

	const ColorXYZf F = Shader.SampleF(Sample.Intersection.Wo, R.D, ShaderPdf, RNG);

	if (F.IsBlack() || F.IsBlack() || ShaderPdf <= 0.0f)
		return;

	Intersection Int;

	IntersectObjects(R, Int);

	if (Int.ID != Sample.Intersection.ID)
		return;
	
	if (Int.Valid && Int.ScatterType == Enums::Light)
	{
		Object& Light = gpObjects[Int.ID];

		ColorXYZf Li = Light.Multiplier * EvaluateTexture(Light.EmissionTextureID, Int.UV);

		if (Light.EmissionUnit == Enums::Power)
			Li /= Light.Shape.Area;

		const float LightPdf = DistanceSquared(Int.P, Sample.Intersection.P) / (Light.Shape.Area);

		const float Weight = PowerHeuristic(1, ShaderPdf, 1, LightPdf);

		ColorXYZf Ld;

		if (Shader.Type == Enums::Brdf)
			Ld = F * Li * (AbsDot(R.D, Sample.Intersection.N) * Weight / ShaderPdf);
		else
			Ld = F * ((Li * Weight) / ShaderPdf);

		if (!IntersectsVolume(R, RNG))
		{
			gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[0] += Ld[0];
			gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[1] += Ld[1];
			gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[2] += Ld[2];
		}
	}
}

void SampleBrdf(Tracer& Tracer, Statistics& Statistics, int NoSamples)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlSampleBrdf<<<GridDim, BlockDim>>>(NoSamples)), "Sample BRDF"); 
}

}
