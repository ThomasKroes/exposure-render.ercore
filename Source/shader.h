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

	HOST_DEVICE ColorXYZf SampleF(const Vec3f& Wo, Vec3f& Wi, float& Pdf, RNG& RNG)
	{
		switch (this->Type)
		{
			case Enums::Brdf:
				return this->Brdf.SampleF(Wo, Wi, Pdf, RNG);

			case Enums::PhaseFunction:
				return this->IsotropicPhase.SampleF(Wo, Wi, Pdf, RNG.Get2());
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

DEVICE void GetShader(Intersection Int, Shader& Shader, RNG& RNG, const int& VolumeID = 0)
{
	switch (Int.GetScatterType())
	{
		case Enums::Volume:
		{
			// Get reference to volume
			Volume& Volume = gpVolumes[gpTracer->VolumeIDs[VolumeID]];
			
			// Get reference to volume property
			VolumeProperty& VolumeProperty = gpTracer->VolumeProperty;

			const ColorXYZf Diffuse			= VolumeProperty.GetDiffuse(Int.GetIntensity());
			const ColorXYZf Specular		= VolumeProperty.GetSpecular(Int.GetIntensity());
			const float Glossiness			= VolumeProperty.GetGlossiness(Int.GetIntensity());
			const float IndexOfReflection	= VolumeProperty.GetIndexOfReflection(Int.GetIntensity());

			switch (VolumeProperty.GetShadingType())
			{
				case Enums::BrdfOnly:
				{
					Shader.Type	= Enums::Brdf;			
					Shader.Brdf	= Brdf(Int.GetN(), Int.GetWo(), Diffuse, Specular, IndexOfReflection, Glossiness);

					break;
				}

				case Enums::PhaseFunctionOnly:
				{
					Shader.Type				= Enums::PhaseFunction;
					Shader.IsotropicPhase	= IsotropicPhase(Diffuse);

					break;
				}

				case Enums::Hybrid:
				{
					const float GradientMagnitude			= Volume.GradientMagnitude(Int.GetP());
					const float NormalizedGradientMagnitude = GradientMagnitude / Volume.MaxGradientMagnitude;

					const float Sensitivity	= 25;
					const float ExpGF		= 3;
					const float Exponent	= Sensitivity * powf(VolumeProperty.GetGradientFactor(), ExpGF) * NormalizedGradientMagnitude;
					
					const float PdfBrdf = VolumeProperty.GetOpacityModulated() ? VolumeProperty.GetOpacity(Int.GetIntensity()) * (1.0f - __expf(-Exponent)) : (1.0f - __expf(-Exponent));
					
					if (RNG.Get1() < PdfBrdf)
					{
						Shader.Type	= Enums::Brdf;			
						Shader.Brdf	= Brdf(Int.GetN(), Int.GetWo(), Diffuse, Specular, IndexOfReflection, Glossiness);
					}
					else
					{
						Shader.Type				= Enums::PhaseFunction;
						Shader.IsotropicPhase	= IsotropicPhase(Diffuse);
					}

					break;
				}
				
				case Enums::Modulation:
				{
					const float GradientMagnitude = Volume.GradientMagnitude(Int.GetP());

					const float NormalizedGradientMagnitude = GradientMagnitude / Volume.MaxGradientMagnitude;
	
					const float PdfBrdf = 1.0f - powf(1.0f - NormalizedGradientMagnitude, 2.0f);
					
					if (RNG.Get1() < PdfBrdf)
					{
						Shader.Type	= Enums::Brdf;			
						Shader.Brdf	= Brdf(Int.GetN(), Int.GetWo(), Diffuse, Specular, IndexOfReflection, Glossiness);
					}
					else
					{
						Shader.Type				= Enums::PhaseFunction;
						Shader.IsotropicPhase	= IsotropicPhase(Diffuse);
					}

					break;
				}
			}

			break;
		}

		case Enums::Light:
		{
			break;
		}

		case Enums::Object:
		{
			const ColorXYZf Diffuse		= EvaluateTexture(gpObjects[Int.GetID()].DiffuseTextureID, Int.GetUV());
			const ColorXYZf Specular	= EvaluateTexture(gpObjects[Int.GetID()].SpecularTextureID, Int.GetUV());
			const ColorXYZf Glossiness	= EvaluateTexture(gpObjects[Int.GetID()].GlossinessTextureID, Int.GetUV());

			Shader.Type	= Enums::Brdf;			
			Shader.Brdf	= Brdf(Int.GetN(), Int.GetWo(), Diffuse, Specular, 15, 500);

			break;
		}
	}
}

}
