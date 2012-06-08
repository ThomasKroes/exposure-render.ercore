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

class EXPOSURE_RENDER_DLL ColorRGBf
{
public:
	CONSTRUCTORS(ColorRGBf, float, 3)
	VEC3_CONSTRUCTOR(ColorRGBf, float)
	ALL_OPERATORS(ColorRGBf, float, 3)
	MIN_MAX(ColorRGBf, float, 3)
	CLAMP(ColorRGBf, float, 3)

	static inline HOST_DEVICE ColorRGBf Black()
	{
		return ColorRGBf(0.0f);
	}

	static HOST_DEVICE ColorRGBf FromXYZf(const float XYZ[3])
	{
		ColorRGBf Result;

		Result[0] =  3.240479f * XYZ[0] - 1.537150f * XYZ[1] - 0.498535f * XYZ[2];
		Result[1] = -0.969256f * XYZ[0] + 1.875991f * XYZ[1] + 0.041556f * XYZ[2];
		Result[2] =  0.055648f * XYZ[0] - 0.204043f * XYZ[1] + 1.057311f * XYZ[2];

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

	DATA(float, 3)
};


static inline HOST_DEVICE ColorRGBf operator * (const ColorRGBf& RGB, const float& F)
{
	return ColorRGBf(RGB[0] * F, RGB[1] * F, RGB[2] * F);
};

static inline HOST_DEVICE ColorRGBf operator * (const float& F, const ColorRGBf& RGB)
{
	return ColorRGBf(RGB[0] * F, RGB[1] * F, RGB[2] * F);
};

static inline HOST_DEVICE ColorRGBf Lerp(const float& LerpC, const ColorRGBf& A, const ColorRGBf& B)
{
	return LerpC * (B - A);
};

}
