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

#include "plane.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL Disk : public Plane
{	
public:
	HOST_DEVICE Disk() :
		Plane(Vec2f(1.0f), true),
		Radius(1.0f)
	{
	}

	HOST_DEVICE Disk(const float& Radius, const bool& OneSided) :
		Plane(Vec2f(2.0f * Radius), OneSided),
		Radius(Radius)
	{
	}

	HOST_DEVICE Disk& operator = (const Disk& Other)
	{
		this->Radius = Other.Radius;

		return *this;
	}

	HOST_DEVICE bool Intersects(const Ray& R) const
	{
		Intersection Int;

		if (fabs(R.O[2] - R.D[2]) < RAY_EPS)
			return false;

		Int.Add((0.0f - R.O[2]) / R.D[2]);
		
		if (Int.HitT[0] < R.MinT || Int.HitT[0] > R.MaxT)
			return false;

		Int.UV		= Vec2f(Int.P[0], Int.P[1]);
		Int.Valid	= true;

		if (Int.Valid && Int.UV.Length() > this->Radius)
			Int.Valid = false;
		
		return Int.Valid;
	}

	HOST_DEVICE void Intersect(const Ray& R, Intersection& Int) const
	{
		if (fabs(R.O[2] - R.D[2]) < RAY_EPS)
			return;

		Int.Add((0.0f - R.O[2]) / R.D[2]);
		
		if (Int.HitT[0] < R.MinT || Int.HitT[0] > R.MaxT)
			return;

		Int.P 	= R(Int.HitT[0]);
		Int.UV	= Vec2f(Int.P[0], Int.P[1]);
		Int.N	= Vec3f(0.0f, 0.0f, 1.0f);

		if (this->OneSided && R.D[2] >= 0.0f)
		{
			Int.Front	= false;
			Int.N		= Vec3f(0.0f, 0.0f, -1.0f);
		}

		Int.Valid = true;

		if (Int.Valid && Int.UV.Length() > this->Radius)
			Int.Valid = false;

		const float Diameter = 2.0f * this->Radius;

		Int.UV /= Diameter;
		Int.UV += Vec2f(0.5f);
		Int.UV[0] = 1.0f - Int.UV[0];
	}

	HOST_DEVICE void Sample(SurfaceSample& SS, const Vec3f& UVW) const
	{
		float r = sqrtf(UVW[0]);
		float theta = 2.0f * PI_F * UVW[1];

		SS.P 	= Vec3f(r * cosf(theta), r * sinf(theta), 0.0f);
		SS.N 	= Vec3f(0.0f, 0.0f, 1.0f);
		SS.UV	= Vec2f(UVW[0], UVW[1]);
		
		SS.P *= this->Radius;
	}

	HOST_DEVICE float GetArea() const
	{
		return PI_F * (this->Radius * this->Radius);
	}

	HOST_DEVICE bool GetOneSided() const
	{
		return Plane::GetOneSided();
	}

protected:
	float	Radius;
};

}
