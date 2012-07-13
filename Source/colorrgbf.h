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

/*! \class ColorRGBf
 * \brief RGB float class
 */
class EXPOSURE_RENDER_DLL ColorRGBf : public Vec<float, 3>
{
public:
	/*! Default constructor */
	HOST_DEVICE ColorRGBf()
	{
		for (int i = 0; i < 3; ++i)
			this->D[i] = 0.0f;
	}
	
	/*! Construct and initialize with default value
		* \param Default The default value
	*/
	HOST_DEVICE ColorRGBf(const float& V)
	{
		for (int i = 0; i < 3; ++i)
			this->D[i] = V;
	}

	/*! Constructor with initializing values */
	HOST_DEVICE ColorRGBf(const float& R, const float& G, const float& B)
	{
		this->D[0] = R;
		this->D[1] = G;
		this->D[2] = B;
	}
	
	/*! Copy constructor */
	HOST_DEVICE ColorRGBf(const Vec<float, 3>& Other)
	{
		for (int i = 0; i < 3; ++i)
			this->D[i] = Other[i];
	}

	static HOST_DEVICE ColorRGBf Black()
	{
		return ColorRGBf();
	}

	static HOST_DEVICE ColorRGBf FromXYZf(const float XYZ[3])
	{
		ColorRGBf Result;

		Result[0] =  3.240479f * XYZ[0] - 1.537150f * XYZ[1] - 0.498535f * XYZ[2];
		Result[1] = -0.969256f * XYZ[0] + 1.875991f * XYZ[1] + 0.041556f * XYZ[2];
		Result[2] =  0.055648f * XYZ[0] - 0.204043f * XYZ[1] + 1.057311f * XYZ[2];

		return Result;
	};

	static HOST_DEVICE ColorRGBf FromXYZAf(const float XYZA[4])
	{
		ColorRGBf Result;

		Result[0] =  3.240479f * XYZA[0] - 1.537150f * XYZA[1] - 0.498535f * XYZA[2];
		Result[1] = -0.969256f * XYZA[0] + 1.875991f * XYZA[1] + 0.041556f * XYZA[2];
		Result[2] =  0.055648f * XYZA[0] - 0.204043f * XYZA[1] + 1.057311f * XYZA[2];

		return Result;
	};

	HOST_DEVICE bool IsBlack()
	{
		for (int i = 0; i < 3; i++)
			if (this->D[i] != 0)
				return false;
												
		return true;
	}

	HOST_DEVICE float Luminance() const
	{
		return 0.3f * D[0] + 0.59f * D[1]+ 0.11f * D[2];
	}
};

/*! Multiply ColorRGBf with float
	* \param C Color
	* \param F Float to multiply with
	@result C x F
*/
static inline HOST_DEVICE ColorRGBf operator * (const ColorRGBf& C, const float& F)
{
	return ColorRGBf(C[0] * F, C[1] * F, C[2] * F);
};

/*! Multiply float with ColorRGBf
	* \param C ColorRGBf
	* \param F Float to multiply with
	@result F x C
*/
static inline HOST_DEVICE ColorRGBf operator * (const float& F, const ColorRGBf& C)
{
	return ColorRGBf(C[0] * F, C[1] * F, C[2] * F);
};

/*! Multiply two ColorRGBf vectors
	* \param A Vector A
	* \param B Vector B
	@result A x B
*/
static inline HOST_DEVICE ColorRGBf operator * (const ColorRGBf& A, const ColorRGBf& B)
{
	return ColorRGBf(A[0] * B[0], A[1] * B[1], A[2] * B[2]);
};

/*! Divide ColorRGBf vector by float value
	* \param C ColorRGBf to divide
	* \param F Float to divide with
	@result F / V
*/
static inline HOST_DEVICE ColorRGBf operator / (const ColorRGBf& C, const float& F)
{
	// Compute F reciprocal, slightly faster
	const float InvF = (F == 0.0f) ? 0.0f : 1.0f / F;

	return ColorRGBf((float)C[0] * InvF, (float)C[1] * InvF, (float)C[2] * InvF);
};

/*! Subtract two ColorRGBf vectors
	* \param A Vector A
	* \param B Vector B
	@result A - B
*/
static inline HOST_DEVICE ColorRGBf operator - (const ColorRGBf& A, const ColorRGBf& B)
{
	return ColorRGBf(A[0] - B[0], A[1] - B[1], A[2] - B[2]);
};

/*! Add two ColorRGBf vectors
	* \param A Vector A
	* \param B Vector B
	@result A + B
*/
static inline HOST_DEVICE ColorRGBf operator + (const ColorRGBf& A, const ColorRGBf& B)
{
	return ColorRGBf(A[0] + B[0], A[1] + B[1], A[2] + B[2]);
};

/*! Linearly interpolate two ColorRGBf vectors
	* \param LerpC Interpolation coefficient
	* \param A Vector A
	* \param B Vector B
	@result Interpolated vector
*/
HOST_DEVICE inline ColorRGBf Lerp(const float& LerpC, const ColorRGBf& A, const ColorRGBf& B)
{
	return (1.0f - LerpC) * A + LerpC * B;
}

}
