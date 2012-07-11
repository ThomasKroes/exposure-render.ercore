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

#include "vector.h"

namespace ExposureRender
{

/*! \class ColorXYZf
 * \brief XYZ float class
 */
class EXPOSURE_RENDER_DLL ColorXYZf : public Vec<float, 3>
{
public:
	/*! Default constructor */
	HOST_DEVICE ColorXYZf()
	{
		for (int i = 0; i < 3; ++i)
			this->D[i] = 0.0f;
	}

	/*! Construct and initialize with default value
		* \param Default The default value
	*/
	HOST_DEVICE ColorXYZf(const float& V)
	{
		for (int i = 0; i < 3; ++i)
			this->D[i] = V;
	}

	/*! Constructor with initializing values */
	HOST_DEVICE ColorXYZf(const float& X, const float& Y, const float& Z)
	{
		this->D[0] = X;
		this->D[1] = Y;
		this->D[2] = Z;
	}

	/*! Copy constructor */
	HOST_DEVICE ColorXYZf(const Vec<float, 3>& Other)
	{
		for (int i = 0; i < 3; ++i)
			this->D[i] = Other[i];
	}

	static HOST_DEVICE ColorXYZf Black()
	{
		return ColorXYZf(0.0f);
	}

	static HOST_DEVICE ColorXYZf FromRGBf(const float RGB[3])
	{
		ColorXYZf Result;

		Result[0] = 0.412453f * RGB[0] + 0.357580f * RGB[1] + 0.180423f * RGB[2];
		Result[1] = 0.212671f * RGB[0] + 0.715160f * RGB[1] + 0.072169f * RGB[2];
		Result[2] = 0.019334f * RGB[0] + 0.119193f * RGB[1] + 0.950227f * RGB[2];

		return Result;
	}

	static HOST_DEVICE ColorXYZf FromRGBuc(const unsigned char RGB[3])
	{
		ColorXYZf Result;

		const float RGBf[3] = 
		{
			ONE_OVER_255 * (float)RGB[0],
			ONE_OVER_255 * (float)RGB[1],
			ONE_OVER_255 * (float)RGB[2]
		};

		Result[0] = 0.412453f * RGBf[0] + 0.357580f * RGBf[1] + 0.180423f * RGBf[2];
		Result[1] = 0.212671f * RGBf[0] + 0.715160f * RGBf[1] + 0.072169f * RGBf[2];
		Result[2] = 0.019334f * RGBf[0] + 0.119193f * RGBf[1] + 0.950227f * RGBf[2];

		return Result;
	}

	static HOST_DEVICE ColorXYZf FromRGBAuc(const unsigned char RGBA[4])
	{
		ColorXYZf Result;

		const float RGBAf[3] = 
		{
			ONE_OVER_255 * (float)RGBA[0],
			ONE_OVER_255 * (float)RGBA[1],
			ONE_OVER_255 * (float)RGBA[2]
		};

		Result[0] = 0.412453f * RGBAf[0] + 0.357580f * RGBAf[1] + 0.180423f * RGBAf[2];
		Result[1] = 0.212671f * RGBAf[0] + 0.715160f * RGBAf[1] + 0.072169f * RGBAf[2];
		Result[2] = 0.019334f * RGBAf[0] + 0.119193f * RGBAf[1] + 0.950227f * RGBAf[2];

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

/*! Multiply ColorXYZf with float
	* \param C Color
	* \param F Float to multiply with
	* \return C x F
*/
static inline HOST_DEVICE ColorXYZf operator * (const ColorXYZf& C, const float& F)
{
	return ColorXYZf(C[0] * F, C[1] * F, C[2] * F);
};

/*! Multiply float with ColorXYZf
	* \param C ColorXYZf
	* \param F Float to multiply with
	* \return F x C
*/
static inline HOST_DEVICE ColorXYZf operator * (const float& F, const ColorXYZf& C)
{
	return ColorXYZf(C[0] * F, C[1] * F, C[2] * F);
};

/*! Multiply two ColorXYZf vectors
	* \param A Vector A
	* \param B Vector B
	* \return A x B
*/
static inline HOST_DEVICE ColorXYZf operator * (const ColorXYZf& A, const ColorXYZf& B)
{
	return ColorXYZf(A[0] * B[0], A[1] * B[1], A[2] * B[2]);
};

/*! Divide ColorXYZf vector by float value
	* \param C ColorXYZf to divide
	* \param F Float to divide with
	* \return F / V
*/
static inline HOST_DEVICE ColorXYZf operator / (const ColorXYZf& C, const float& F)
{
	// Compute F reciprocal, slightly faster
	const float InvF = (F == 0.0f) ? 0.0f : 1.0f / F;

	return ColorXYZf((float)C[0] * InvF, (float)C[1] * InvF, (float)C[2] * InvF);
};

/*! Subtract two ColorXYZf vectors
	* \param A Vector A
	* \param B Vector B
	* \return A - B
*/
static inline HOST_DEVICE ColorXYZf operator - (const ColorXYZf& A, const ColorXYZf& B)
{
	return ColorXYZf(A[0] - B[0], A[1] - B[1], A[2] - B[2]);
};

/*! Add two ColorXYZf vectors
	* \param A Vector A
	* \param B Vector B
	* \return A + B
*/
static inline HOST_DEVICE ColorXYZf operator + (const ColorXYZf& A, const ColorXYZf& B)
{
	return ColorXYZf(A[0] + B[0], A[1] + B[1], A[2] + B[2]);
};

/*! Linearly interpolate two ColorXYZf vectors
	* \param LerpC Interpolation coefficient
	* \param A Vector A
	* \param B Vector B
	* \return Interpolated vector
*/
HOST_DEVICE inline ColorXYZf Lerp(const float& LerpC, const ColorXYZf& A, const ColorXYZf& B)
{
	return (1.0f - LerpC) * A + LerpC * B;
}

}
