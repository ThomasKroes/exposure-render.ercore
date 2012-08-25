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
#include "filtering.cuh"

#include "textures.h"

namespace ExposureRender
{

#define EPS (0.001f)

KERNEL void KrnlDvrSimple()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	// Initialize the random number generator
	RNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));
	
	ColorRGBAuc& DvrSimple = gpTracer->FrameBuffer.DVR(IDx, IDy);

	// Initialize black
	DvrSimple = ColorRGBAuc::Black();

	// Camera ray
	Ray R;

	// Generate
	gpTracer->Camera.Sample(R, Vec2i(IDx, IDy), RNG);
	
	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[0]];

	if (!gpTracer->Grid.GetBoundingBox().Intersect(R, R.MinT, R.MaxT))
		return;
	
	ColorXYZAf Result = ColorXYZAf::Black();

	int Counter = 0;
	
	R.MinT += RNG.Get1() * 0.0001f;

	float Distance = 0.0f;

	while (R.MinT < R.MaxT && Counter < 1000)
	{
		float NextT = 0.0f;

		int EmptySpace = 0;

		gpTracer->Grid.GetNextBoundary(R.GetMinP(), R.D, NextT, EmptySpace);

		if (EmptySpace == 1)
		{
		}
		else
		{
			float T = R.MinT;

			// Ray marching
			while (T < R.MinT + NextT)
			{
				ColorXYZf Diffuse = gpTracer->VolumeProperty.GetDiffuse(Volume(R(T)));

				ColorXYZAf Color;

				Color[0] = Diffuse[0];
				Color[1] = Diffuse[1];
				Color[2] = Diffuse[2];
				Color[3] = gpTracer->VolumeProperty.GetOpacity(Volume(R(T)));
				Color[3] *= gStepFactorPrimary * 200.f;

				Result[0] = Result[0] + (1.0f - Result[3]) * Color[3] * Color[0];
				Result[1] = Result[1] + (1.0f - Result[3]) * Color[3] * Color[1];
				Result[2] = Result[2] + (1.0f - Result[3]) * Color[3] * Color[2];
				Result[3] = Result[3] + (1.0f - Result[3]) * Color[3];

				T += gStepFactorPrimary;
			}
		}
		
		if (Result[3] >= 1.0f)
		{
			Result[3] = 1.0;
			break;
		}

		R.MinT += NextT + 0.0001f;

		++Counter;
    }
	
	DvrSimple[0] = Clamp((int)(Result[0] * 255.0f), 0, 255);
	DvrSimple[1] = Clamp((int)(Result[1] * 255.0f), 0, 255);
	DvrSimple[2] = Clamp((int)(Result[2] * 255.0f), 0, 255);
	DvrSimple[3] = 255;//Clamp((int)(Clamp(Result[3], 0.0f, 1.0f) * 255.0f), 0, 255);
}

