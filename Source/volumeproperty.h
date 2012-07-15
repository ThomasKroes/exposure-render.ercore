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

#include "transferfunctions.h"
#include "buffers.h"

namespace ExposureRender
{

/*! \class VolumeProperty
 * \brief Volume property class which determines the appearance of the volume
 */
class EXPOSURE_RENDER_DLL VolumeProperty : public TimeStamp
{
public:
	/*! Default constructor */
	HOST VolumeProperty() :
		TimeStamp(),
		Opacity1D("Opacity"),
		Diffuse1D("Diffuse"),
		Specular1D("Specular"),
		Glossiness1D("Glossiness"),
		IndexOfReflection1D("IOR"),
		Emission1D("Emission"),
		StepFactorPrimary(2),
		StepFactorShadow(2),
		Shadows(true),
		ShadingType(Enums::BrdfOnly),
		DensityScale(100),
		OpacityModulated(true),
		GradientFactor(0.5f),
		GradientMode(Enums::CentralDifferences)
	{
	}
	
	/*! Copy constructor
		@param[in] Other Volume property to copy
	*/
	HOST VolumeProperty(const VolumeProperty& Other) :
		TimeStamp(),
		Opacity1D("Opacity"),
		Diffuse1D("Diffuse"),
		Specular1D("Specular"),
		Glossiness1D("Glossiness"),
		IndexOfReflection1D("IOR"),
		Emission1D("Emission"),
		StepFactorPrimary(2),
		StepFactorShadow(2),
		Shadows(true),
		ShadingType(Enums::BrdfOnly),
		DensityScale(100),
		OpacityModulated(true),
		GradientFactor(0.5f),
		GradientMode(Enums::CentralDifferences)
	{
		*this = Other;
	}

	/*! Assignment operator
		@param[in] Other Volume property to copy
		@result Volume property
	*/
	HOST VolumeProperty& operator = (const VolumeProperty& Other)
	{
		TimeStamp::operator = (Other);

		this->Opacity1D				= Other.Opacity1D;
		this->Diffuse1D				= Other.Diffuse1D;
		this->Specular1D			= Other.Specular1D;
		this->Glossiness1D			= Other.Glossiness1D;
		this->IndexOfReflection1D	= Other.IndexOfReflection1D;
		this->Emission1D			= Other.Emission1D;
		this->StepFactorPrimary		= Other.StepFactorPrimary;
		this->StepFactorShadow		= Other.StepFactorShadow;
		this->Shadows				= Other.Shadows;
		this->ShadingType			= Other.ShadingType;
		this->DensityScale			= Other.DensityScale;
		this->OpacityModulated		= Other.OpacityModulated;
		this->GradientFactor		= Other.GradientFactor;
		this->GradientMode			= Other.GradientMode;
		
		return *this;
	}
	
	/*! Gets the opacity at \a Intensity from the opacity transfer function
		@param[in] Intensity Intensity at which to fetch the opacity
		@result Opacity
	*/
	DEVICE float GetOpacity(const unsigned short& Intensity)
	{
		return this->Opacity1D.Evaluate(Intensity);
	}
	
	/*! Gets the diffuse color at \a Intensity from the diffuse transfer function
		@param[in] Intensity Intensity at which to fetch the diffuse color
		@result Diffuse color
	*/
	DEVICE ColorXYZf GetDiffuse(const unsigned short& Intensity)
	{
		return this->Diffuse1D.Evaluate(Intensity);
	}
	
	/*! Gets the specular color at \a Intensity from the specular transfer function
		@param[in] Intensity Intensity at which to fetch the specular color
		@result Specular color
	*/
	DEVICE ColorXYZf GetSpecular(const unsigned short& Intensity)
	{
		return this->Specular1D.Evaluate(Intensity);
	}
	
	/*! Gets the glossiness at \a Intensity from the glossiness transfer function
		@param[in] Intensity Intensity at which to fetch the glossiness
		@result Glossiness
	*/
	DEVICE float GetGlossiness(const unsigned short& Intensity)
	{
		return this->Glossiness1D.Evaluate(Intensity);
	}
	
	/*! Gets the index of reflection at \a Intensity from the index of reflection transfer function
		@param[in] Intensity Intensity at which to fetch the index of reflection
		@result Index of reflection
	*/
	DEVICE float GetIndexOfReflection(const unsigned short& Intensity)
	{
		return this->IndexOfReflection1D.Evaluate(Intensity);
	}
	
	/*! Gets the emission color at \a Intensity from the emission transfer function
		@param[in] Intensity Intensity at which to fetch the emission color
		@result Emission color
	*/
	DEVICE ColorXYZf GetEmission(const unsigned short& Intensity)
	{
		return this->Emission1D.Evaluate(Intensity);
	}

	HOST_DEVICE GET_REF_SET_MACRO(Opacity1D, ScalarTransferFunction1D)
	HOST_DEVICE GET_REF_SET_MACRO(Diffuse1D, ColorTransferFunction1D)
	HOST_DEVICE GET_REF_SET_MACRO(Specular1D, ColorTransferFunction1D)
	HOST_DEVICE GET_REF_SET_MACRO(Glossiness1D, ScalarTransferFunction1D)
	HOST_DEVICE GET_REF_SET_MACRO(IndexOfReflection1D, ScalarTransferFunction1D)
	HOST_DEVICE GET_REF_SET_MACRO(Emission1D, ColorTransferFunction1D)
	HOST_DEVICE GET_SET_MACRO(StepFactorPrimary, float)
	HOST_DEVICE GET_SET_MACRO(StepFactorShadow, float)
	HOST_DEVICE GET_SET_MACRO(Shadows, bool)
	HOST_DEVICE GET_SET_MACRO(ShadingType, Enums::ShadingMode)
	HOST_DEVICE GET_SET_MACRO(DensityScale, float)
	HOST_DEVICE GET_SET_MACRO(OpacityModulated, bool)
	HOST_DEVICE GET_SET_MACRO(GradientFactor, float)
	HOST_DEVICE GET_SET_MACRO(GradientMode, Enums::GradientMode)

protected:
	ScalarTransferFunction1D	Opacity1D;					/*! Opacity transfer function */
	ColorTransferFunction1D		Diffuse1D;					/*! Diffuse color transfer function */
	ColorTransferFunction1D		Specular1D;					/*! Specular color transfer function */
	ScalarTransferFunction1D	Glossiness1D;				/*! Glossiness transfer function */
	ScalarTransferFunction1D	IndexOfReflection1D;		/*! Index of reflection transfer function */
	ColorTransferFunction1D		Emission1D;					/*! Emission color transfer function */
	float						StepFactorPrimary;			/*! Primary step factor for camera rays */
	float						StepFactorShadow;			/*! Step factor for shadow rays */
	bool						Shadows;					/*! Whether to render shadows */
	Enums::ShadingMode			ShadingType;				/*! Type of shading */
	float						DensityScale;				/*! Overall density scale of the volume */
	bool						OpacityModulated;			/*! Whether hybrid scattering is opacity modulated or not */
	float						GradientFactor;				/*! Parameter which controls the amount of BRDF vs. Phase function scattering */
	Enums::GradientMode			GradientMode;				/*! Determines how gradients are computed */

	friend class Tracer;
};

}
