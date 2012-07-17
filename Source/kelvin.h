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

#include "color.h"

namespace ExposureRender
{

/*! Bitmap class */
class KelvinRgb
{
public:
	/*! Constructor */
	KelvinRgb(const float& Temperature, const ColorRGBf& RGB) :
		Temperature(Temperature),
		RGB(RGB)
	{
	}

	float		Temperature;
	ColorRGBf	RGB;
}

/*! Precomputed table of kelvin temperatures and corresponding color */
static gKelvinRgb[] =
{
	KelvinRgb(1000, ColorRGBf(255, 51, 0)),
	KelvinRgb(1100, ColorRGBf(255, 69, 0)),
	KelvinRgb(1200, ColorRGBf(255, 82, 0)),
	KelvinRgb(1300, ColorRGBf(255, 93, 0)),
	KelvinRgb(1400, ColorRGBf(255, 102, 0)),
	KelvinRgb(1500, ColorRGBf(255, 111, 0)),
	KelvinRgb(1600, ColorRGBf(255, 118, 0)),
	KelvinRgb(1700, ColorRGBf(255, 124, 0)),
	KelvinRgb(1800, ColorRGBf(255, 130, 0)),
	KelvinRgb(1900, ColorRGBf(255, 135, 0)),
	KelvinRgb(2000, ColorRGBf(255, 141, 11)),
	KelvinRgb(2100, ColorRGBf(255, 146, 29)),
	KelvinRgb(2200, ColorRGBf(255, 152, 41)),
	KelvinRgb(2300, ColorRGBf(255, 157, 51)),
	KelvinRgb(2400, ColorRGBf(255, 162, 60)),
	KelvinRgb(2500, ColorRGBf(255, 166, 69)),
	KelvinRgb(2600, ColorRGBf(255, 170, 77)),
	KelvinRgb(2700, ColorRGBf(255, 174, 84)),
	KelvinRgb(2800, ColorRGBf(255, 178, 91)),
	KelvinRgb(2900, ColorRGBf(255, 182, 98)),
	KelvinRgb(3000, ColorRGBf(255, 185, 105)),
	KelvinRgb(3100, ColorRGBf(255, 189, 111)),
	KelvinRgb(3200, ColorRGBf(255, 192, 118)),
	KelvinRgb(3300, ColorRGBf(255, 195, 124)),
	KelvinRgb(3400, ColorRGBf(255, 198, 130)),
	KelvinRgb(3500, ColorRGBf(255, 201, 135)),
	KelvinRgb(3600, ColorRGBf(255, 203, 141)),
	KelvinRgb(3700, ColorRGBf(255, 206, 146)),
	KelvinRgb(3800, ColorRGBf(255, 208, 151)),
	KelvinRgb(3900, ColorRGBf(255, 211, 156)),
	KelvinRgb(4000, ColorRGBf(255, 213, 161)),
	KelvinRgb(4100, ColorRGBf(255, 215, 166)),
	KelvinRgb(4200, ColorRGBf(255, 217, 171)),
	KelvinRgb(4300, ColorRGBf(255, 219, 175)),
	KelvinRgb(4400, ColorRGBf(255, 221, 180)),
	KelvinRgb(4500, ColorRGBf(255, 223, 184)),
	KelvinRgb(4600, ColorRGBf(255, 225, 188)),
	KelvinRgb(4700, ColorRGBf(255, 226, 192)),
	KelvinRgb(4800, ColorRGBf(255, 228, 196)),
	KelvinRgb(4900, ColorRGBf(255, 229, 200)),
	KelvinRgb(5000, ColorRGBf(255, 231, 204)),
	KelvinRgb(5100, ColorRGBf(255, 232, 208)),
	KelvinRgb(5200, ColorRGBf(255, 234, 211)),
	KelvinRgb(5300, ColorRGBf(255, 235, 215)),
	KelvinRgb(5400, ColorRGBf(255, 237, 218)),
	KelvinRgb(5500, ColorRGBf(255, 238, 222)),
	KelvinRgb(5600, ColorRGBf(255, 239, 225)),
	KelvinRgb(5700, ColorRGBf(255, 240, 228)),
	KelvinRgb(5800, ColorRGBf(255, 241, 231)),
	KelvinRgb(5900, ColorRGBf(255, 243, 234)),
	KelvinRgb(6000, ColorRGBf(255, 244, 237)),
	KelvinRgb(6100, ColorRGBf(255, 245, 240)),
	KelvinRgb(6200, ColorRGBf(255, 246, 243)),
	KelvinRgb(6300, ColorRGBf(255, 247, 245)),
	KelvinRgb(6400, ColorRGBf(255, 248, 248)),
	KelvinRgb(6500, ColorRGBf(255, 249, 251)),
	KelvinRgb(6600, ColorRGBf(255, 249, 253)),
	KelvinRgb(6700, ColorRGBf(254, 250, 255)),
	KelvinRgb(6800, ColorRGBf(252, 248, 255)),
	KelvinRgb(6900, ColorRGBf(250, 247, 255)),
	KelvinRgb(7000, ColorRGBf(247, 245, 255)),
	KelvinRgb(7100, ColorRGBf(245, 244, 255)),
	KelvinRgb(7200, ColorRGBf(243, 243, 255)),
	KelvinRgb(7300, ColorRGBf(241, 241, 255)),
	KelvinRgb(7400, ColorRGBf(239, 240, 255)),
	KelvinRgb(7500, ColorRGBf(238, 239, 255)),
	KelvinRgb(7600, ColorRGBf(236, 238, 255)),
	KelvinRgb(7700, ColorRGBf(234, 237, 255)),
	KelvinRgb(7800, ColorRGBf(233, 236, 255)),
	KelvinRgb(7900, ColorRGBf(231, 234, 255)),
	KelvinRgb(8000, ColorRGBf(229, 233, 255)),
	KelvinRgb(8100, ColorRGBf(228, 233, 255)),
	KelvinRgb(8200, ColorRGBf(227, 232, 255)),
	KelvinRgb(8300, ColorRGBf(225, 231, 255)),
	KelvinRgb(8400, ColorRGBf(224, 230, 255)),
	KelvinRgb(8500, ColorRGBf(223, 229, 255)),
	KelvinRgb(8600, ColorRGBf(221, 228, 255)),
	KelvinRgb(8700, ColorRGBf(220, 227, 255)),
	KelvinRgb(8800, ColorRGBf(219, 226, 255)),
	KelvinRgb(8900, ColorRGBf(218, 226, 255)),
	KelvinRgb(9000, ColorRGBf(217, 225, 255)),
	KelvinRgb(9100, ColorRGBf(216, 224, 255)),
	KelvinRgb(9200, ColorRGBf(215, 223, 255)),
	KelvinRgb(9300, ColorRGBf(214, 223, 255)),
	KelvinRgb(9400, ColorRGBf(213, 222, 255)),
	KelvinRgb(9500, ColorRGBf(212, 221, 255)),
	KelvinRgb(9600, ColorRGBf(211, 221, 255)),
	KelvinRgb(9700, ColorRGBf(210, 220, 255)),
	KelvinRgb(9800, ColorRGBf(209, 220, 255)),
	KelvinRgb(9900, ColorRGBf(208, 219, 255)),
	KelvinRgb(10000, ColorRGBf(207, 218, 255)),
	KelvinRgb(10100, ColorRGBf(207, 218, 255)),
	KelvinRgb(10200, ColorRGBf(206, 217, 255)),
	KelvinRgb(10300, ColorRGBf(205, 217, 255)),
	KelvinRgb(10400, ColorRGBf(204, 216, 255)),
	KelvinRgb(10500, ColorRGBf(204, 216, 255)),
	KelvinRgb(10600, ColorRGBf(203, 215, 255)),
	KelvinRgb(10700, ColorRGBf(202, 215, 255)),
	KelvinRgb(10800, ColorRGBf(202, 214, 255)),
	KelvinRgb(10900, ColorRGBf(201, 214, 255)),
	KelvinRgb(11000, ColorRGBf(200, 213, 255)),
	KelvinRgb(11100, ColorRGBf(200, 213, 255)),
	KelvinRgb(11200, ColorRGBf(199, 212, 255)),
	KelvinRgb(11300, ColorRGBf(198, 212, 255)),
	KelvinRgb(11400, ColorRGBf(198, 212, 255)),
	KelvinRgb(11500, ColorRGBf(197, 211, 255)),
	KelvinRgb(11600, ColorRGBf(197, 211, 255)),
	KelvinRgb(11700, ColorRGBf(196, 210, 255)),
	KelvinRgb(11800, ColorRGBf(196, 210, 255)),
	KelvinRgb(11900, ColorRGBf(195, 210, 255)),
	KelvinRgb(12000, ColorRGBf(195, 209, 255)),
	KelvinRgb(12100, ColorRGBf(194, 209, 255)),
	KelvinRgb(12200, ColorRGBf(194, 208, 255)),
	KelvinRgb(12300, ColorRGBf(193, 208, 255)),
	KelvinRgb(12400, ColorRGBf(193, 208, 255)),
	KelvinRgb(12500, ColorRGBf(192, 207, 255)),
	KelvinRgb(12600, ColorRGBf(192, 207, 255)),
	KelvinRgb(12700, ColorRGBf(191, 207, 255)),
	KelvinRgb(12800, ColorRGBf(191, 206, 255)),
	KelvinRgb(12900, ColorRGBf(190, 206, 255)),
	KelvinRgb(13000, ColorRGBf(190, 206, 255)),
	KelvinRgb(13100, ColorRGBf(190, 206, 255)),
	KelvinRgb(13200, ColorRGBf(189, 205, 255)),
	KelvinRgb(13300, ColorRGBf(189, 205, 255)),
	KelvinRgb(13400, ColorRGBf(188, 206, 255)),
	KelvinRgb(13500, ColorRGBf(188, 204, 255)),
	KelvinRgb(13600, ColorRGBf(188, 204, 255)),
	KelvinRgb(13700, ColorRGBf(187, 204, 255)),
	KelvinRgb(13800, ColorRGBf(187, 204, 255)),
	KelvinRgb(13900, ColorRGBf(187, 203, 255)),
	KelvinRgb(14000, ColorRGBf(186, 203, 255)),
	KelvinRgb(14100, ColorRGBf(186, 203, 255)),
	KelvinRgb(14200, ColorRGBf(186, 203, 255)),
	KelvinRgb(14300, ColorRGBf(185, 202, 255)),
	KelvinRgb(14400, ColorRGBf(185, 202, 255)),
	KelvinRgb(14500, ColorRGBf(185, 202, 255)),
	KelvinRgb(14600, ColorRGBf(184, 202, 255)),
	KelvinRgb(14700, ColorRGBf(184, 201, 255)),
	KelvinRgb(14800, ColorRGBf(184, 201, 255)),
	KelvinRgb(14900, ColorRGBf(184, 201, 255)),
	KelvinRgb(15000, ColorRGBf(183, 201, 255)
};

/*! Converts a Kelvin temperature into RGB color
	@param Kelvin
	@return RGB color
*/
inline ColorRGBf KelvinToColorRGBf(const float& Kelvin)
{
	const float CorrectedKelvin = Kelvin - 1000.0f;
	
	const KelvinRgb S1 = gKelvinRgb[(int)floorf(CorrectedKelvin / 100.0f)];
	const KelvinRgb S2 = gKelvinRgb[(int)ceilf(CorrectedKelvin / 100.0f)];

	const float L1 = 1.0f - ((Kelvin - S1.Temperature) / 100.0f);
	const float L2 = 1.0f - L1;

	return ColorRGBf(L1 * S1.RGB[0] + L2 * S2.RGB[0], L1 * S1.RGB[1] + L2 * S2.RGB[1], L1 * S1.RGB[2] + L2 * S2.RGB[2]);
}

/*! Converts a Kelvin temperature into CIE XYZ color
	@param Kelvin
	@return XYZ color
*/
inline ColorXYZf KelvinToColorXYZf(const float& Kelvin)
{
	return RGBfToXYZf(KelvinToColorRGBf(Kelvin));
}

}
