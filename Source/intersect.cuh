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
#include "intersection.h"

namespace ExposureRender
{

DEVICE void IntersectObjects(const Ray& R, Intersection& Int, const int& ScatterTypes = Enums::Object | Enums::Light)
{
	float NearestT = FLT_MAX;

	Intersection LocalInt;

	for (int i = 0; i < gpTracer->ObjectIDs.GetNoIndices(); i++)
	{
		const Object& Object = gpObjects[i];
		
		if (Object.Visible && Object.Shape.Intersect(R, LocalInt) && LocalInt.T < NearestT)
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

DEVICE bool IntersectsObjects(Ray R)
{
	for (int i = 0; i < gpTracer->ObjectIDs.GetNoIndices(); i++)
	{
		if (gpObjects[i].Shape.Intersects(R))
			return true;
	}

	return false;
}

DEVICE bool Intersect(Ray R, RNG& RNG, Intersection& Int, const int& ScatterTypes = Enums::Volume | Enums::Object | Enums::Light)
{
	Intersection Ints[2];
	
	if (ScatterTypes & Enums::Object || ScatterTypes & Enums::Light)
		IntersectObjects(R, Ints[0], ScatterTypes);

	if (ScatterTypes & Enums::Volume)
		IntersectVolume(R, RNG, Ints[1]);
	
	float HitT = FLT_MAX;

	for (int i = 0; i < 2; i++)
	{
		if (Ints[i].Valid && Ints[i].T < HitT)
		{
			Int		= Ints[i];
			HitT	= Ints[i].T;
		}
	}

	return Int.Valid;
}

DEVICE bool Intersects(Ray R, RNG& RNG)
{
	if (IntersectsObjects(R))
		return true;

	if (IntersectsVolume(R, RNG))
		return true;

	return false;
}

}
