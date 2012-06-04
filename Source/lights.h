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

#include "light.h"
#include "textures.h"
#include "shapes.h"

namespace ExposureRender
{

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

HOST_DEVICE_NI void IntersectLight(const Light& Light, const Ray& R, ScatterEvent& SE)
{
	Intersection Int;

	Light.Shape.Intersect(R, Int);

	if (Int.Valid)
	{
		SE.Valid	= true;
		SE.T 		= Length(Int.P - R.O);
		SE.P		= Int.P;
		SE.N		= Int.N;
		SE.Wo		= -R.D;
		SE.UV		= Int.UV;
		SE.Le		= Int.Front ? Light.Multiplier * EvaluateTexture(Light.TextureID, SE.UV) : ColorXYZf::Black();
		
		if (Light.EmissionUnit == Enums::Power)
			SE.Le /= Light.Shape.Area;
	}
}

HOST_DEVICE_NI void IntersectLights(const Ray& R, ScatterEvent& RS, bool RespectVisibility = false)
{
	float T = FLT_MAX; 

	for (int i = 0; i < gpTracer->LightIDs.Count; i++)
	{
		const Light& Light = gpLights[gpTracer->LightIDs[i]];
		
		ScatterEvent LocalRS(Enums::Light);

		LocalRS.ID = i;

		if (RespectVisibility && !Light.Visible)
			continue;

		IntersectLight(Light, R, LocalRS);

		if (LocalRS.Valid && LocalRS.T < T)
		{
			RS = LocalRS;
			T = LocalRS.T;
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

}
