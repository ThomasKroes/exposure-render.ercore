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

/*! \class Vec3f
 * \brief Three dimensional float vector
 */
class EXPOSURE_RENDER_DLL Vec3f : public Vec<float, 3>
{
public:
	/*! Default constructor */
	HOST_DEVICE Vec3f()
	{
		for (int i = 0; i < 3; ++i)
			this->D[i] = 0.0f;
	}

	/*! Constructor with initializing values */
	HOST_DEVICE Vec3f(const float& V1, const float& V2, const float& V3)
	{
		this->D[0] = V1;
		this->D[1] = V2;
		this->D[2] = V3;
	}
	
	/*! Copy constructor */
	HOST_DEVICE Vec3f(const Vec3f& Other)
	{
		for (int i = 0; i < 3; ++i)
			this->D[i] = Other[i];
	}

	/*! Copy constructor */
	HOST_DEVICE Vec3f(const Vec<float, 3>& Other)
	{
		for (int i = 0; i < 3; ++i)
			this->D[i] = Other[i];
	}
	
	/*! Length squared
		* \return Squared length of the vector
	*/
	HOST_DEVICE float LengthSquared(void) const
	{
		return this->D[0] * this->D[0] + this->D[1] * this->D[1] + this->D[2] * this->D[2];
	}
	
	/*! Length
		* \return Length of the vector
	*/
	HOST_DEVICE float Length(void) const
	{
		return sqrtf(this->LengthSquared());
	}
	
	/*! Normalizes the vector */
	HOST_DEVICE void Normalize(void)
	{
		// Compute length reciprocal
		const float InvL = 1.0f / this->Length();

		this->D[0] *= InvL;
		this->D[1] *= InvL;
		this->D[2] *= InvL;
	}
	
	/*! Normalized
		* \return Normalized vector
	*/
	HOST_DEVICE Vec3f Normalized(void) const
	{
		// Compute length reciprocal
		const float InvL = 1.0f / this->Length();

		return Vec3f(this->D[0] * InvL, this->D[1] * InvL, this->D[2] * InvL);
	}

	/*! Dot product
		@param[in] V Input vector
		* \return Dot product
	*/
	HOST_DEVICE float Dot(const Vec3f& V) const
	{
		return (this->D[0] * V[0] + this->D[1] * V[1] + this->D[2] * V[2]);
	}
	
	/*! Cross product
		@param[in] V Input vector
		* \return Cross product
	*/
	HOST_DEVICE Vec3f Cross(const Vec3f& V) const
	{
		return Vec3f( (this->D[1] * V[2]) - (this->D[2] * V[1]), (this->D[2] * V[0]) - (this->D[0] * V[2]), (this->D[0] * V[1]) - (this->D[1] * V[0]) );
	}

	/*! Negate operator
		* \return Negated vector by value
	*/
	HOST_DEVICE Vec3f operator - () const
	{
		Vec3f Result;
		
		for (int i = 0; i < 3; ++i)
			Result[i] = -this->D[i];
		
		return Result;
	}
};

/*! Multiply Vec3f with float
	@param[in] V Input vector
	@param[in] F Input float
	* \return V x F
*/
static inline HOST_DEVICE Vec3f operator * (const Vec3f& V, const float& F)
{
	return Vec3f(V[0] * F, V[1] * F, V[2] * F);
};

/*! Multiply float with Vec3f
	@param[in] V Input vector
	@param[in] F Input float
	* \return F x V
*/
static inline HOST_DEVICE Vec3f operator * (const float& F, const Vec3f& V)
{
	return Vec3f(V[0] * F, V[1] * F, V[2] * F);
};

/*! Multiply two Vec3f vectors
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return A x B
*/
static inline HOST_DEVICE Vec3f operator * (const Vec3f& A, const Vec3f& B)
{
	return Vec3f(A[0] * B[0], A[1] * B[1], A[2] * B[2]);
};

/*! Divide Vec3f by float value
	@param[in] V Input vector
	@param[in] F Input float
	* \return V / F
*/
static inline HOST_DEVICE Vec3f operator / (const Vec3f& V, const float& F)
{
	// Compute F reciprocal, slightly faster
	const float InvF = (F == 0.0f) ? 0.0f : 1.0f / F;

	return Vec3f(V[0] * InvF, V[1] * InvF, V[2] * InvF);
};

/*! Divide float by Vec3f
	@param[in] F Input float
	@param[in] V Input vector
	* \return F / V
*/
static inline HOST_DEVICE Vec3f operator / (const float& F, const Vec3f& V)
{
	return Vec3f(F / V[0], F / V[1], F / V[2]);
};

/*! Subtract two Vec3f vectors
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return A - B
*/
static inline HOST_DEVICE Vec3f operator - (const Vec3f& A, const Vec3f& B)
{
	return Vec3f(A[0] - B[0], A[1] - B[1], A[2] - B[2]);
};

/*! Add two Vec3f vectors
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return A + B
*/
static inline HOST_DEVICE Vec3f operator + (const Vec3f& A, const Vec3f& B)
{
	return Vec3f(A[0] + B[0], A[1] + B[1], A[2] + B[2]);
};

/*! Length squared
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return Squared length of the vector
*/
static inline HOST_DEVICE float LengthSquared(const Vec3f& A, const Vec3f& B)
{
	return (A - B).LengthSquared();
};

/*! Length
	@param[in] V Input vector
	* \return Length of the vector
*/
static inline HOST_DEVICE float Length(const Vec3f& A, const Vec3f& B)
{
	return (A - B).Length();
};

/*! Normalize
	@param[in] V Input vector
	* \return Normalized vector
*/
static inline HOST_DEVICE Vec3f Normalize(const Vec3f& V)
{
	return V.Normalized();
};

/*! Dot product
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return Dot product of A and B
*/
static inline HOST_DEVICE float Dot(const Vec3f& A, const Vec3f& B)
{
	return A.Dot(B);
};

/*! Absolute dot product
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return Absolute dot product of A and B
*/
static inline HOST_DEVICE float AbsDot(const Vec3f& A, const Vec3f& B)
{
	return fabs(A.Dot(B));
};

/*! Clamped dot product
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return Clamped dot product of A and B
*/
static inline HOST_DEVICE float ClampedDot(const Vec3f& A, const Vec3f& B)
{
	return Clamp(Dot(A, B), 0.0f, 1.0f);
};

/*! Clamped absolute dot product
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return Clamped absolute dot product of A and B
*/
static inline HOST_DEVICE float ClampedAbsDot(const Vec3f& A, const Vec3f& B)
{
	return Clamp(fabs(A.Dot(B)), 0.0f, 1.0f);
};

/*! Cross product
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return Cross product of A and B
*/
static inline HOST_DEVICE Vec3f Cross(const Vec3f& A, const Vec3f& B)
{
	return A.Cross(B);
};

/*! Normalized cross product
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return Normalized cross product of A and B
*/
static inline HOST_DEVICE Vec3f NormalizedCross(const Vec3f& A, const Vec3f& B)
{
	return Normalize(A.Cross(B));
};

/*! Linearly interpolate two Vec3f vectors
	@param[in] LerpC Interpolation coefficient
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return Interpolated vector
*/
HOST_DEVICE inline Vec3f Lerp(const float& LerpC, const Vec3f& A, const Vec3f& B)
{
	return (1.0f - LerpC) * A + LerpC * B;
}

}
