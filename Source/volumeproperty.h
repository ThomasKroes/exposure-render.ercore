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

#include "transferfunction.h"

namespace ExposureRender
{

class EXPOSURE_RENDER_DLL VolumeProperty : public TimeStamp
{
public:
	HOST VolumeProperty() :
		TimeStamp(),
		Opacity1D(),
		Diffuse1D(),
		Specular1D(),
		Glossiness1D(),
		IndexOfReflection1D(),
		Emission1D(),
		G(),
		StepFactorPrimary(2),
		StepFactorShadow(2),
		Shadows(true),
		ShadingType(Enums::BrdfOnly),
		DensityScale(100),
		OpacityModulated(true),
		GradientMode(Enums::CentralDifferences),
		GradientThreshold(0.5f),
		GradientFactor(0.5f)
	{
	}

	HOST VolumeProperty(const VolumeProperty& Other) :
		TimeStamp(),
		Opacity1D(),
		Diffuse1D(),
		Specular1D(),
		Glossiness1D(),
		IndexOfReflection1D(),
		Emission1D(),
		G(),
		StepFactorPrimary(2),
		StepFactorShadow(2),
		Shadows(true),
		ShadingType(Enums::BrdfOnly),
		DensityScale(100),
		OpacityModulated(true),
		GradientMode(Enums::CentralDifferences),
		GradientThreshold(0.5f),
		GradientFactor(0.5f)
	{
		*this = Other;
	}

	HOST VolumeProperty& operator = (const VolumeProperty& Other)
	{
		this->Opacity1D				= Other.Opacity1D;
		this->Diffuse1D				= Other.Diffuse1D;
		this->Specular1D			= Other.Specular1D;
		this->Glossiness1D			= Other.Glossiness1D;
		this->IndexOfReflection1D	= Other.IndexOfReflection1D;
		this->Emission1D			= Other.Emission1D;
		this->G						= Other.G;
		this->StepFactorPrimary		= Other.StepFactorPrimary;
		this->StepFactorShadow		= Other.StepFactorShadow;
		this->Shadows				= Other.Shadows;
		this->ShadingType			= Other.ShadingType;
		this->DensityScale			= Other.DensityScale;
		this->OpacityModulated		= Other.OpacityModulated;
		this->GradientMode			= Other.GradientMode;
		this->GradientThreshold		= Other.GradientThreshold;
		this->GradientFactor		= Other.GradientFactor;
		
		return *this;
	}

	ScalarTransferFunction1D	Opacity1D;
	ColorTransferFunction1D		Diffuse1D;
	ColorTransferFunction1D		Specular1D;
	ScalarTransferFunction1D	Glossiness1D;
	ScalarTransferFunction1D	IndexOfReflection1D;
	ColorTransferFunction1D		Emission1D;
	ScalarTransferFunction1D	G;
	float						StepFactorPrimary;
	float						StepFactorShadow;
	bool						Shadows;
	Enums::ShadingMode			ShadingType;
	float						DensityScale;
	bool						OpacityModulated;
	Enums::GradientMode			GradientMode;
	float						GradientThreshold;
	float						GradientFactor;
};

}
