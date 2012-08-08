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
#include "surfacesample.h"
#include "geometry.h"

namespace ExposureRender
{

/*! Box shape class */
class EXPOSURE_RENDER_DLL Box
{	
public:
	/*! Default constructor */
	HOST_DEVICE Box() :
		MinP(-0.5f),
		MaxP(0.5f)
	{
	}
	
	/*! Constructor
		@param[in] Size Size of the box
	*/
	HOST_DEVICE Box(const Vec3f& Size) :
		MinP(-0.5f * Size),
		MaxP(0.5f * Size)
	{
	}
	
	/*! Constructor
		@param[in] MinP Minimum point of the box
		@param[in] MaxP Maximum point of the box
	*/
	HOST_DEVICE Box(const Vec3f& MinP, const Vec3f& MaxP) :
		MinP(MinP),
		MaxP(MaxP)
	{
	}
	
	/*! Assignment operator
		@param[in] Other Box to copy
		@return Copied box
	*/
	HOST_DEVICE Box& operator = (const Box& Other)
	{
		this->MinP	= Other.MinP;
		this->MaxP	= Other.MaxP;

		return *this;
	}
	
	/*! Test whether ray \a R intersects the box
		@param[in] R Ray
		@return If \a R intersects the box
	*/
	HOST_DEVICE bool Intersects(const Ray& R) const
	{
		const Vec3f InvR		= Vec3f(1.0f, 1.0f, 1.0f) / R.D;
		const Vec3f BottomT		= InvR * (MinP - R.O);
		const Vec3f TopT		= InvR * (MaxP - R.O);
		const Vec3f MinT		= TopT.Min(BottomT);
		const Vec3f MaxT		= TopT.Max(BottomT);
		const float LargestMinT = max(max(MinT[0], MinT[1]), max(MinT[0], MinT[2]));
		const float LargestMaxT = min(min(MaxT[0], MaxT[1]), min(MaxT[0], MaxT[2]));

		if (LargestMaxT < LargestMinT)
			return false;

		const float NearT = LargestMinT > 0.0f ? LargestMinT : 0.0f;

		if (NearT < R.MinT || NearT > R.MaxT)
			return false;

		return true;
	}
	
	/*! Intersect the box with ray \a R and store the result in \a Int
		@param[in] R Ray
		@param[out] Int Resulting intersection
		@return If \a R intersects the box
	*/
	HOST_DEVICE bool Intersect(const Ray& R, Intersection& Int) const
	{
		const Vec3f InvR		= Vec3f(1.0f, 1.0f, 1.0f) / R.D;
		const Vec3f BottomT		= InvR * (this->MinP - R.O);
		const Vec3f TopT		= InvR * (this->MaxP - R.O);
		const Vec3f MinT		= TopT.Min(BottomT);
		const Vec3f MaxT		= TopT.Max(BottomT);
		const float LargestMinT = max(max(MinT[0], MinT[1]), max(MinT[0], MinT[2]));
		const float LargestMaxT = min(min(MaxT[0], MaxT[1]), min(MaxT[0], MaxT[2]));

		if (LargestMaxT < LargestMinT)
			return false;

		Int.SetT(LargestMinT > 0.0f ? LargestMinT : 0.0f);

		if (Int.GetT() < R.MinT || Int.GetT() > R.MaxT)
			return false;

		Int.SetP(R(Int.GetT()));
		Int.SetN(Vec3f(0.0f));
		Int.SetUV(Vec2f(0.0f, 0.0f));

		for (int i = 0; i < 3; i++)
		{
			if (Int.GetP()[i] <= MinP[i] + 0.0001f)
				Int.GetN()[i] = -1.0f;

			if (Int.GetP()[i] >= MaxP[i] - 0.0001f)
				Int.GetN()[i] = 1.0f;
		}

		return true;
	}

