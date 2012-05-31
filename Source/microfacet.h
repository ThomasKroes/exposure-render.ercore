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

#include "blinn.h"
#include "fresnel.h"

namespace ExposureRender
{

class Microfacet
{
public:
	HOST_DEVICE Microfacet(void)
	{
	}

	HOST_DEVICE Microfacet(const ColorXYZf& Reflectance, const float& Ior, const float& Exponent) :
		R(Reflectance),
		Fresnel(1.0f, Ior),
		Blinn(Exponent)
	{
	}

	HOST_DEVICE ColorXYZf F(const Vec3f& Wo, const Vec3f& Wi)
	{
		const float cosThetaO = AbsCosTheta(Wo);
		const float cosThetaI = AbsCosTheta(Wi);

		if (cosThetaI == 0.0f || cosThetaO == 0.0f)
			return ColorXYZf(0.0f);

		Vec3f Wh = Wi + Wo;

		if (Wh[0] == 0.0f && Wh[1] == 0.0f && Wh[2] == 0.0f)
			return ColorXYZf(0.0f);

		Wh = Normalize(Wh);
		
		float cosThetaH = Dot(Wi, Wh);

		ColorXYZf F = this->Fresnel.Evaluate(cosThetaH);

		return this->R * this->Blinn.D(Wh) * G(Wo, Wi, Wh) * F / (4.0f * cosThetaI * cosThetaO);
	}

	HOST_DEVICE ColorXYZf SampleF(const Vec3f& Wo, Vec3f& Wi, float& Pdf, const Vec2f& U)
	{
		this->Blinn.SampleF(Wo, Wi, Pdf, U);

		if (!SameHemisphere(Wo, Wi))
			return ColorXYZf::Black();

		return this->F(Wo, Wi);
	}

	HOST_DEVICE float Pdf(const Vec3f& Wo, const Vec3f& Wi)
	{
		if (!SameHemisphere(Wo, Wi))
			return 0.0f;

		return Blinn.Pdf(Wo, Wi);
	}

	HOST_DEVICE float G(const Vec3f& Wo, const Vec3f& Wi, const Vec3f& Wh)
	{
		const float NdotWh 	= AbsCosTheta(Wh);
		const float NdotWo 	= AbsCosTheta(Wo);
		const float NdotWi 	= AbsCosTheta(Wi);
		const float WOdotWh = AbsDot(Wo, Wh);

		return min(1.0f, min((2.0f * NdotWh * NdotWo / WOdotWh), (2.0f * NdotWh * NdotWi / WOdotWh)));
	}

	HOST_DEVICE Microfacet& operator = (const Microfacet& Other)
	{
		this->R			= Other.R;
		this->Fresnel	= Other.Fresnel;
		this->Blinn		= Other.Blinn;

		return *this;
	}

	ColorXYZf	R;
	Fresnel		Fresnel;
	Blinn		Blinn;

};

}