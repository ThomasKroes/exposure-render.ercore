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

#include "vtkErDll.h"
#include "vtkErBindable.h"

#include <vtkAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>

using namespace ExposureRender;

class VTK_ER_EXPORT vtkErVolumeProperty : public vtkAlgorithm
{
public:
	static vtkErVolumeProperty* New();
	vtkTypeRevisionMacro(vtkErVolumeProperty, vtkAlgorithm);

	void RequestData(ExposureRender::VolumeProperty& VolumeProperty);

	vtkPiecewiseFunction* GetOpacity()										{	return this->Opacity.GetPointer();																		};
	void SetOpacity(vtkPiecewiseFunction* Opacity)							{	this->Opacity = Opacity; this->Opacity->Modified(); this->Modified();									};
	
	vtkColorTransferFunction* GetDiffuse()									{	return this->Diffuse.GetPointer();																		};
	void SetDiffuse(vtkColorTransferFunction* Diffuse)						{	this->Diffuse = Diffuse; this->Diffuse->Modified(); this->Modified();									};

	vtkColorTransferFunction* GetSpecular()									{ 	return this->Specular.GetPointer();																		};
	void SetSpecular(vtkColorTransferFunction* Specular)					{ 	this->Specular = Specular; this->Specular->Modified(); this->Modified();								};

	vtkPiecewiseFunction* GetGlossiness()									{ 	return this->Glossiness.GetPointer();																	};
	void SetGlossiness(vtkPiecewiseFunction* Glossiness)					{ 	this->Glossiness = Glossiness; this->Glossiness->Modified(); this->Modified();							};
	
	vtkPiecewiseFunction* GetIndexOfReflection()							{ 	return this->IndexOfReflection.GetPointer();															};
	void SetIndexOfReflection(vtkPiecewiseFunction* IndexOfReflection)		{ 	this->IndexOfReflection = IndexOfReflection; this->IndexOfReflection->Modified(); this->Modified();		};

	vtkColorTransferFunction* GetEmission()									{ 	return this->Emission.GetPointer();																		};
	void SetEmission(vtkColorTransferFunction* Emission)					{ 	this->Emission = Emission; this->Emission->Modified(); this->Modified();								};

	vtkGetMacro(StepFactorPrimary, float);
	vtkSetMacro(StepFactorPrimary, float);
	
	vtkGetMacro(StepFactorShadow, float);
	vtkSetMacro(StepFactorShadow, float);

	vtkGetMacro(Shadows, bool);
	vtkSetMacro(Shadows, bool);
	
	vtkGetMacro(ShadingMode, Enums::ShadingMode);
	vtkSetMacro(ShadingMode, Enums::ShadingMode);

	vtkGetMacro(DensityScale, float);
	vtkSetMacro(DensityScale, float);

	vtkGetMacro(OpacityModulated, bool);
	vtkSetMacro(OpacityModulated, bool);

	vtkGetMacro(GradientMode, Enums::GradientMode);
	vtkSetMacro(GradientMode, Enums::GradientMode);

	vtkGetMacro(GradientThreshold, float);
	vtkSetMacro(GradientThreshold, float);

	vtkGetMacro(GradientFactor, float);
	vtkSetMacro(GradientFactor, float);

protected:
	vtkErVolumeProperty();
	virtual ~vtkErVolumeProperty() {};

private:
	vtkErVolumeProperty(const vtkErVolumeProperty& Other);		// Not implemented
    void operator = (const vtkErVolumeProperty& Other);			// Not implemented

	vtkSmartPointer<vtkPiecewiseFunction>			Opacity;
	vtkSmartPointer<vtkColorTransferFunction>		Diffuse;
	vtkSmartPointer<vtkColorTransferFunction>		Specular;
	vtkSmartPointer<vtkPiecewiseFunction>			Glossiness;
	vtkSmartPointer<vtkPiecewiseFunction>			IndexOfReflection;
	vtkSmartPointer<vtkColorTransferFunction>		Emission;
	unsigned long									LastOpacityTimeStamp;
	unsigned long									LastDiffuseTimeStamp;
	unsigned long									LastSpecularTimeStamp;
	unsigned long									LastGlossinessTimeStamp;
	unsigned long									LastIndexOfReflectionTimeStamp;
	unsigned long									LastEmissionTimeStamp;
	float											StepFactorPrimary;
	float											StepFactorShadow;
	bool											Shadows;
	Enums::ShadingMode								ShadingMode;
	float											DensityScale;
	bool											OpacityModulated;
	Enums::GradientMode								GradientMode;
	float											GradientThreshold;
	float											GradientFactor;
};