	/*! Intersect the box with ray \a R and output the near \a T0 and far \a T1 distance
		@param[in] R Ray
		@param[out] T0 Nearest intersection, if any
		@param[out] T1 Farthest intersection, if any
		@return If \a R intersects the box
	*/
	HOST_DEVICE bool Intersect(const Ray& R, float& T0, float& T1) const
	{
		const Vec3f InvR		= Vec3f(1.0f, 1.0f, 1.0f) / R.D;
		const Vec3f BottomT		= InvR * (this->MinP - R.O);
		const Vec3f TopT		= InvR * (this->MaxP - R.O);
		const Vec3f MinT		= TopT.Min(BottomT);
		const Vec3f MaxT		= TopT.Max(BottomT);
		const float LargestMinT = max(max(MinT[0], MinT[1]), max(MinT[0], MinT[2]));
		const float LargestMaxT = min(min(MaxT[0], MaxT[1]), min(MaxT[0], MaxT[2]));

		if (LargestMaxT < LargestMinT)
			return false;

		T0 = LargestMinT > 0.0f ? LargestMinT : 0.0f;
		T1 = LargestMaxT;

		T0 = max(T0, R.MinT);
		T1 = min(T1, R.MaxT);

		return true;
	}
	
	/*! Sample the unit box
		@param[out] SS Resulting surface sample
		@param[in] UVW Random sample
	*/
	HOST_DEVICE void SampleUnit(SurfaceSample& SS, const Vec3f& UVW) const
	{
		int Side = (int)floorf(UVW[0] * 6.0f);

		switch (Side)
		{
			case 0:
			{
				SS.P	= Vec3f(0.5f, -0.5f + UVW[2], -0.5f + UVW[1]);
				SS.N	= Vec3f(1.0f, 0.0f, 0.0f);
				break;
			}

			case 1:
			{
				SS.P	= Vec3f(-0.5f, -0.5f + UVW[2], -0.5f + UVW[1]);
				SS.N	= Vec3f(-1.0f, 0.0f, 0.0f);
				break;
			}

			case 2:
			{
				SS.P	= Vec3f(-0.5f + UVW[1], 0.5f, -0.5f + UVW[2]);
				SS.N	= Vec3f(0.0f, 1.0f, 0.0f);
				break;
			}

			case 3:
			{
				SS.P	= Vec3f(-0.5f + UVW[1], -0.5f, -0.5f + UVW[2]);
				SS.N	= Vec3f(0.0f, -1.0f, 0.0f);
				break;
			}

			case 4:
			{
				SS.P	= Vec3f(-0.5f + UVW[1], -0.5f + UVW[2], 0.5f);
				SS.N	= Vec3f(0.0f, 0.0f, 1.0f);
				break;
			}

			case 5:
			{
				SS.P	= Vec3f(-0.5f + UVW[1], -0.5f + UVW[2], -0.5f);
				SS.N	= Vec3f(0.0f, 0.0f, -1.0f);
				break;
			}
		}

		SS.UV = Vec2f(UVW[1], UVW[2]);
	}
	
	/*! Samples the box
		@param[out] SS Resulting surface sample
		@param[in] UVW Random sample
	*/
	HOST_DEVICE void Sample(SurfaceSample& SS, const Vec3f& UVW) const
	{
		SampleUnit(SS, UVW);

		SS.P = this->MinP + SS.P * (this->MaxP - this->MinP);
	}
	
	/*! Computes the surface area of the box
		@return Surface area
	*/
	HOST_DEVICE float GetArea() const
	{
		const Vec3f Size = this->MaxP - this->MinP;
		return (2.0f * Size[0] * Size[1]) + (2.0f * Size[0] * Size[2]) + (2.0f * Size[1] * Size[2]);
	}
	
	/*! Returns if the box is one sided or not
		@return One sided
	*/
	HOST_DEVICE bool GetOneSided() const
	{
		return false;
	}
	
	/*! Test whether point \a P is inside the box
		@return If \a P is inside the box
	*/
	HOST_DEVICE bool Inside(const Vec3f& P) const
	{
		return P[2] > 0.0f;
	}
	
	GET_SET_MACRO(HOST_DEVICE, MinP, Vec3f)
	GET_SET_MACRO(HOST_DEVICE, MaxP, Vec3f)

protected:
	Vec3f	MinP;		/*! Minimum point of the box */
	Vec3f	MaxP;		/*! Maximum point of the box */
};

}
