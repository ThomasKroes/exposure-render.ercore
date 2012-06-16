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

#include "geometry.h"
#include "volume.h"
#include "transferfunction.h"
#include "shapes.h"
#include "scatterevent.h"
#include "segment.h"

namespace ExposureRender
{

DEVICE void IntersectVolume(Ray R, RNG& RNG, ScatterEvent& SE, const int& VolumeID = 0)
{
	/*
	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[VolumeID]];

	Intersection Int;

	Box BoundingBox(Volume.BoundingBox.MinP, Volume.BoundingBox.MaxP);

	if (!BoundingBox.Intersect(R, R.MinT, R.MaxT))
		return;

	const float S	= -log(RNG.Get1()) / gDensityScale;
	float Sum		= 0.0f;

	Vec3f P[BLOCK_SIZE];

	__syncthreads();

	float Intensity = 0.0f;

	R.MinT += RNG.Get1() * gStepFactorPrimary;

	while (Sum < S)
	{
		if (R.MinT >= R.MaxT)
			return;

		P[IDt]		= R.O + R.MinT * R.D;
		Intensity	= Volume([IDt], VolumeID);
		Sum			+= gDensityScale * gpTracer->GetOpacity(Intensity) * gStepFactorPrimary;
		R.MinT		+= gStepFactorPrimary;
	}

	SE.SetVolumeScattering(R.MinT, [IDt], Volume.NormalizedGradient([IDt], Enums::CentralDifferences), -R.D, Intensity);
	*/
}

DEVICE bool ScatterEventInVolume(Ray R, RNG& RNG, const int& VolumeID = 0)
{
	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[VolumeID]];
	VolumeProperty& VolumeProperty = gpTracer->VolumeProperty;

	Vec3f Ps;

	Intersection Int;
		
	Box BoundingBox(Volume.BoundingBox.MinP, Volume.BoundingBox.MaxP);

	if (!BoundingBox.Intersect(R, R.MinT, R.MaxT))
		return false;

	const float S				= -log(RNG.Get1());
	float Sum					= 0.0f;

	const float StepSize = VolumeProperty.StepFactorShadow * Volume.MinStep;

	R.MinT += RNG.Get1() * StepSize;

	while (Sum < S)
	{
		if (R.MinT > R.MaxT)
			return false;

		Ps		= R.O + R.MinT * R.D;
		Sum		+= 1000.0f * gpTracer->GetOpacity(Volume(Ps, VolumeID)) * StepSize;
		R.MinT	+= StepSize;
	}

	return true;
}

}
