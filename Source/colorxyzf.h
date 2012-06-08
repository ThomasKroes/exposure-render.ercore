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

class EXPOSURE_RENDER_DLL ColorXYZf
{
public:
	CONSTRUCTORS(ColorXYZf, float, 3)
	VEC3_CONSTRUCTOR(ColorXYZf, float)
	ALL_OPERATORS(ColorXYZf, float, 3)
	MIN_MAX(ColorXYZf, float, 3)
	CLAMP(ColorXYZf, float, 3)

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

	HOST_DEVICE bool IsBlack()
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

	DATA(float, 3)
};

static inline HOST_DEVICE ColorXYZf operator * (const ColorXYZf& XYZ, const float& F)
{
	return ColorXYZf(XYZ[0] * F, XYZ[1] * F, XYZ[2] * F);
};

static inline HOST_DEVICE ColorXYZf operator * (const float& F, const ColorXYZf& XYZ)
{
	return ColorXYZf(XYZ[0] * F, XYZ[1] * F, XYZ[2] * F);
};

static inline HOST_DEVICE ColorXYZf Lerp(const float& LerpC, const ColorXYZf& A, const ColorXYZf& B)
{
	return LerpC * (B - A);
};

}
