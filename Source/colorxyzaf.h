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

/*! \class ColorXYZAf
 * \brief XYZA float class
 */
class EXPOSURE_RENDER_DLL ColorXYZAf : public Vec<float, 4>
{
public:
	/*! Default constructor */
	HOST_DEVICE ColorXYZAf()
	{
		for (int i = 0; i < 4; ++i)
			this->D[i] = 0.0f;
	}
	
	/*! Construct and initialize with default value
		* \param Default The default value
	*/
	HOST_DEVICE ColorXYZAf(const float& V)
	{
		for (int i = 0; i < 4; ++i)
			this->D[i] = V;
	}

	/*! Constructor with initializing values */
	HOST_DEVICE ColorXYZAf(const float& X, const float& Y, const float& Z, const float& A)
	{
		this->D[0] = X;
		this->D[1] = Y;
		this->D[2] = Z;
		this->D[3] = A;
	}

	/*! Copy constructor */
	HOST_DEVICE ColorXYZAf(const Vec<float, 4>& Other)
	{
		for (int i = 0; i < 4; ++i)
			this->D[i] = Other[i];
	}

	static HOST_DEVICE ColorXYZAf Black()
	{
		return ColorXYZAf();
	}

	static HOST_DEVICE ColorXYZAf FromRGBf(const float RGB[3])
	{
		ColorXYZAf Result;

		Result[0] = 0.412453f * RGB[0] + 0.357580f * RGB[1] + 0.180423f * RGB[2];
		Result[1] = 0.212671f * RGB[0] + 0.715160f * RGB[1] + 0.072169f * RGB[2];
		Result[2] = 0.019334f * RGB[0] + 0.119193f * RGB[1] + 0.950227f * RGB[2];
		Result[3] = 1.0f;

		return Result;
	}

	static HOST_DEVICE ColorXYZAf FromRGBuc(const unsigned char RGB[3])
	{
		ColorXYZAf Result;

		const float RGBf[3] = 
		{
			ONE_OVER_255 * (float)RGB[0],
			ONE_OVER_255 * (float)RGB[1],
			ONE_OVER_255 * (float)RGB[2]
		};

		Result[0] = 0.412453f * RGBf[0] + 0.357580f * RGBf[1] + 0.180423f * RGBf[2];
		Result[1] = 0.212671f * RGBf[0] + 0.715160f * RGBf[1] + 0.072169f * RGBf[2];
		Result[2] = 0.019334f * RGBf[0] + 0.119193f * RGBf[1] + 0.950227f * RGBf[2];
		Result[3] = 1.0f;

		return Result;
	}

	static HOST_DEVICE ColorXYZAf FromRGBAuc(const unsigned char RGBA[4])
	{
		ColorXYZAf Result;

		const float RGBAf[3] = 
		{
			ONE_OVER_255 * (float)RGBA[0],
			ONE_OVER_255 * (float)RGBA[1],
			ONE_OVER_255 * (float)RGBA[2]
		};

		Result[0] = 0.412453f * RGBAf[0] + 0.357580f * RGBAf[1] + 0.180423f * RGBAf[2];
		Result[1] = 0.212671f * RGBAf[0] + 0.715160f * RGBAf[1] + 0.072169f * RGBAf[2];
		Result[2] = 0.019334f * RGBAf[0] + 0.119193f * RGBAf[1] + 0.950227f * RGBAf[2];
		Result[3] = RGBA[3];

		return Result;
	}

	HOST_DEVICE void ToneMap(const float& Exposure)
	{
		// Compute reciprocal, which is slightly faster
		const float InvExposure = 1.0f / Exposure;

		// Perform the tone mapping
		for (int i = 0; i < 3; i++)
			this->D[i] = ExposureRender::Clamp(1.0f - expf(-this->D[i] * InvExposure), 0.0f, 1.0f);
	}

	HOST_DEVICE bool IsBlack() const
	{
		for (int i = 0; i < 3; i++)
			if (this->D[i] != 0)
				return false;
												
		return true;
	}

	HOST_DEVICE float Y() const
	{
		float Weight[3] =
		{
			0.212671f, 0.715160f, 0.072169f
		};
		
		float L = 0.0f;
		
		for (int i = 0; i < 3; i++)
			L += Weight[i] * this->D[i];
		
		return L;
	}
};

/*! Multiply ColorXYZAf with float
	* \param C ColorXYZAf
	* \param F Float to multiply with
	* \return C x F
*/
static inline HOST_DEVICE ColorXYZAf operator * (const ColorXYZAf& C, const float& F)
{
	return ColorXYZAf(C[0] * F, C[1] * F, C[2] * F, C[3] * F);
};

/*! Multiply float with ColorXYZAf
	* \param C ColorXYZAf
	* \param F Float to multiply with
	* \return F x C
*/
static inline HOST_DEVICE ColorXYZAf operator * (const float& F, const ColorXYZAf& C)
{
	return ColorXYZAf(C[0] * F, C[1] * F, C[2] * F, C[3] * F);
};

/*! Multiply two ColorXYZAf vectors
	* \param A Vector A
	* \param B Vector B
	* \return A x B
*/
static inline HOST_DEVICE ColorXYZAf operator * (const ColorXYZAf& A, const ColorXYZAf& B)
{
	return ColorXYZAf(A[0] * B[0], A[1] * B[1], A[2] * B[2], A[3] * B[3]);
};

/*! Divide ColorXYZAf vector by float value
	* \param C ColorXYZAf to divide
	* \param F Float to divide with
	* \return F / V
*/
static inline HOST_DEVICE ColorXYZAf operator / (const ColorXYZAf& C, const float& F)
{
	// Compute F reciprocal, slightly faster
	const float InvF = (F == 0.0f) ? 0.0f : 1.0f / F;

	return ColorXYZAf((float)C[0] * InvF, (float)C[1] * InvF, (float)C[2] * InvF, (float)C[3] * InvF);
};

/*! Subtract two ColorXYZAf vectors
	* \param A Vector A
	* \param B Vector B
	* \return A - B
*/
static inline HOST_DEVICE ColorXYZAf operator - (const ColorXYZAf& A, const ColorXYZAf& B)
{
	return ColorXYZAf(A[0] - B[0], A[1] - B[1], A[2] - B[2], A[3] - B[3]);
};

/*! Add two ColorXYZAf vectors
	* \param A Vector A
	* \param B Vector B
	* \return A + B
*/
static inline HOST_DEVICE ColorXYZAf operator + (const ColorXYZAf& A, const ColorXYZAf& B)
{
	return ColorXYZAf(A[0] + B[0], A[1] + B[1], A[2] + B[2], A[3] + B[3]);
};

/*! Linearly interpolate two ColorXYZAf vectors
	* \param LerpC Interpolation coefficient
	* \param A Vector A
	* \param B Vector B
	* \return Interpolated vector
*/
HOST_DEVICE inline ColorXYZAf Lerp(const float& LerpC, const ColorXYZAf& A, const ColorXYZAf& B)
{
	return (1.0f - LerpC) * A + LerpC * B;
}

}
