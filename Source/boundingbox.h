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

#include "vector.h"

namespace ExposureRender
{

/*! Bounding box class */
class EXPOSURE_RENDER_DLL BoundingBox
{
public:
	/*! Default constructor */
	HOST_DEVICE BoundingBox() :
		MinP(FLT_MAX),
		MaxP(FLT_MIN),
		Size(0.0f),
		InvSize(0.0f)
	{
	}

	/*! Constructor
		@param[in] MinP Minimum point of bounding box
		@param[in] MaxP Maximum point of bounding box
	*/
	HOST_DEVICE BoundingBox(const Vec3f& MinP, const Vec3f& MaxP) :
		MinP(MinP),
		MaxP(MaxP),
		Size(MaxP - MinP),
		InvSize(1.0f / Size)
	{
	}
	
	/*! Assignment operator
		@param[in] Other Bounding box to copy
		@result Bounding box
	*/
	HOST_DEVICE BoundingBox& BoundingBox::operator = (const BoundingBox& Other)
	{
		this->MinP		= Other.MinP;	
		this->MaxP		= Other.MaxP;
		this->Size		= Other.Size;
		this->InvSize	= Other.InvSize;

		return *this;
	}
	
	/*! Gets the minimum point of the bounding box
		@result Minimum point
	*/
	HOST_DEVICE Vec3f GetMinP() const
	{
		return this->MinP;
	}
	
	/*! Sets the minimum point of the bounding box
		@param MinP Minimum point
	*/
	HOST_DEVICE void SetMinP(const Vec3f& MinP)
	{
		this->MinP = MinP;
		this->Update();
	}

	/*! Gets the maximum point of the bounding box
		@result Maximum point
	*/
	HOST_DEVICE Vec3f GetMaxP() const
	{
		return this->MaxP;
	}
	
	/*! Sets the maximum point of the bounding box
		@param MaxP Maximum point
	*/
	HOST_DEVICE void SetMaxP(const Vec3f& MaxP)
	{
		this->MaxP = MaxP;
		this->Update();
	}

	/*! Updates the bounding box */
	HOST_DEVICE void Update()
	{
		this->Size		= this->MaxP - this->MinP,
		this->InvSize	= 1.0f / Size;
	}
	
	/*! Intersects the bounding box with a ray
		@param R Ray
		@param T0 Nearest hit distance
		@param T1 Farthest hit distance
		@result Whether \a R intersects the bounding box or not
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

	GET_SET_MACRO(HOST_DEVICE, Size, Vec3f);
	GET_SET_MACRO(HOST_DEVICE, InvSize, Vec3f);

protected:
	Vec3f	MinP;			/*! Minimum point of the bounding box */
	Vec3f	MaxP;			/*! Maximum point of the bounding box */
	Vec3f	Size;			/*! Size of the bounding box */
	Vec3f	InvSize;		/*! Inverse size of the bounding box */
};

}