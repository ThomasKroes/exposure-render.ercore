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
		const float A = Dot(R.D, R.D);
		const float B = 2 * Dot(R.D, R.O);
		const float C = Dot(R.O, R.O) - (this->Radius * this->Radius);

		const float D = B * B - 4 * A * C;
	    
		if (D < 0)
			return false;

		const float SqrtD = sqrtf(D);
		
		float Q = 0.0f;

		if (B < 0)
			Q = 0.5f * (-B - SqrtD);
		else
			Q = 0.5f * (-B + SqrtD);

		float Hit0 = Q / A;
		float Hit1 = C / Q;

		if (Hit0 > Hit1)
		{
			const float TempHit = Hit0;
			Hit0 = Hit1;
			Hit1 = TempHit;
		}

		float NearT = 0.0f;

		if (Hit0 >= R.MinT && Hit0 < R.MaxT)
		{
			NearT = Hit0;
		}
		else
		{
			if (Hit1 >= R.MinT && Hit1 < R.MaxT)
				NearT = Hit1;
			else
				return false;
		}

		if (NearT < R.MinT || NearT > R.MaxT)
			return false;

		return true;
	}

	HOST_DEVICE bool Intersect(const Ray& R, Intersection& Int) const
	{
		const float A = Dot(R.D, R.D);
		const float B = 2 * Dot(R.D, R.O);
		const float C = Dot(R.O, R.O) - (this->Radius * this->Radius);

		const float D = B * B - 4 * A * C;
	    
		if (D < 0)
			return false;

		const float SqrtD = sqrtf(D);
		
		float Q = 0.0f;

		if (B < 0)
			Q = 0.5f * (-B - SqrtD);
		else
			Q = 0.5f * (-B + SqrtD);

		float Hit0 = Q / A;
		float Hit1 = C / Q;

		if (Hit0 > Hit1)
		{
			const float TempHit = Hit0;
			Hit0 = Hit1;
			Hit1 = TempHit;
		}

		if (Hit0 >= R.MinT && Hit0 < R.MaxT)
		{
			Int.T = Hit0;

			if (Hit1 >= R.MinT && Hit1 < R.MaxT)
				Int.T = Hit1;
		}
		else
		{
			if (Hit1 >= R.MinT && Hit1 < R.MaxT)
				Int.T = Hit1;
			else
				return false;
		}

		Int.P		= R(Int.T);
		Int.N		= Normalize(Int.P);
		Int.UV		= SphericalToUV(Int.P);

		return true;
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

	HOST_DEVICE bool Inside(const Vec3f& P) const
	{
		return P[2] > 0.0f;
	}

protected:
	float	Radius;
};

}
