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

using namespace std;

namespace ExposureRender
{

class Intersection
{
public:
	HOST_DEVICE Intersection() :
		Valid(false),
		Front(true),
		T(FLT_MAX),
		P(),
		N(),
		UV(),
		Intensity(),
		ScatterType(Enums::Volume),
		ID(-1)
	{
	}

	HOST_DEVICE Intersection(const Intersection& Other) :
		Valid(false),
		Front(true),
		T(FLT_MAX),
		P(),
		N(),
		UV(),
		Intensity(),
		ScatterType(Enums::Volume),
		ID(-1)
	{
		*this = Other;
	}

	HOST_DEVICE Intersection& Intersection::operator = (const Intersection& Other)
	{
		this->Valid			= Other.Valid;
		this->Front			= Other.Front;
		this->T				= Other.T;
		this->P				= Other.P;
		this->N				= Other.N;
		this->UV			= Other.UV;
		this->Intensity		= Other.Intensity;
		this->ScatterType	= Other.ScatterType;
		this->ID			= Other.ID;

		return *this;
	}
/*
#ifdef __CUDACC__
	DEVICE void GetShader(Shader& Shader, RNG& RNG, const int& VolumeID = 0)
	{
		switch (this->ScatterType)
		{
			case Enums::Volume:
			{
				VolumeProperty& VolumeProperty = gpTracer->VolumeProperty;

				const ColorXYZf Diffuse			= gpTracer->GetDiffuse(this->Intensity);//VolumeProperty.Diffuse1D.Evaluate(this->Intensity);
				const ColorXYZf Specular		= gpTracer->GetSpecular(this->Intensity);//VolumeProperty.Specular1D.Evaluate(this->Intensity);
				const float Glossiness			= gpTracer->GetGlossiness(this->Intensity);//VolumeProperty.Glossiness1D.Evaluate(this->Intensity);
				const float IndexOfReflection	= gpTracer->GetIndexOfReflection(this->Intensity);//VolumeProperty.IndexOfReflection1D.Evaluate(this->Intensity);

				switch (VolumeProperty.ShadingType)
				{
					case Enums::BrdfOnly:
					{
						Shader.Type	= Enums::Brdf;			
						Shader.Brdf	= Brdf(this->N, this->Wo, Diffuse, Specular, IndexOfReflection, GlossinessExponent(Glossiness));

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
						Volume& Volume = gpVolumes[gpTracer->VolumeIDs[VolumeID]];

						const float GradientMagnitude			= Volume.GradientMagnitude(this->P);
						const float NormalizedGradientMagnitude = GradientMagnitude / Volume.MaxGradientMagnitude;

						const float Sensitivity	= 25;
						const float ExpGF		= 3;
						const float Exponent	= Sensitivity * powf(VolumeProperty.GradientFactor, ExpGF) * NormalizedGradientMagnitude;
						
						const float PdfBrdf = gpTracer->VolumeProperty.OpacityModulated ? gpTracer->GetOpacity(this->Intensity) * (1.0f - __expf(-Exponent)) : (1.0f - __expf(-Exponent));
						
						if (RNG.Get1() < PdfBrdf)
						{
							Shader.Type	= Enums::Brdf;			
							Shader.Brdf	= Brdf(this->N, this->Wo, Diffuse, Specular, IndexOfReflection, GlossinessExponent(Glossiness));
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
						Volume& Volume = gpVolumes[gpTracer->VolumeIDs[VolumeID]];

						const float GradientMagnitude = Volume.GradientMagnitude(this->P);

						const float NormalizedGradientMagnitude = GradientMagnitude / Volume.MaxGradientMagnitude;
		
						const float PdfBrdf = 1.0f - powf(1.0f - NormalizedGradientMagnitude, 2.0f);
						
						if (RNG.Get1() < PdfBrdf)
						{
							Shader.Type	= Enums::Brdf;			
							Shader.Brdf	= Brdf(this->N, this->Wo, Diffuse, Specular, IndexOfReflection, GlossinessExponent(Glossiness));
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
				const ColorXYZf Diffuse		= EvaluateTexture(gpObjects[this->ID].DiffuseTextureID, this->UV);
				const ColorXYZf Specular	= EvaluateTexture(gpObjects[this->ID].SpecularTextureID, this->UV);
				const ColorXYZf Glossiness	= EvaluateTexture(gpObjects[this->ID].GlossinessTextureID, this->UV);

				Shader.Type	= Enums::Brdf;			
				Shader.Brdf	= Brdf(this->N, this->Wo, Diffuse, Specular, 15, 500);

				break;
			}

			case Enums::SlicePlane:
			{
				break;
			}
		}
	}
#endif
*/

	bool					Valid;
	bool					Front;
	float					T;
	Vec3f					P;
	Vec3f					N;
	Vec2f					UV;
	float					Intensity;
	Enums::ScatterType		ScatterType;
	int						ID;
};

}
