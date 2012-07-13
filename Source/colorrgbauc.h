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

/*! \class ColorRGBAf
 * \brief RGBA unsigned char class
 */
class EXPOSURE_RENDER_DLL ColorRGBAuc : public Vec<unsigned char, 4>
{
public:
	/*! Default constructor */
	HOST_DEVICE ColorRGBAuc()
	{
		for (int i = 0; i < 4; ++i)
			this->D[i] = 0;
	}

	/*! Construct and initialize with default value
		* \param Default The default value
	*/
	HOST_DEVICE ColorRGBAuc(const unsigned char& V)
	{
		for (int i = 0; i < 3; ++i)
			this->D[i] = V;
	}

	/*! Constructor with initializing values */
	HOST_DEVICE ColorRGBAuc(const unsigned char& R, const unsigned char& G, const unsigned char& B, const unsigned char& A)
	{
		this->D[0] = R;
		this->D[1] = G;
		this->D[2] = B;
		this->D[3] = A;
	}

	/*! Copy constructor */
	HOST_DEVICE ColorRGBAuc(const Vec<unsigned char, 4>& Other)
	{
		for (int i = 0; i < 4; ++i)
			this->D[i] = Other[i];
	}

	static HOST_DEVICE ColorRGBAuc Black()
	{
		return ColorRGBAuc();
	}

	static HOST_DEVICE ColorRGBAuc FromXYZf(const float XYZ[3])
	{
		ColorRGBAuc Result;

		// Convert to RGB
		float RGB[3] = 
		{
			 3.240479f * XYZ[0] - 1.537150f * XYZ[1] - 0.498535f * XYZ[2],
			-0.969256f * XYZ[0] + 1.875991f * XYZ[1] + 0.041556f * XYZ[2],
			 0.055648f * XYZ[0] - 0.204043f * XYZ[1] + 1.057311f * XYZ[2]
		};
		
		// Make sure the RBG values are in the (0 - 1) range
		for (int i = 0; i < 3; i++)
			RGB[i] = ExposureRender::Clamp(RGB[i], 0.0f, 1.0f);

		// Convert to unsigned char
		for (int i = 0; i < 4; i++)
			Result[i] = (unsigned char)(RGB[i] * 255.0f);

		return Result;
	}

	static HOST_DEVICE ColorRGBAuc FromXYZAf(const float XYZA[4])
	{
		ColorRGBAuc Result;

		// Convert to RGB
		float RGBA[4] = 
		{
			 3.240479f * XYZA[0] - 1.537150f * XYZA[1] - 0.498535f * XYZA[2],
			-0.969256f * XYZA[0] + 1.875991f * XYZA[1] + 0.041556f * XYZA[2],
			 0.055648f * XYZA[0] - 0.204043f * XYZA[1] + 1.057311f * XYZA[2],
			 XYZA[3]
		};

		// Make sure the RBG values are in the (0 - 1) range
		for (int i = 0; i < 4; i++)
			RGBA[i] = ExposureRender::Clamp(RGBA[i], 0.0f, 1.0f);

		// Convert to unsigned char
		for (int i = 0; i < 4; i++)
			Result[i] = (unsigned char)(RGBA[i] * 255.0f);

		return Result;
	}

	HOST_DEVICE void GammaCorrect(const float& Gamma)
	{
		const float InvGamma = 1.0f / Gamma;

		this->D[0] = (unsigned char)powf((float)this->D[0], InvGamma);
		this->D[1] = (unsigned char)powf((float)this->D[1], InvGamma);
		this->D[2] = (unsigned char)powf((float)this->D[2], InvGamma);
	}

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

