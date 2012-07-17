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

#include "colorrgbauc.h"
#include "colorrgbf.h"
#include "colorrgbaf.h"
#include "colorxyzf.h"
#include "colorxyzaf.h"

namespace ExposureRender
{

static inline HOST_DEVICE ColorXYZf RGBfToXYZf(ColorRGBf& RGB)
{
	return ColorXYZf::FromRGBf(RGB.D);
};

#define NO_PIXELS 4

/*! Pixel hysteresis base template class */
class EXPOSURE_RENDER_DLL PixelHysteresis
{
public:
	/*! Default constructor */
	HOST PixelHysteresis() :
		PixelAverage(),
		NoPixels(0)
	{
	}
	
	/*! Assignment operator
		@param[in] Other Pixel hysteresis to copy
		@return Pixel hysteresis
	*/
	HOST PixelHysteresis& operator = (const PixelHysteresis& Other)
	{
		/*
		this->Duration		= Other.Duration;
		this->Durations		= Other.Durations;
		*/

		return *this;
	}

	HOST_DEVICE void AddPixel(const ColorRGBAuc& Pixel)
	{
		/*
		if (this->NoPixels > 1)
		{
			for (int i = this->NoPixels - 1; i > 0; i--)
				Pixels[i] = Pixels[i - 1];
		}

		Pixels[0] = Pixel;
		
		this->NoPixels++;
		
		this->NoPixels = Clamp(this->NoPixels, 0, NO_PIXELS);

		const int Radius = 8;

		
		ColorRGBAuc FilteredPixels[NO_PIXELS];

		// First filter the pixels
		for (int i = 0; i < this->NoPixels; i++)
		{
			int Range[2] =
			{
				max(0, i - Radius),
				min(NO_PIXELS - 1, i + Radius)
			};
			
			float SumColor[4]	= { 0.0f, 0.0f, 0.0f, 0.0f };
			float SumWeight[4]	= { 0.0f, 0.0f, 0.0f, 0.0f };

			for (int j = Range[0]; j < Range[1]; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					SumColor[k]		+= this->Pixels[j][k];
					SumWeight[k]	+= 1.0f;
				}
			}

			for (int k = 0; k < 4; k++)
				FilteredPixels[i][k] = SumColor[k] / SumWeight[k];
		};
		*/

		float SumColor[4]	= { 0.0f, 0.0f, 0.0f, 0.0f };
		float SumWeight[4]	= { 0.0f, 0.0f, 0.0f, 0.0f };

		for (int i = 0; i < this->NoPixels; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				SumColor[k]		+= this->Pixels[i][k];
				SumWeight[k]	+= 1.0f;
			}
		}
		
		for (int i = 0; i < 4; i++)
			this->PixelAverage[i] = (unsigned char)((float)SumColor[i] / (float)SumWeight[i]);
	}

	ColorRGBAuc		Pixels[NO_PIXELS];
	ColorRGBAuc		PixelAverage;
	int				NoPixels;
};

}
