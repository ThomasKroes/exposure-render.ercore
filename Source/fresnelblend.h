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

#include "montecarlo.h"

namespace ExposureRender
{

class FresnelBlend
{
public:
	HOST_DEVICE FresnelBlend(const ColorXYZf& Rd = ColorXYZf(0.0f), const ColorXYZf& Rs = ColorXYZf(0.0f)) :
		Rd(Rd),
		Rs(Rs)
	{
	}

	HOST_DEVICE FresnelBlend& operator = (const FresnelBlend& Other)
	{
		this->Rd 			= Other.Rd;
		this->Rs 			= Other.Rs;

		return *this;
	}

	HOST_DEVICE ColorXYZf F(const Vec3f& Wo, const Vec3f& Wi) const
	{
		ColorXYZf diffuse = (28.f/(23.f * PI_F)) * Rd * (ColorXYZf(1.0f) - Rs) * (1 - powf(1 - .5f * fabsf(CosTheta(Wi)), 5)) * (1 - powf(1 - .5f * fabsf(CosTheta(Wo)), 5));
		
		Vec3f H = Normalize(Wi + Wo);

		ColorXYZf specular = Blinn.D(H) / (8.f * PI_F * AbsDot(Wi, H) * max(fabsf(CosTheta(Wi)), fabsf(CosTheta(Wo)))) * SchlickFresnel(Dot(Wi, H));

		return diffuse + specular;
	}

	HOST_DEVICE ColorXYZf SampleF(const Vec3f& Wo, Vec3f& Wi, float& Pdf, const Vec2f& U)
	{
		
	}

	HOST_DEVICE float Pdf(const Vec3f& Wo, const Vec3f& Wi) const
	{
	}

	HOST_DEVICE ColorXYZf SchlickFresnel(float costheta) const
	{
		return Rs + powf(1 - costheta, 5.f) * (ColorXYZf(1.0f) - Rs);
	}

	ColorXYZf 	Rd;
	ColorXYZf 	Rs;
	Blinn		Blinn;
};


}
