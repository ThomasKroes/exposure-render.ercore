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

#include "brdf.h"
#include "phasefunction.h"
#include "utilities.h"

namespace ExposureRender
{

class Shader
{
public:
	HOST_DEVICE Shader(void)
	{
	}

	HOST_DEVICE Shader(const Enums::ScatterFunction& Type, const Vec3f& N, const Vec3f& Wo, const ColorXYZf& Kd, const ColorXYZf& Ks, const float& Ior, const float& Exponent) :
		Type(Type),
		Brdf(N, Wo, Kd, Ks, Ior, Exponent),
		IsotropicPhase(Kd)
	{
	}

	HOST_DEVICE ColorXYZf F(Vec3f Wo, Vec3f Wi)
	{
		switch (this->Type)
		{
			case Enums::Brdf:
				return this->Brdf.F(Wo, Wi);

			case Enums::PhaseFunction:
				return this->IsotropicPhase.F(Wo, Wi);
		}

		return 1.0f;
	}

	HOST_DEVICE ColorXYZf SampleF(const Vec3f& Wo, Vec3f& Wi, float& Pdf, const BrdfSample& S)
	{
		switch (this->Type)
		{
			case Enums::Brdf:
				return this->Brdf.SampleF(Wo, Wi, Pdf, S);

			case Enums::PhaseFunction:
				return this->IsotropicPhase.SampleF(Wo, Wi, Pdf, S.Dir);
		}

		return ColorXYZf(0.0f);
	}

	HOST_DEVICE float Pdf(const Vec3f& Wo, const Vec3f& Wi)
	{
		switch (this->Type)
		{
			case Enums::Brdf:
				return this->Brdf.Pdf(Wo, Wi);

			case Enums::PhaseFunction:
				return this->IsotropicPhase.Pdf(Wo, Wi);
		}

		return 1.0f;
	}

	HOST_DEVICE Shader& operator = (const Shader& Other)
	{
		this->Type 				= Other.Type;
		this->Brdf 				= Other.Brdf;
		this->IsotropicPhase	= Other.IsotropicPhase;

		return *this;
	}

	Enums::ScatterFunction		Type;
	Brdf						Brdf;
	IsotropicPhase				IsotropicPhase;
};

}
