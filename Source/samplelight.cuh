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
#include "shader.h"

namespace ExposureRender
{

KERNEL void KrnlSampleLight(int NoSamples)
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	if (IDk >= NoSamples)
		return;

	const int ID = gpTracer->FrameBuffer.IDs(IDx, IDy);

	Sample& Sample = gpTracer->FrameBuffer.Samples[ID];
	
	Intersection& Int = Sample.Intersection;

	RNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));

	const int LightID = gpTracer->LightIDs[(int)floorf(RNG.Get1() * gpTracer->LightIDs.Count)];

	if (LightID < 0)
		return;
	
	Sample.LightID = LightID;

	const Object& Light = gpObjects[LightID];
	
	SurfaceSample SS;

	Light.Shape.Sample(SS, RNG.Get3());

	ColorXYZf Li = Light.Multiplier * EvaluateTexture(Light.EmissionTextureID, SS.UV);

	if (Light.EmissionUnit == Enums::Power)
		Li /= Light.Shape.Area;

	Shader Shader;

	GetShader(Int, Shader, RNG);

	const Vec3f Wi = Normalize(SS.P - Int.P);

	const Ray R(Int.P, Wi, 0.0f, (SS.P - Int.P).Length());

	const ColorXYZf F = Shader.F(Int.Wo, Wi);

	const float ShaderPdf = Shader.Pdf(Int.Wo, Wi);

	if (Li.IsBlack() || F.IsBlack() || ShaderPdf <= 0.0f)
		return;

	if (!IntersectsVolume(R, RNG))
	{
		const float LightPdf = DistanceSquared(Int.P, SS.P) / (Light.Shape.Area);

		const float Weight = PowerHeuristic(1, LightPdf, 1, ShaderPdf);

		ColorXYZf Ld;

		if (Shader.Type == Enums::Brdf)
			Ld += F * Li * (AbsDot(Wi, Int.N) * Weight / LightPdf);
		else
			Ld += F * ((Li * Weight) / LightPdf);

		gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[0] += Ld[0];
		gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[1] += Ld[1];
		gpTracer->FrameBuffer.FrameEstimate(Sample.UV[0], Sample.UV[1])[2] += Ld[2];
	}
}

void SampleLight(Tracer& Tracer, Statistics& Statistics, int NoSamples)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlSampleLight<<<GridDim, BlockDim>>>(NoSamples)), "Sample light"); 
}

}
