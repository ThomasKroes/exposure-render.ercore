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
#include "clippingobjects.h"

namespace ExposureRender
{

DEVICE void SampleVolume(Ray R, CRNG& RNG, ScatterEvent& SE, const int& VolumeID = 0)
{
	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[VolumeID]];
	VolumeProperty& VolumeProperty = gpTracer->VolumeProperty;

	float MinT;
	float MaxT;
	
	Intersection Int;

	Box BoundingBox(Volume.BoundingBox.MinP, Volume.BoundingBox.MaxP);

	BoundingBox.Intersect(R, Int);

	if (!Int.Valid)
		return;

	MinT = max(Int.HitT[0], R.MinT);
	MaxT = min(Int.HitT[1], R.MaxT);

	const float S	= -log(RNG.Get1()) / VolumeProperty.DensityScale;
	float Sum		= 0.0f;

	Vec3f Ps;

	const float StepSize = VolumeProperty.StepFactorPrimary * Volume.MinStep;

	MinT += RNG.Get1() * StepSize;

	float Intensity = 0.0f;

	while (Sum < S)
	{
		Ps = R.O + MinT * R.D;

		if (MinT >= MaxT)
			return;
		
		Intensity	= Volume(Ps, VolumeID);
		Sum			+= VolumeProperty.DensityScale * VolumeProperty.Opacity1D.Evaluate(Intensity) * StepSize;
		MinT		+= StepSize;
	}

	SE.SetVolumeScattering(MinT, Ps, Volume.NormalizedGradient(Ps, VolumeProperty.GradientMode), -R.D, Intensity);
}

DEVICE bool ScatterEventInVolume(Ray R, CRNG& RNG, const int& VolumeID = 0)
{
	Volume& Volume = gpVolumes[gpTracer->VolumeIDs[VolumeID]];
	VolumeProperty& VolumeProperty = gpTracer->VolumeProperty;

	float MinT;
	float MaxT;

	Vec3f Ps;

	Intersection Int;
		
	Box BoundingBox(Volume.BoundingBox.MinP, Volume.BoundingBox.MaxP);

	BoundingBox.Intersect(R, Int);
	
	if (!Int.Valid)
		return false;

	MinT = max(Int.HitT[0], R.MinT);
	MaxT = min(Int.HitT[1], R.MaxT);

	const float	DensityScale	= VolumeProperty.DensityScale;
	const float S				= -log(RNG.Get1());
	float Sum					= 0.0f;

	const float StepSize = VolumeProperty.StepFactorShadow * Volume.MinStep;

	MinT += RNG.Get1() * StepSize;

	while (Sum < S)
	{
		Ps = R.O + MinT * R.D;

		if (MinT > MaxT)
			return false;
		
		Sum		+= DensityScale * VolumeProperty.Opacity1D.Evaluate(Volume(Ps, VolumeID)) * StepSize;
		MinT	+= StepSize;
	}

	return true;
}

}
