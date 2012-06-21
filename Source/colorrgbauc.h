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

class EXPOSURE_RENDER_DLL ColorRGBAuc
{
public:
	CONSTRUCTORS(ColorRGBAuc, unsigned char, 4)
	VEC4_CONSTRUCTOR(ColorRGBAuc, unsigned char)
	ALL_OPERATORS(ColorRGBAuc, unsigned char, 4)

	static inline HOST_DEVICE ColorRGBAuc Black()
	{
		return ColorRGBAuc();
	}

	HOST_DEVICE ColorRGBAuc FromXYZf(const float XYZ[3])
	{
		ColorRGBAuc Result;

		const float RGB[3] = 
		{
			3.240479f * XYZ[0] - 1.537150f * XYZ[1] - 0.498535f * XYZ[2],
			-0.969256f * XYZ[0] + 1.875991f * XYZ[1] + 0.041556f * XYZ[2],
			0.055648f * XYZ[0] - 0.204043f * XYZ[1] + 1.057311f * XYZ[2]
		};

		Result[0] = (unsigned char)ExposureRender::Clamp((int)(RGB[0] * 255.0f), 0, 255);
		Result[1] = (unsigned char)ExposureRender::Clamp((int)(RGB[1] * 255.0f), 0, 255);
		Result[2] = (unsigned char)ExposureRender::Clamp((int)(RGB[2] * 255.0f), 0, 255);

		return Result;
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

	HOST_DEVICE void BlendWithForeGround(const ColorRGBAuc& Foreground)
	{
		// http://forums.codeguru.com/showthread.php?497515-Math-to-merge-two-RGBA-bitmaps
		const float Alpha = this->D[3] * Foreground[3];

		this->D[0] = this->D[0] * Alpha + Foreground[0] * Foreground[3] + Alpha * Foreground[3] * Foreground[0];
		this->D[1] = this->D[1] * Alpha + Foreground[1] * Foreground[3] + Alpha * Foreground[3] * Foreground[1];
		this->D[2] = this->D[2] * Alpha + Foreground[2] * Foreground[3] + Alpha * Foreground[3] * Foreground[2];
		this->D[3] = Foreground[3] + Alpha - Foreground[3] * Alpha;
	}

	DATA(unsigned char, 4)
};

static inline HOST_DEVICE ColorRGBAuc operator * (const ColorRGBAuc& RGBA, const float& F)
{
	return ColorRGBAuc(	(unsigned char)((float)RGBA[0] * F),
						(unsigned char)((float)RGBA[1] * F),
						(unsigned char)((float)RGBA[2] * F),
						(unsigned char)((float)RGBA[3] * F));
};

static inline HOST_DEVICE ColorRGBAuc operator * (const float& F, const ColorRGBAuc& RGBA)
{
	return ColorRGBAuc(	(unsigned char)((float)RGBA[0] * F),
						(unsigned char)((float)RGBA[1] * F),
						(unsigned char)((float)RGBA[2] * F),
						(unsigned char)((float)RGBA[3] * F));
};

}
