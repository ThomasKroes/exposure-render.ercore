/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or witDEVut modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software witDEVut specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT DEVLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT DEVLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) DEVWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "vec.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL Vec3f : public Vec<float, 3>
{
public:
	/*! Constructor with initializing values */
	HOST_DEVICE Vec3f(const float& V1, const float& V2, const float& V3)
	{
		this->D[0] = V1;
		this->D[1] = V2;
		this->D[2] = V3;
	}
	
	/*! Copy constructor */
	HOST_DEVICE Vec3f(const Vec<float, 3>& Other)
	{
		for (int i = 0; i < 3; ++i)
			this->D[i] = Other[i];
	}

	HOST_DEVICE float LengthSquared(void) const
	{
		return this->D[0] * this->D[0] + this->D[1] * this->D[1] + this->D[2] * this->D[2];
	}

	HOST_DEVICE float Length(void) const
	{
		return sqrtf(this->LengthSquared());
	}

	HOST_DEVICE void Normalize(void)
	{
		const float L = this->Length();
		this->D[0] /= L;
		this->D[1] /= L;
		this->D[2] /= L;
	}

	HOST_DEVICE float Dot(const Vec3f& V) const
	{
		return (this->D[0] * V[0] + this->D[1] * V[1] + this->D[2] * V[2]);
	}

	HOST_DEVICE Vec3f Cross(const Vec3f& V) const
	{
		return Vec3f( (this->D[1] * V[2]) - (this->D[2] * V[1]), (this->D[2] * V[0]) - (this->D[0] * V[2]), (this->D[0] * V[1]) - (this->D[1] * V[0]) );
	}

	HOST_DEVICE void ScaleBy(float F)
	{
		this->D[0] *= F;
		this->D[1] *= F;
		this->D[2] *= F;
	}
};

static inline HOST_DEVICE Vec3f operator * (const float& F, const Vec3f& V)					{ return Vec3f(V[0] * F, V[1] * F, V[2] * F);								};

static inline HOST_DEVICE Vec3f Normalize(const Vec3f& V)									{ Vec3f R = V; R.Normalize(); return R; 									};
static inline HOST_DEVICE float Length(const Vec3f& V)										{ return V.Length();														};
static inline HOST_DEVICE Vec3f Cross(const Vec3f& A, const Vec3f& B)						{ return A.Cross(B);														};
static inline HOST_DEVICE float Dot(const Vec3f& A, const Vec3f& B)							{ return A.Dot(B);															};
static inline HOST_DEVICE float AbsDot(const Vec3f& A, const Vec3f& B)						{ return fabs(A.Dot(B));													};
static inline HOST_DEVICE float ClampedAbsDot(const Vec3f& A, const Vec3f& B)				{ return Clamp(fabs(A.Dot(B)), 0.0f, 1.0f);									};
static inline HOST_DEVICE float ClampedDot(const Vec3f& A, const Vec3f& B)					{ return Clamp(Dot(A, B), 0.0f, 1.0f);										};
static inline HOST_DEVICE float Distance(const Vec3f& A, const Vec3f& B)					{ return (A - B).Length();													};
static inline HOST_DEVICE float DistanceSquared(const Vec3f& A, const Vec3f& B)				{ return (A - B).LengthSquared();											};
static inline HOST_DEVICE Vec3f Lerp(const Vec3f& A, const Vec3f& B, const float& LerpC)	{ return A + LerpC * (B - A);												};

}
