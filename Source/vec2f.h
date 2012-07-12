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

/*! \class Vec2f
 * \brief Two dimensional float vector
 */
class EXPOSURE_RENDER_DLL Vec2f : public Vec<float, 2>
{
public:
	/*! Default constructor */
	HOST_DEVICE Vec2f()
	{
		for (int i = 0; i < 2; ++i)
			this->D[i] = 0.0f;
	}

	/*! Constructor with initializing values */
	HOST_DEVICE Vec2f(const float& V1, const float& V2)
	{
		this->D[0] = V1;
		this->D[1] = V2;
	}
	
	/*! Copy constructor */
	HOST_DEVICE Vec2f(const Vec2f& Other)
	{
		for (int i = 0; i < 2; ++i)
			this->D[i] = Other[i];
	}

	/*! Copy constructor */
	HOST_DEVICE Vec2f(const Vec<float, 2>& Other)
	{
		for (int i = 0; i < 2; ++i)
			this->D[i] = Other[i];
	}
	
	/*! Length squared
		* \return Squared length of the vector
	*/
	HOST_DEVICE float LengthSquared(void) const
	{
		return this->D[0] * this->D[0] + this->D[1] * this->D[1];
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
	}

	/*! Normalized
		* \return Normalized vector
	*/
	HOST_DEVICE Vec2f Normalized(void) const
	{
		// Compute length reciprocal
		const float InvL = 1.0f / this->Length();

		return Vec2f(this->D[0] * InvL, this->D[1] * InvL);
	}

	/*! Negate operator
		* \return Negated vector by value
	*/
	HOST_DEVICE Vec2f operator - () const
	{
		Vec2f Result;
		
		for (int i = 0; i < 2; ++i)
			Result[i] = -this->D[i];
		
		return Result;
	}
};

/*! Multiply Vec2f with float
	@param[in] V Input vector
	@param[in] F Input float
	* \return V x F
*/
static inline HOST_DEVICE Vec2f operator * (const Vec2f& V, const float& F)
{
	return Vec2f(V[0] * F, V[1] * F);
};

/*! Multiply float with Vec2f
	@param[in] F Input float
	@param[in] V Input vector
	* \return F x V
*/
static inline HOST_DEVICE Vec2f operator * (const float& F, const Vec2f& V)
{
	return Vec2f(V[0] * F, V[1] * F);
};

/*! Multiply two Vec2f vectors
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return A x B
*/
static inline HOST_DEVICE Vec2f operator * (const Vec2f& A, const Vec2f& B)
{
	return Vec2f(A[0] * B[0], A[1] * B[1]);
};

/*! Subtract two Vec2f vectors
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return A - B
*/
static inline HOST_DEVICE Vec2f operator - (const Vec2f& A, const Vec2f& B)
{
	return Vec2f(A[0] - B[0], A[1] - B[1]);
};

/*! Add two Vec2f vectors
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return A + B
*/
static inline HOST_DEVICE Vec2f operator + (const Vec2f& A, const Vec2f& B)
{
	return Vec2f(A[0] + B[0], A[1] + B[1]);
};

/*! Divide Vec2f by float value
	@param[in] V Input vector
	@param[in] F Input float
	* \return V / F
*/
static inline HOST_DEVICE Vec2f operator / (const Vec2f& V, const float& F)
{
	// Compute F reciprocal, slightly faster
	const float InvF = (F == 0.0f) ? 0.0f : 1.0f / F;

	return Vec2f(V[0] * InvF, V[1] * InvF);
};

/*! Divide float by Vec2f
	@param[in] F Input float
	@param[in] V Input vector
	* \return F / V
*/
static inline HOST_DEVICE Vec2f operator / (const float& F, const Vec2f& V)
{
	return Vec2f(F / V[0], F / V[1]);
};

/*! Length squared
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return Squared length of the vector
*/
static inline HOST_DEVICE float LengthSquared(const Vec2f& A, const Vec2f& B)
{
	return Vec2f(A - B).LengthSquared();
};

/*! Length
	@param[in] V Input vector
	* \return Length of the vector
*/
static inline HOST_DEVICE float Length(const Vec2f& V)
{
	return V.Length();
};

/*! Normalize
	@param[in] V Input vector
	* \return Normalized vector
*/
static inline HOST_DEVICE Vec2f Normalize(const Vec2f& V)
{
	return V.Normalized();
};

/*! Linearly interpolate two Vec2f vectors
	@param[in] LerpC Interpolation coefficient
	@param[in] A Input vector A
	@param[in] B Input vector B
	* \return Interpolated vector
*/
HOST_DEVICE inline Vec2f Lerp(const float& LerpC, const Vec2f& A, const Vec2f& B)
{
	return (1.0f - LerpC) * A + LerpC * B;
}

}