KERNEL void KrnlDvrProgressive()
{
	/*
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	// Initialize the random number generator
	RNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));
	
	// Get current sample
	Sample& Sample = gpTracer->FrameBuffer.Samples(IDx, IDy);

	// Reference to estimate pixel
	ColorXYZAf& Estimate = gpTracer->FrameBuffer.FrameEstimate(IDx, IDy);

	// Get volume
	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[0]];

	// Generate ray
	if (gpTracer->NoEstimates == 0)
	{
		// Initialize with black
		Estimate = ColorXYZAf::Black();

		// Generate camera ray
		gpTracer->Camera.Sample(Sample.Ray, Vec2i(IDx, IDy), RNG);
		
		if (!Volume.BoundingBox.Intersect(Sample.Ray, Sample.Ray.MinT, Sample.Ray.MaxT))
		{
			Sample.Ray.MinT = -1.0f;
			return;
		}
		else
		{
			Sample.Ray.MinT = Sample.MinT + (RNG.Get1() * gStepFactorPrimary);
		}
	}
	
	if (Sample.Ray.MinT < 0.0f || Sample.Ray.MinT > Sample.Ray.MaxT)
		return;

	// Get sample point
	const Vec3f P = Sample.Ray(Sample.Ray.MinT);

	// Obtain intensity
	const float Intensity = Volume(P);
	
	// Obtain diffuse
	ColorXYZf Diffuse;// = gpTracer->GetDiffuse(Intensity);

	// Choose light to sample
	Sample.LightID = gpTracer->LightIDs[(int)floorf(RNG.Get1() * gpTracer->LightIDs.Count)];

	if (Sample.LightID < 0)
		return;
	
	// Get the light
	const Object& Light = gpProps[Sample.LightID];
	
	SurfaceSample SS;

	// Sample light and determine exitant radiance
	Light.Shape.Sample(SS, RNG.Get3());

	ColorXYZf Li = Light.Multiplier * EvaluateTexture(Light.EmissionTextureID, SS.UV);

	if (Light.EmissionUnit == Enums::Power)
		Li /= Light.Shape.Area;

	Shader Shader;

	Sample.Intersection.P				= P;
	Sample.Intersection.N				= Volume.GradientCD(Sample.Intersection.P);
	Sample.Intersection.Wo				= -Sample.Ray.D;
	Sample.Intersection.ScatterType		= Enums::Volume;
	Sample.Intersection.Wo				= true;

	// Obtain shader from intersection
	GetShader(Sample.Intersection, Shader, RNG);

	// Construct shadow ray
	Ray R;
	
	R.O		= SS.P;
	R.D		= Normalize(Sample.Intersection.P - SS.P);
	R.MinT	= RAY_EPS;
	R.MaxT	= (Sample.Intersection.P - SS.P).Length();

	const Vec3f Wi = Normalize(SS.P - Sample.Intersection.P);

	// Reflected radiance
	const ColorXYZf F = Shader.F(Sample.Intersection.Wo, Wi);

	const float ShaderPdf = Shader.Pdf(Sample.Intersection.Wo, Wi);

	if (Li.IsBlack() || F.IsBlack() || ShaderPdf <= 0.0f)
	{
	}
	else
	{
		const float LightPdf = DistanceSquared(SS.P, Sample.Intersection.P) / (AbsDot(-Wi, SS.N) * Light.Shape.Area);

		const float Weight = PowerHeuristic(1, LightPdf, 1, ShaderPdf);

		ColorXYZf Ld;

		if (Shader.Type == Enums::Brdf)
			Ld = F * Li * (AbsDot(Wi, Sample.Intersection.N) * Weight / LightPdf);
		else
			Ld = F * ((Li * Weight) / LightPdf);

		Ld *= (float)gpTracer->LightIDs.Count;

		float Alpha = 0.0f;

		R.MinT += RNG.Get1() * gStepFactorShadow;

		while (R.MinT <= R.MaxT)
		{
			Vec3f Ps = R(R.MinT);

			Alpha += (1.0f - Alpha) * gpTracer->GetOpacity(Volume(Ps));

			if (Alpha >= 1.0f)
			{
				Alpha = 1.0f;
				break;
			}

			R.MinT += gStepFactorShadow;
		}

		Ld *= (1 - Alpha);

		Diffuse[0] = Ld[0];
		Diffuse[1] = Ld[1];
		Diffuse[2] = Ld[2];
	}

	// Determine opacity
	const float Opacity = gpTracer->GetOpacity(Intensity) * (gStepFactorPrimary * 200.0f);

	// Compositing
    Estimate[0] += (1.0f - Estimate[3]) * Opacity * Diffuse[0];
    Estimate[1] += (1.0f - Estimate[3]) * Opacity * Diffuse[1];
    Estimate[2] += (1.0f - Estimate[3]) * Opacity * Diffuse[2];
    Estimate[3] += (1.0f - Estimate[3]) * Opacity;

	// Early ray termination
    if (Estimate[3] >= 1.0f)
	{
        Estimate[3] = 1.0f;

		Sample.Ray.MinT = -1.0f;
    }
	
	// Move along ray
	Sample.Ray.MinT += gStepFactorPrimary;

	ColorRGBAuc& DvrProgressive = gpTracer->FrameBuffer.DvrProgressive(IDx, IDy);

	// Output color
	DvrProgressive[0] = Clamp((int)(Estimate[0] * 255.0f), 0, 255);
	DvrProgressive[1] = Clamp((int)(Estimate[1] * 255.0f), 0, 255);
	DvrProgressive[2] = Clamp((int)(Estimate[2] * 255.0f), 0, 255);
	DvrProgressive[3] = Clamp((int)(Estimate[3] * 255.0f), 0, 255);

	DvrProgressive.BlendWithBackground(gpTracer->FrameBuffer.DvrSimple(IDx, IDy));
	*/
}

void DvrSimple(Tracer& Tracer, Statistics& Statistics)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlDvrSimple<<<GridDim, BlockDim>>>()), "DVR Simple");
}

void DvrProgressive(Tracer& Tracer, Statistics& Statistics)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlDvrProgressive<<<GridDim, BlockDim>>>()), "DVR Progressive");
}

}
