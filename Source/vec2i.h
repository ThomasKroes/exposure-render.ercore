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

#include "vec.h"

namespace ExposureRender
{											

/*! \class Vec2i
 * \brief Two dimensional integer vector
 */
class EXPOSURE_RENDER_DLL Vec2i : public Vec<int, 2>
{
public:
	/*! Default constructor */
	HOST_DEVICE Vec2i()
	{
		for (int i = 0; i < 2; ++i)
			this->D[i] = 0;
	}

	/*! Constructor with initializing values */
	HOST_DEVICE Vec2i(const int& V1, const int& V2)
	{
		this->D[0] = V1;
		this->D[1] = V2;
	}
	
	/*! Copy constructor */
	HOST_DEVICE Vec2i(const Vec2i& Other)
	{
		for (int i = 0; i < 2; ++i)
			this->D[i] = Other[i];
	}

	/*! Copy constructor */
	HOST_DEVICE Vec2i(const Vec<int, 2>& Other)
	{
		for (int i = 0; i < 2; ++i)
			this->D[i] = Other[i];
	}

	/*! Negate operator
		@result Negated vector by value
	*/
	HOST_DEVICE Vec2i operator - () const
	{
		Vec2i Result;
		
		for (int i = 0; i < 2; ++i)
			Result[i] = -this->D[i];
		
		return Result;
	}
};

/*! Multiply Vec2i with float
	@param[in] V Input vector
	@param[in] F Input float
	@result V x F
*/
static inline HOST_DEVICE Vec2i operator * (const Vec2i& V, const float& F)
{
	return Vec2i((int)((float)V[0] * F), (int)((float)V[1] * F));
};

/*! Multiply float with Vec2i
	@param[in] F Input float 
	@param[in] V Input vector
	@result F x V
*/
static inline HOST_DEVICE Vec2i operator * (const float& F, const Vec2i& V)
{
	return Vec2i((int)((float)V[0] * F), (int)((float)V[1] * F));
};

/*! Multiply two Vec2i vectors
	@param[in] A Input vector A
	@param[in] B Input vector B
	@result A x B
*/
static inline HOST_DEVICE Vec2i operator * (const Vec2i& A, const Vec2i& B)
{
	return Vec2i(A[0] * B[0], A[1] * B[1]);
};

/*! Subtract two Vec2i vectors
	@param[in] A Input vector A
	@param[in] B Input vector B
	@result A - B
*/
static inline HOST_DEVICE Vec2i operator - (const Vec2i& A, const Vec2i& B)
{
	return Vec2i(A[0] - B[0], A[1] - B[1]);
};

/*! Add two Vec2i vectors
	@param[in] A Input vector A
	@param[in] B Input vector B
	@result A + B
*/
static inline HOST_DEVICE Vec2i operator + (const Vec2i& A, const Vec2i& B)
{
	return Vec2i(A[0] + B[0], A[1] + B[1]);
};

/*! Divide Vec2i by float value
	@param[in] V Input vector
	@param[in] F Input float
	@result V / F
*/
static inline HOST_DEVICE Vec2i operator / (const Vec2i& V, const float& F)
{
	// Compute F reciprocal, slightly faster
	const float InvF = (F == 0.0f) ? 0.0f : 1.0f / F;

	return Vec2i((int)((float)V[0] * InvF), (int)((float)V[1] * InvF));
};

/*! Divide float by Vec2i
	@param[in] F Input float
	@param[in] V Input vector
	@result F / V
*/
static inline HOST_DEVICE Vec2i operator / (const float& F, const Vec2i& V)
{
	return Vec2i((int)(F / (float)V[0]), (int)(F / (float)V[1]));
};

/*! Linearly interpolate two Vec2i vectors
	@param[in] LerpC Interpolation coefficient
	@param[in] A Input vector A
	@param[in] B Input vector B
	@result Interpolated vector
*/
HOST_DEVICE inline Vec2i Lerp(const float& LerpC, const Vec2i& A, const Vec2i& B)
{
	return (1.0f - LerpC) * A + LerpC * B;
}

}
