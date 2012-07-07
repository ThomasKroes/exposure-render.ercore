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

inline DEVICE ColorXYZf Phong(Vec3f sample, ColorXYZf color, Vec3f gradient, Vec3f lightPos, Vec3f cameraPos)
{
    Vec3f N = Normalize(gradient);
    Vec3f L = Normalize(lightPos - sample);
    Vec3f V = Normalize(cameraPos - sample);

//         float3 shadedColor = getAmbientTerm(color);
//         shadedColor += getDiffuseTerm(color, N, L);
//        shadedColor += getSpecularTerm(color, N, L, V);

    float shade = 0.3f;
    float NdotL = max(Dot(N, L), 0.0f);
    shade += NdotL * 0.4f;

    Vec3f H = Normalize(V + L);                           //opt: nearly 50%!
    float NdotH = __powf(max(Dot(N, H), 0.0f), 60.f);
    shade += NdotH * 0.5f;

    return color * shade;
}

KERNEL void KrnlDvr()
{
	KERNEL_2D(gpTracer->FrameBuffer.Resolution[0], gpTracer->FrameBuffer.Resolution[1])

	// Initialize the random number generator
	RNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(IDx, IDy), &gpTracer->FrameBuffer.RandomSeeds2(IDx, IDy));
	
	ColorRGBAuc& DVR = gpTracer->FrameBuffer.DVR(IDx, IDy);

	DVR = ColorRGBAuc::Black();

	// Camera ray
	Ray R;

	// Generate
	gpTracer->Camera.Sample(R, Vec2i(IDx, IDy), RNG);
	
	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[0]];

	if (!Volume.BoundingBox.Intersect(R, R.MinT, R.MaxT))
		return;
	
	ColorXYZAf result = ColorXYZAf::Black();

	R.MinT += RNG.Get1() * gStepFactorPrimary;

	int NoSamples = 0;

    while (R.MinT <= R.MaxT && NoSamples < 300)
	{
		// Get sample point
        const Vec3f P = R(R.MinT);

		// Obtain gradient with central differences
//		const Vec3f N = Volume.GradientCD(P);
		
		// Obtain intensity
        const float Intensity = Volume(P);

		// Move along ray
		R.MinT += gStepFactorPrimary;
		
		ColorXYZf Diffuse = gpTracer->GetDiffuse(Intensity);

//		if (NoSamples % 10 == 0)
//		{
//			for (int i = 0; i < gpTracer->LightIDs.Count; i++)
//			{
//				Object& Light = gpObjects[gpTracer->LightIDs[i]];

//				const Vec3f LightP(1.0f);// = Light.Shape.Transform.GetTranslation();

//				Diffuse += Phong(P, gpTracer->GetDiffuse(Intensity), N, LightP, R.O);
				
				/*
				const Vec3f LightDir = LightP - Intersection.P;

				ColorXYZf Li = Light.Multiplier * EvaluateTexture(Light.EmissionTextureID, Vec2f(0.0f, 0.0f));

				if (Light.EmissionUnit == Enums::Power)
					Li /= Light.Shape.Area;

				Diffuse += S.F(Intersection.Wo, Normalize(LightDir)) * Li;
				*/
//			}
//		}

/**/
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

		const float Opacity = gpTracer->GetOpacity(Intensity)  * (gStepFactorPrimary * 200.0f);

		// Compositing
        result[0] = result[0] + (1.0f - result[3]) * Opacity * Diffuse[0];
        result[1] = result[1] + (1.0f - result[3]) * Opacity * Diffuse[1];
        result[2] = result[2] + (1.0f - result[3]) * Opacity * Diffuse[2];
        result[3] = result[3] + (1.0f - result[3]) * Opacity;

		NoSamples++;

		// Early ray termination
        if (result[3] >= 1.0f)
		{
            result[3]	= 1.0f;
            break;
        }
    }

	DVR[0] = Clamp((int)(result[0] * 255.0f), 0, 255);
	DVR[1] = Clamp((int)(result[1] * 255.0f), 0, 255);
	DVR[2] = Clamp((int)(result[2] * 255.0f), 0, 255);
	DVR[3] = Clamp((int)(result[3] * 255.0f), 0, 255);
}

void Dvr(Tracer& Tracer, Statistics& Statistics)
{
	LAUNCH_DIMENSIONS(Tracer.FrameBuffer.Resolution[0], Tracer.FrameBuffer.Resolution[1], 1, BLOCK_W, BLOCK_H, 1)
	LAUNCH_CUDA_KERNEL_TIMED((KrnlDvr<<<GridDim, BlockDim>>>()), "DVR");
}

}