	static HOST_DEVICE ColorRGBAuc Blend(const ColorRGBAuc& Background, const ColorRGBAuc& Foreground)
	{
		// http://paulbourke.net/oldstuff/composite/
		
		ColorRGBAuc Blend;

		const float NormBackgroundRGBAf[4] =
		{
			ONE_OVER_255 * (float)Background[0],
			ONE_OVER_255 * (float)Background[1],
			ONE_OVER_255 * (float)Background[2],
			ONE_OVER_255 * (float)Background[3]
		};

		const float NormForegroundRGBAf[4] =
		{
			ONE_OVER_255 * (float)Foreground[0],
			ONE_OVER_255 * (float)Foreground[1],
			ONE_OVER_255 * (float)Foreground[2],
			ONE_OVER_255 * (float)Foreground[3]
		};

		const float NormBlendRGBAf[4] =
		{
			NormBackgroundRGBAf[0] * NormBackgroundRGBAf[3] * (1.0f - NormForegroundRGBAf[3]) + NormForegroundRGBAf[0] * NormForegroundRGBAf[3],
			NormBackgroundRGBAf[1] * NormBackgroundRGBAf[3] * (1.0f - NormForegroundRGBAf[3]) + NormForegroundRGBAf[1] * NormForegroundRGBAf[3],
			NormBackgroundRGBAf[2] * NormBackgroundRGBAf[3] * (1.0f - NormForegroundRGBAf[3]) + NormForegroundRGBAf[2] * NormForegroundRGBAf[3],
			NormBackgroundRGBAf[3] * (1.0f - NormForegroundRGBAf[3]) + NormForegroundRGBAf[3],
		};

		Blend[0] = (unsigned char)(NormBlendRGBAf[0] * 255.0f);
		Blend[1] = (unsigned char)(NormBlendRGBAf[1] * 255.0f);
		Blend[2] = (unsigned char)(NormBlendRGBAf[2] * 255.0f);
		Blend[3] = (unsigned char)(NormBlendRGBAf[3] * 255.0f);

		return Blend;
	}

	HOST_DEVICE void BlendWithForeground(const ColorRGBAuc& Foreground)
	{
		*this = ColorRGBAuc::Blend(*this, Foreground);
	}

	HOST_DEVICE void BlendWithBackground(const ColorRGBAuc& Background)
	{
		*this = ColorRGBAuc::Blend(Background, *this);
	}
};

/*! Multiply ColorRGBAuc with float
	* \param C ColorRGBAuc
	* \param F Float to multiply with
	* \return C x F
*/
static inline HOST_DEVICE ColorRGBAuc operator * (const ColorRGBAuc& C, const float& F)
{
	return ColorRGBAuc(	(unsigned char)((float)C[0] * F),
						(unsigned char)((float)C[1] * F),
						(unsigned char)((float)C[2] * F),
						(unsigned char)((float)C[3] * F));
};

/*! Multiply float with ColorRGBAuc
	* \param C ColorRGBAuc
	* \param F Float to multiply with
	* \return F x C
*/
static inline HOST_DEVICE ColorRGBAuc operator * (const float& F, const ColorRGBAuc& C)
{
	return ColorRGBAuc(	(unsigned char)((float)C[0] * F),
						(unsigned char)((float)C[1] * F),
						(unsigned char)((float)C[2] * F),
						(unsigned char)((float)C[3] * F));
};

/*! Multiply two ColorRGBAuc vectors
	* \param A Vector A
	* \param B Vector B
	* \return A x B
*/
static inline HOST_DEVICE ColorRGBAuc operator * (const ColorRGBAuc& A, const ColorRGBAuc& B)
{
	return ColorRGBAuc(A[0] * B[0], A[1] * B[1], A[2] * B[2], A[3] * B[3]);
};

/*! Divide ColorRGBAuc vector by float value
	* \param C ColorRGBAuc to divide
	* \param F Float to divide with
	* \return F / V
*/
static inline HOST_DEVICE ColorRGBAuc operator / (const ColorRGBAuc& C, const float& F)
{
	// Compute F reciprocal, slightly faster
	const float InvF = (F == 0.0f) ? 0.0f : 1.0f / F;

	return ColorRGBAuc((unsigned char)((float)C[0] * InvF), (unsigned char)((float)C[1] * InvF), (unsigned char)((float)C[2] * InvF), (unsigned char)((float)C[3] * InvF));
};

/*! Subtract two ColorRGBAuc vectors
	* \param A Vector A
	* \param B Vector B
	* \return A - B
*/
static inline HOST_DEVICE ColorRGBAuc operator - (const ColorRGBAuc& A, const ColorRGBAuc& B)
{
	return ColorRGBAuc(A[0] - B[0], A[1] - B[1], A[2] - B[2], A[3] - B[3]);
};

/*! Add two ColorRGBAuc vectors
	* \param A Vector A
	* \param B Vector B
	* \return A + B
*/
static inline HOST_DEVICE ColorRGBAuc operator + (const ColorRGBAuc& A, const ColorRGBAuc& B)
{
	return ColorRGBAuc(A[0] + B[0], A[1] + B[1], A[2] + B[2], A[3] + B[3]);
};

/*! Linearly interpolate two ColorRGBAuc vectors
	* \param LerpC Interpolation coefficient
	* \param A Vector A
	* \param B Vector B
	* \return Interpolated vector
*/
HOST_DEVICE inline ColorRGBAuc Lerp(const float& LerpC, const ColorRGBAuc& A, const ColorRGBAuc& B)
{
	return (1.0f - LerpC) * A + LerpC * B;
}

}
