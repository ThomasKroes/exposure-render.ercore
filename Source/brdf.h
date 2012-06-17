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

#include "lambert.h"
#include "microfacet.h"
#include "fresnelblend.h"
#include "sample.h"

namespace ExposureRender
{

class Brdf
{
public:
	HOST_DEVICE Brdf(void)
	{
	}

	HOST_DEVICE Brdf(const Vec3f& N, const Vec3f& Wo, const ColorXYZf& Kd, const ColorXYZf& Ks, const float& Ior, const float& Exponent) :
		Nn(Normalize(N)),
		Nu(Normalize(Cross(N, Wo))),
		Nv(Normalize(Cross(N, Nu))),
		Lambert(Kd),
		Microfacet(Ks, Ior, Exponent)
	{
	}

	HOST_DEVICE Vec3f WorldToLocal(const Vec3f& W)
	{
		return Normalize(Vec3f(Dot(W, this->Nu), Dot(W, this->Nv), Dot(W, this->Nn)));
	}

	HOST_DEVICE Vec3f LocalToWorld(const Vec3f& W)
	{
		return Normalize(Vec3f(	this->Nu[0] * W[0] + this->Nv[0] * W[1] + this->Nn[0] * W[2],
								this->Nu[1] * W[0] + this->Nv[1] * W[1] + this->Nn[1] * W[2],
								this->Nu[2] * W[0] + this->Nv[2] * W[1] + this->Nn[2] * W[2]));
	}

	HOST_DEVICE ColorXYZf F(const Vec3f& Wo, const Vec3f& Wi)
	{
		const Vec3f Wol = WorldToLocal(Wo);
		const Vec3f Wil = WorldToLocal(Wi);

		return this->Lambert.F(Wol, Wil) + this->Microfacet.F(Wol, Wil);
	}

	HOST_DEVICE ColorXYZf SampleF(const Vec3f& Wo, Vec3f& Wi, float& Pdf, RNG& RNG)
	{
		const Vec3f Wol = WorldToLocal(Wo);
		Vec3f Wil;

		ColorXYZf R;

		if (RNG.Get1() <= 0.5f)
		{
			this->Lambert.SampleF(Wol, Wil, Pdf, RNG.Get2());
		}
		else
		{
			this->Microfacet.SampleF(Wol, Wil, Pdf, RNG.Get2());
		}

		Pdf += this->Lambert.Pdf(Wol, Wil);
		Pdf += this->Microfacet.Pdf(Wol, Wil);

		Wi = LocalToWorld(Wil);

		return this->Lambert.F(Wol, Wil) + this->Microfacet.F(Wol, Wil);
	}

	HOST_DEVICE float Pdf(const Vec3f& Wo, const Vec3f& Wi)
	{
		const Vec3f Wol = WorldToLocal(Wo);
		const Vec3f Wil = WorldToLocal(Wi);

		float Pdf = 0.0f;

		Pdf += this->Lambert.Pdf(Wol, Wil);
		Pdf += this->Microfacet.Pdf(Wol, Wil);
		
		return Pdf;
	}

	HOST_DEVICE Brdf& operator = (const Brdf& Other)
	{
		this->Nn 			= Other.Nn;
		this->Nu 			= Other.Nu;
		this->Nv 			= Other.Nv;
		this->Lambert 		= Other.Lambert;
		this->Microfacet 	= Other.Microfacet;

		return *this;
	}

	Vec3f			Nn;
	Vec3f			Nu;
	Vec3f			Nv;
	Lambert			Lambert;
	Microfacet		Microfacet;
};

}
