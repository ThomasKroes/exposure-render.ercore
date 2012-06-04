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

#include "intersection.h"
#include "sample.h"

namespace ExposureRender
{

// http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection#Example_Code

class EXPOSURE_RENDER_DLL Sphere
{	
public:
	HOST_DEVICE Sphere() :
		Radius(1.0f)
	{
	}

	HOST_DEVICE Sphere(const float& Radius) :
		Radius(Radius)
	{
	}

	HOST_DEVICE Sphere& operator = (const Sphere& Other)
	{
		this->Radius	= Other.Radius;

		return *this;
	}

	HOST_DEVICE bool Intersects(const Ray& R) const
	{
		// Compute A, B and C coefficients
		float a = Dot(R.D, R.D);
		float b = 2 * Dot(R.D, R.O);
		float c = Dot(R.O, R.O) - (this->Radius * this->Radius);

		//Find discriminant
		const float disc = b * b - 4 * a * c;
	    
		// if discriminant is negative there are no real roots, so return false, as ray misses sphere
		if (disc < 0)
			return false;

		// compute q as described above
		float distSqrt = sqrtf(disc);
		float q;

		if (b < 0)
			q = (-b - distSqrt) / 2.0f;
		else
			q = (-b + distSqrt) / 2.0f;

		// compute t0 and t1
		float t0 = q / a;
		float t1 = c / q;

		// make sure t0 is smaller than t1
		if (t0 > t1)
		{
			// if t0 is bigger than t1 swap them around
			float temp = t0;
			t0 = t1;
			t1 = temp;
		}

		float NearT;

		if (t0 >= R.MinT && t0 < R.MaxT)
		{
			NearT = t0;
		}
		else
		{
			if (t1 >= R.MinT && t1 < R.MaxT)
				NearT = t1;
			else
				return false;
		}

		if (NearT < R.MinT || NearT > R.MaxT)
			return false;

		return true;
	}

	HOST_DEVICE void Intersect(const Ray& R, Intersection& Int) const
	{
		// Compute A, B and C coefficients
		float a = Dot(R.D, R.D);
		float b = 2 * Dot(R.D, R.O);
		float c = Dot(R.O, R.O) - (this->Radius * this->Radius);

		//Find discriminant
		const float disc = b * b - 4 * a * c;
	    
		// if discriminant is negative there are no real roots, so return false, as ray misses sphere
		if (disc < 0)
			return;

		// compute q as described above
		float distSqrt = sqrtf(disc);
		float q;

		if (b < 0)
			q = (-b - distSqrt) / 2.0f;
		else
			q = (-b + distSqrt) / 2.0f;

		// compute t0 and t1
		float t0 = q / a;
		float t1 = c / q;

		// make sure t0 is smaller than t1
		if (t0 > t1)
		{
			// if t0 is bigger than t1 swap them around
			float temp = t0;
			t0 = t1;
			t1 = temp;
		}


		if (t0 >= R.MinT && t0 < R.MaxT)
		{
			Int.HitT[0] = t0;
		}
		else
		{
			if (t1 >= R.MinT && t1 < R.MaxT)
				Int.HitT[1] = t1;
			else
				return;
		}

		if (Int.HitT[0] < R.MinT || Int.HitT[0] > R.MaxT)
			return;

		Int.Valid	= true;
		Int.P		= R(Int.HitT[0]);
		Int.N		= Normalize(Int.P);
		Int.UV		= SphericalToUV(Int.P);
	}

	HOST_DEVICE void SampleUnit(SurfaceSample& SS, const Vec3f& UVW) const
	{
		float z		= 1.0f - 2.0f * UVW[0];
		float r		= sqrtf(max(0.0f, 1.0f - z * z));
		float phi	= 2.0f * PI_F * UVW[1];
		float x		= r * cosf(phi);
		float y		= r * sinf(phi);

		SS.P	= Vec3f(x, y, z);
		SS.N	= SS.P;
		SS.UV	= Vec2f(SphericalTheta(SS.P), SphericalPhi(SS.P));
	}

	HOST_DEVICE void Sample(SurfaceSample& SS, const Vec3f& UVW) const
	{
		SampleUnit(SS, UVW);

		SS.P *= this->Radius;
	}

	HOST_DEVICE float GetArea() const
	{
		return 4.0f * PI_F * (this->Radius * this->Radius);
	}

	HOST_DEVICE bool GetOneSided() const
	{
		return false;
	}

protected:
	float	Radius;
};


/*


HOST_DEVICE void IntersectUnitSphere(const Ray& R, Intersection& Int)
{
	IntersectSphere(R, 1.0f, Int);
}


HOST_DEVICE bool InsideSphere(const Vec3f& P, const float& Radius)
{
	return Length(P) < Radius;
}
*/


}
