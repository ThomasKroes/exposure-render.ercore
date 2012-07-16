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

#include "plane.h"

namespace ExposureRender
{

/*! Ring shape class */
class EXPOSURE_RENDER_DLL Ring : public Plane
{	
public:
	/*! Default constructor */
	HOST_DEVICE Ring() :
		Plane(Vec2f(1.0f), true),
		InnerRadius(0.5f),
		OuterRadius(1.0f)
	{
	}
	
	/*! Constructor
		@param[in] InnerRadius Inner radius of the ring
		@param[in] OuterRadius Outer radius of the ring
		@param[in] OneSided Whether the ring is one sided or not
	*/
	HOST_DEVICE Ring(const float& InnerRadius, const float& OuterRadius, const bool& OneSided) :
		Plane(Vec2f(2.0f * OuterRadius), OneSided),
		InnerRadius(InnerRadius),
		OuterRadius(OuterRadius)
	{
	}
	
	/*! Assignment operator
		@param[in] Other Ring to copy
		@return Copied ring
	*/
	HOST_DEVICE Ring& operator = (const Ring& Other)
	{
		Plane::operator = (Other);

		this->InnerRadius	= Other.InnerRadius;
		this->OuterRadius	= Other.OuterRadius;

		return *this;
	}
	
	/*! Test whether ray \a R intersects the ring
		@param[in] R Ray
		@return If \a R intersects the ring
	*/
	HOST_DEVICE bool Intersects(const Ray& R) const
	{
		Intersection Int;

		if (fabs(R.O[2] - R.D[2]) < RAY_EPS)
			return false;

		Int.SetT((0.0f - R.O[2]) / R.D[2]);
		
		if (Int.GetT() < R.MinT || Int.GetT() > R.MaxT)
			return false;

		Int.SetUV(Vec2f(Int.GetP()[0], Int.GetP()[1]));

		if (Int.GetUV().Length() < InnerRadius || Int.GetUV().Length() >= 1.0f)
			return false;
		
		return true;
	}

	/*! Intersect the ring with ray \a R and store the result in \a Int
		@param[in] R Ray
		@param[out] Int Resulting intersection
		@return If \a R intersects the ring
	*/
	HOST_DEVICE bool Intersect(const Ray& R, Intersection& Int) const
	{
		if (fabs(R.O[2] - R.D[2]) < RAY_EPS)
			return false;

		Int.SetT((0.0f - R.O[2]) / R.D[2]);
		
		if (Int.GetT() < R.MinT || Int.GetT() > R.MaxT)
			return false;

		Int.SetP(R(Int.GetT()));
		Int.SetUV(Vec2f(Int.GetP()[0], Int.GetP()[1]));
		Int.SetN(Vec3f(0.0f, 0.0f, 1.0f));

		if (this->OneSided && R.D[2] >= 0.0f)
		{
			Int.SetFront(false);
			Int.SetN(Vec3f(0.0f, 0.0f, -1.0f));
		}

		if (Int.GetUV().Length() < this->InnerRadius || Int.GetUV().Length() > this->OuterRadius)
			return false;

		const float Diameter = 2.0f * this->OuterRadius;

		Int.GetUV() /= Diameter;
		Int.GetUV() += Vec2f(0.5f);
		Int.GetUV()[0] = 1.0f - Int.GetUV()[0];

		return true;
	}

	/*! Sample the unit ring
		@param[out] SS Resulting surface sample
		@param[in] UVW Random sample
		@param[in] InnerRadius Inner radius of the ring
	*/
	HOST_DEVICE void SampleUnit(SurfaceSample& SS, const Vec3f& UVW, const float& InnerRadius) const
	{
		const float r = InnerRadius + (1.0f - InnerRadius) * sqrtf(UVW[0]);
		const float theta = 2.0f * PI_F * UVW[1];

		SS.P	= Vec3f(r * cosf(theta), r * sinf(theta), 0.0f);
		SS.N	= Vec3f(0.0f, 0.0f, 1.0f);
		SS.UV	= Vec2f(SS.P[0], SS.P[1]);
	}
	
	/*! Samples the ring
		@param[out] SS Resulting surface sample
		@param[in] UVW Random sample
	*/
	HOST_DEVICE void Sample(SurfaceSample& SS, const Vec3f& UVW) const
	{
		this->SampleUnit(SS, UVW, this->InnerRadius / this->OuterRadius);

		SS.P *= this->OuterRadius;
		SS.UV = Vec2f(SS.P[0], SS.P[1]);
	}
	
	/*! Computes the surface area of the ring
		@param[in] Surface area
	*/
	HOST_DEVICE float GetArea() const
	{
		return (PI_F * (this->OuterRadius * this->OuterRadius)) - (PI_F * (this->InnerRadius * this->InnerRadius));
	}
	
	/*! Returns if the ring is one sided
		@return One sided
	*/
	HOST_DEVICE bool GetOneSided() const
	{
		return Plane::GetOneSided();
	}
	
	/*! Test whether point \a P is inside the ring
		@return If \a P is inside the ring
	*/
	HOST_DEVICE bool Inside(const Vec3f& P) const
	{
		return P[2] > 0.0f;
	}
	
	GET_SET_MACRO(HOST_DEVICE, InnerRadius, float)
	GET_SET_MACRO(HOST_DEVICE, OuterRadius, float)

protected:
	float	InnerRadius;		/*! Inner radius of the ring */
	float	OuterRadius;		/*! Outer radius of the ring */
};

}
