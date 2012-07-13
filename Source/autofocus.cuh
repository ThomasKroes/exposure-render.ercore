/*
*	@file
*	@author  Thomas Kroes <t.kroes at tudelft.nl>
*	@version 1.0
*	
*	@section LICENSE
*	
*	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*	
*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
*	Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "macros.cuh"
#include "montecarlo.h"
#include "raymarching.h"

namespace ExposureRender
{

KERNEL void KrnlComputeAutoFocusDistance(float* pAutoFocusDistance, Vec2i FilmUV, unsigned int Seed1, unsigned int Seed2)
{
	RNG RNG(&Seed1, &Seed2);

	Ray Rc;

	Intersection Int;

	float Sum = 0.0f, SumWeight = 0.0f;

	for (int i = 0; i < 100; i++)
	{
		Vec2f ScreenPoint;

		ScreenPoint[0] = gpTracer->Camera.Screen[0][0] + (gpTracer->Camera.InvScreen[0] * (float)FilmUV[0]);
		ScreenPoint[1] = gpTracer->Camera.Screen[1][0] + (gpTracer->Camera.InvScreen[1] * (float)FilmUV[1]);

		ScreenPoint += 0.01f * ConcentricSampleDisk(RNG.Get2());

		Rc.O	= gpTracer->Camera.Pos;
		Rc.D	= Normalize(gpTracer->Camera.N + (ScreenPoint[0] * gpTracer->Camera.U) - (ScreenPoint[1] * gpTracer->Camera.V));
		Rc.MinT	= gpTracer->Camera.ClipNear;
		Rc.MaxT	= gpTracer->Camera.ClipFar;

		IntersectVolume(Rc, RNG, Int);

		if (Int.Valid && Int.T > 0.0f)
		{
			Sum += (Int.P - Rc.O).Length();
			SumWeight += 1.0f;
		}
	}

	if (Sum <= 0.0f)
		*pAutoFocusDistance = (Int.P - Rc.O).Length();
	else
		*pAutoFocusDistance = Sum / SumWeight;
}

void ComputeAutoFocusDistance(const Vec2i& FilmUV, float& AutoFocusDistance, Statistics& Statistics)
{
	float* pAutoFocusDistance = NULL;

	Cuda::Allocate(pAutoFocusDistance);

	LAUNCH_CUDA_KERNEL_TIMED((KrnlComputeAutoFocusDistance<<<1, 1>>>(pAutoFocusDistance, FilmUV, rand(), rand())), "Autofocus");
	
	Cuda::MemCopyDeviceToHost(pAutoFocusDistance, &AutoFocusDistance);
	Cuda::Free(pAutoFocusDistance);
}

}