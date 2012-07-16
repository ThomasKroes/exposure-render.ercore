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

namespace ExposureRender
{

/*! Plane shape class */
class EXPOSURE_RENDER_DLL Plane
{	
public:
	/*! Default constructor */
	HOST_DEVICE Plane() :
		Size(1.0f),
		OneSided(false)
	{
	}
	
	/*! Constructor
		@param[in] Size Size of the plane
		@param[in] OneSided Whether the plane is one sided or not
	*/
	HOST_DEVICE Plane(const Vec2f& Size, const bool& OneSided) :
		Size(Size),
		OneSided(OneSided)
	{
	}
	
	/*! Assignment operator
		@param[in] Other Plane to copy
		@return Copied plane
	*/
	HOST_DEVICE Plane& operator = (const Plane& Other)
	{
		this->Size		= Other.Size;
		this->OneSided	= Other.OneSided;

		return *this;
	}
	
	/*! Test whether ray \a R intersects the plane
		@param[in] R Ray
		@return If \a R intersects the plane
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

		if (Int.GetUV()[0] < -0.5f * this->Size[0] || Int.GetUV()[0] > 0.5f * this->Size[0] || Int.GetUV()[1] < -0.5f * this->Size[1] || Int.GetUV()[1] > 0.5f * this->Size[1])
			return false;
		
		return true;
	}
	
	/*! Intersect the plane with ray \a R and store the result in \a Int
		@param[in] R Ray
		@param[out] Int Resulting intersection
		@return If \a R intersects the plane
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
		Int.SetN(this->OneSided && R.D[2] >= 0.0f ? Vec3f(0.0f, 0.0f, -1.0f) : Vec3f(0.0f, 0.0f, 1.0f));
		Int.SetFront(this->OneSided && R.D[2] >= 0.0f ? false : true);

		if (Int.GetUV()[0] < -0.5f * this->Size[0] || Int.GetUV()[0] > 0.5f * this->Size[0] || Int.GetUV()[1] < -0.5f * this->Size[1] || Int.GetUV()[1] > 0.5f * this->Size[1])
			return false;

		Int.GetUV()[0] /= this->Size[0];
		Int.GetUV()[1] /= this->Size[1];

		Int.GetUV() += Vec2f(0.5f);
		Int.GetUV()[0] = 1.0f - Int.GetUV()[0];

		return true;
	}
	
	/*! Samples the plane
		@param[out] SS Resulting surface sample
		@param[in] UVW Random sample
	*/
	HOST_DEVICE void Sample(SurfaceSample& SS, const Vec3f& UVW) const
	{
		SS.P 	= Vec3f(-0.5f + UVW[0], -0.5f + UVW[1], 0.0f);
		SS.N 	= Vec3f(0.0f, 0.0f, 1.0f);
		SS.UV	= Vec2f(UVW[0], UVW[1]);

		SS.P *= Vec3f(this->Size[0], this->Size[1], 0.0f);
	}
	
	/*! Computes the surface area of the plane
		@param[in] Surface area
	*/
	HOST_DEVICE float GetArea() const
	{
		return this->Size[0] * this->Size[1];
	}
	
	/*! Returns if the plane is one sided or not
		@return If the plane is one sided
	*/
	HOST_DEVICE bool GetOneSided() const
	{
		return this->OneSided;
	}
	
	/*! Test whether point \a P is inside the plane
		@return If \a P is inside the plane
	*/
	HOST_DEVICE bool Inside(const Vec3f& P) const
	{
		return P[2] < 0.0f;
	}
	
	GET_SET_MACRO(HOST_DEVICE, Size, Vec2f)

protected:
	Vec2f	Size;			/*! Size of the plane */
	bool	OneSided;		/*! One sided */
};

}
