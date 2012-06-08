/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "color.h"

namespace ExposureRender
{

struct KelvinRgb
{
	KelvinRgb(const float& Temperature, const float& R, const float& G, const float& B)
	{
		this->Temperature	= Temperature;
		this->RGB[0]		= R;
		this->RGB[1]		= G;
		this->RGB[2]		= B;
	}

	float	Temperature;
	float	RGB[3];
}

static gKelvinRgb[] =
{
	KelvinRgb(1000, 255, 51, 0),
	KelvinRgb(1100, 255, 69, 0),
	KelvinRgb(1200, 255, 82, 0),
	KelvinRgb(1300, 255, 93, 0),
	KelvinRgb(1400, 255, 102, 0),
	KelvinRgb(1500, 255, 111, 0),
	KelvinRgb(1600, 255, 118, 0),
	KelvinRgb(1700, 255, 124, 0),
	KelvinRgb(1800, 255, 130, 0),
	KelvinRgb(1900, 255, 135, 0),
	KelvinRgb(2000, 255, 141, 11),
	KelvinRgb(2100, 255, 146, 29),
	KelvinRgb(2200, 255, 152, 41),
	KelvinRgb(2300, 255, 157, 51),
	KelvinRgb(2400, 255, 162, 60),
	KelvinRgb(2500, 255, 166, 69),
	KelvinRgb(2600, 255, 170, 77),
	KelvinRgb(2700, 255, 174, 84),
	KelvinRgb(2800, 255, 178, 91),
	KelvinRgb(2900, 255, 182, 98),
	KelvinRgb(3000, 255, 185, 105),
	KelvinRgb(3100, 255, 189, 111),
	KelvinRgb(3200, 255, 192, 118),
	KelvinRgb(3300, 255, 195, 124),
	KelvinRgb(3400, 255, 198, 130),
	KelvinRgb(3500, 255, 201, 135),
	KelvinRgb(3600, 255, 203, 141),
	KelvinRgb(3700, 255, 206, 146),
	KelvinRgb(3800, 255, 208, 151),
	KelvinRgb(3900, 255, 211, 156),
	KelvinRgb(4000, 255, 213, 161),
	KelvinRgb(4100, 255, 215, 166),
	KelvinRgb(4200, 255, 217, 171),
	KelvinRgb(4300, 255, 219, 175),
	KelvinRgb(4400, 255, 221, 180),
	KelvinRgb(4500, 255, 223, 184),
	KelvinRgb(4600, 255, 225, 188),
	KelvinRgb(4700, 255, 226, 192),
	KelvinRgb(4800, 255, 228, 196),
	KelvinRgb(4900, 255, 229, 200),
	KelvinRgb(5000, 255, 231, 204),
	KelvinRgb(5100, 255, 232, 208),
	KelvinRgb(5200, 255, 234, 211),
	KelvinRgb(5300, 255, 235, 215),
	KelvinRgb(5400, 255, 237, 218),
	KelvinRgb(5500, 255, 238, 222),
	KelvinRgb(5600, 255, 239, 225),
	KelvinRgb(5700, 255, 240, 228),
	KelvinRgb(5800, 255, 241, 231),
	KelvinRgb(5900, 255, 243, 234),
	KelvinRgb(6000, 255, 244, 237),
	KelvinRgb(6100, 255, 245, 240),
	KelvinRgb(6200, 255, 246, 243),
	KelvinRgb(6300, 255, 247, 245),
	KelvinRgb(6400, 255, 248, 248),
	KelvinRgb(6500, 255, 249, 251),
	KelvinRgb(6600, 255, 249, 253),
	KelvinRgb(6700, 254, 250, 255),
	KelvinRgb(6800, 252, 248, 255),
	KelvinRgb(6900, 250, 247, 255),
	KelvinRgb(7000, 247, 245, 255),
	KelvinRgb(7100, 245, 244, 255),
	KelvinRgb(7200, 243, 243, 255),
	KelvinRgb(7300, 241, 241, 255),
	KelvinRgb(7400, 239, 240, 255),
	KelvinRgb(7500, 238, 239, 255),
	KelvinRgb(7600, 236, 238, 255),
	KelvinRgb(7700, 234, 237, 255),
	KelvinRgb(7800, 233, 236, 255),
	KelvinRgb(7900, 231, 234, 255),
	KelvinRgb(8000, 229, 233, 255),
	KelvinRgb(8100, 228, 233, 255),
	KelvinRgb(8200, 227, 232, 255),
	KelvinRgb(8300, 225, 231, 255),
	KelvinRgb(8400, 224, 230, 255),
	KelvinRgb(8500, 223, 229, 255),
	KelvinRgb(8600, 221, 228, 255),
	KelvinRgb(8700, 220, 227, 255),
	KelvinRgb(8800, 219, 226, 255),
	KelvinRgb(8900, 218, 226, 255),
	KelvinRgb(9000, 217, 225, 255),
	KelvinRgb(9100, 216, 224, 255),
	KelvinRgb(9200, 215, 223, 255),
	KelvinRgb(9300, 214, 223, 255),
	KelvinRgb(9400, 213, 222, 255),
	KelvinRgb(9500, 212, 221, 255),
	KelvinRgb(9600, 211, 221, 255),
	KelvinRgb(9700, 210, 220, 255),
	KelvinRgb(9800, 209, 220, 255),
	KelvinRgb(9900, 208, 219, 255),
	KelvinRgb(10000, 207, 218, 255),
	KelvinRgb(10100, 207, 218, 255),
	KelvinRgb(10200, 206, 217, 255),
	KelvinRgb(10300, 205, 217, 255),
	KelvinRgb(10400, 204, 216, 255),
	KelvinRgb(10500, 204, 216, 255),
	KelvinRgb(10600, 203, 215, 255),
	KelvinRgb(10700, 202, 215, 255),
	KelvinRgb(10800, 202, 214, 255),
	KelvinRgb(10900, 201, 214, 255),
	KelvinRgb(11000, 200, 213, 255),
	KelvinRgb(11100, 200, 213, 255),
	KelvinRgb(11200, 199, 212, 255),
	KelvinRgb(11300, 198, 212, 255),
	KelvinRgb(11400, 198, 212, 255),
	KelvinRgb(11500, 197, 211, 255),
	KelvinRgb(11600, 197, 211, 255),
	KelvinRgb(11700, 196, 210, 255),
	KelvinRgb(11800, 196, 210, 255),
	KelvinRgb(11900, 195, 210, 255),
	KelvinRgb(12000, 195, 209, 255),
	KelvinRgb(12100, 194, 209, 255),
	KelvinRgb(12200, 194, 208, 255),
	KelvinRgb(12300, 193, 208, 255),
	KelvinRgb(12400, 193, 208, 255),
	KelvinRgb(12500, 192, 207, 255),
	KelvinRgb(12600, 192, 207, 255),
	KelvinRgb(12700, 191, 207, 255),
	KelvinRgb(12800, 191, 206, 255),
	KelvinRgb(12900, 190, 206, 255),
	KelvinRgb(13000, 190, 206, 255),
	KelvinRgb(13100, 190, 206, 255),
	KelvinRgb(13200, 189, 205, 255),
	KelvinRgb(13300, 189, 205, 255),
	KelvinRgb(13400, 188, 206, 255),
	KelvinRgb(13500, 188, 204, 255),
	KelvinRgb(13600, 188, 204, 255),
	KelvinRgb(13700, 187, 204, 255),
	KelvinRgb(13800, 187, 204, 255),
	KelvinRgb(13900, 187, 203, 255),
	KelvinRgb(14000, 186, 203, 255),
	KelvinRgb(14100, 186, 203, 255),
	KelvinRgb(14200, 186, 203, 255),
	KelvinRgb(14300, 185, 202, 255),
	KelvinRgb(14400, 185, 202, 255),
	KelvinRgb(14500, 185, 202, 255),
	KelvinRgb(14600, 184, 202, 255),
	KelvinRgb(14700, 184, 201, 255),
	KelvinRgb(14800, 184, 201, 255),
	KelvinRgb(14900, 184, 201, 255),
	KelvinRgb(15000, 183, 201, 255)
};

inline ColorRGBf KelvinToColorRGBf(const float& Kelvin)
{
	const float CorrectedKelvin = Kelvin - 1000.0f;
	
	const KelvinRgb S1 = gKelvinRgb[(int)floorf(CorrectedKelvin / 100.0f)];
	const KelvinRgb S2 = gKelvinRgb[(int)ceilf(CorrectedKelvin / 100.0f)];

	const float L1 = 1.0f - ((Kelvin - S1.Temperature) / 100.0f);
	const float L2 = 1.0f - L1;

	return ColorRGBf(L1 * S1.RGB[0] + L2 * S2.RGB[0], L1 * S1.RGB[1] + L2 * S2.RGB[1], L1 * S1.RGB[2] + L2 * S2.RGB[2]);
}

inline ColorXYZf KelvinToColorXYZf(const float& Kelvin)
{
	return ColorXYZf::FromRGBf(KelvinToColorRGBf(Kelvin).D);
}

}
