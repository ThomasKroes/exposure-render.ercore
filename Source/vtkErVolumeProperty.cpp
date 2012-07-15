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

#include "vtkErStable.h"
#include "vtkErVolumeProperty.h"

vtkStandardNewMacro(vtkErVolumeProperty);
vtkCxxRevisionMacro(vtkErVolumeProperty, "$Revision: 1.0 $");

vtkErVolumeProperty::vtkErVolumeProperty()
{
	this->Opacity				= vtkPiecewiseFunction::New();
	this->Diffuse				= vtkColorTransferFunction::New();
	this->Specular				= vtkColorTransferFunction::New();
	this->Glossiness			= vtkPiecewiseFunction::New();
	this->IndexOfReflection		= vtkPiecewiseFunction::New();
	this->Emission				= vtkColorTransferFunction::New();

	double Min = 0, Max = 2048;

	Opacity->AddPoint(Min, 0);
	Opacity->AddPoint(Max, 1);
	Diffuse->AddRGBPoint(Min, 1, 1, 1);
	Diffuse->AddRGBPoint(Max, 1, 1, 1);
	Specular->AddRGBPoint(Min, 0, 0, 0);
	Specular->AddRGBPoint(Max, 0, 0, 0);
	Glossiness->AddPoint(Min, 1);
	Glossiness->AddPoint(Max, 1);
	IndexOfReflection->AddPoint(Min, 50.0f);
	IndexOfReflection->AddPoint(Max, 50.0f);
	Emission->AddRGBPoint(Min, 0, 0, 0);
	Emission->AddRGBPoint(Max, 0, 0, 0);
	
	this->LastOpacityTimeStamp				= 0;
	this->LastDiffuseTimeStamp				= 0;
	this->LastSpecularTimeStamp				= 0;
	this->LastGlossinessTimeStamp			= 0;
	this->LastIndexOfReflectionTimeStamp	= 0;
	this->LastEmissionTimeStamp				= 0;

	this->SetStepFactorPrimary(3.0f);
	this->SetStepFactorShadow(3.0f);
	this->SetShadows(true);
	this->SetShadingMode(Enums::PhaseFunctionOnly);
	this->SetDensityScale(10.0f);
	this->SetOpacityModulated(true);
	this->SetGradientMode(Enums::CentralDifferences);
	this->SetGradientThreshold(0.5f);
	this->SetGradientFactor(1.0f);
}

void vtkErVolumeProperty::RequestData(ExposureRender::VolumeProperty& VolumeProperty)
{
	// Only copy when needed
	if (this->LastOpacityTimeStamp < Opacity->GetMTime())
	{
		// Get reference to transfer function
		ScalarTransferFunction1D& Opacity1D = VolumeProperty.GetOpacity1D();

		// Reset
		Opacity1D.Reset();
		
		// Copy
		for (int j = 0; j < GetOpacity()->GetSize(); j++)
		{
			double NodeValue[4];
			GetOpacity()->GetNodeValue(j, NodeValue);
			Opacity1D.AddNode(NodeValue[0], NodeValue[1]);
		}
		
		// Record last time stamp
		this->LastOpacityTimeStamp = Opacity->GetMTime();
	}

	// Only copy when needed
	if (this->LastDiffuseTimeStamp < Diffuse->GetMTime())
	{
		// Get reference to transfer function
		ColorTransferFunction1D& Diffuse1D = VolumeProperty.GetDiffuse1D();

		// Reset
		Diffuse1D.Reset();

		// Copy
		for (int j = 0; j < GetDiffuse()->GetSize(); j++)
		{
			double NodeValue[6];
			GetDiffuse()->GetNodeValue(j, NodeValue);
			const ColorXYZf XYZ = RGBfToXYZf(ColorRGBf(NodeValue[1], NodeValue[2], NodeValue[3]));
			Diffuse1D.AddNode(NodeValue[0], XYZ);
		}
		
		// Record last time stamp
		this->LastDiffuseTimeStamp = Diffuse->GetMTime();
	}
	
	// Only copy when needed
	if (this->LastSpecularTimeStamp < Specular->GetMTime())
	{
		// Get reference to transfer function
		ColorTransferFunction1D& Specular1D = VolumeProperty.GetSpecular1D();

		// Reset
		Specular1D.Reset();

		// Copy
		for (int j = 0; j < GetSpecular()->GetSize(); j++)
		{
			double NodeValue[6];
			GetSpecular()->GetNodeValue(j, NodeValue);
			const ColorXYZf XYZ = RGBfToXYZf(ColorRGBf(NodeValue[1], NodeValue[2], NodeValue[3]));
			Specular1D.AddNode(NodeValue[0], XYZ);
		}
		
		// Record last time stamp
		this->LastSpecularTimeStamp = Specular->GetMTime();
	}

	// Only copy when needed
	if (this->LastGlossinessTimeStamp < Glossiness->GetMTime())
	{
		// Get reference to transfer function
		ScalarTransferFunction1D& Glossiness1D = VolumeProperty.GetGlossiness1D();
		
		// Reset
		Glossiness1D.Reset();
		
		// Copy
		for (int j = 0; j < GetGlossiness()->GetSize(); j++)
		{
			double NodeValue[4];
			GetGlossiness()->GetNodeValue(j, NodeValue);
			Glossiness1D.AddNode(NodeValue[0], NodeValue[1]);
		}
		
		// Record last time stamp
		this->LastGlossinessTimeStamp = Glossiness->GetMTime();
	}

	// Only copy when needed
	if (this->LastIndexOfReflectionTimeStamp < IndexOfReflection->GetMTime())
	{
		// Get reference to transfer function
		ScalarTransferFunction1D& IndexOfReflection1D = VolumeProperty.GetIndexOfReflection1D();

		// Reset
		IndexOfReflection1D.Reset();
		
		// Copy
		for (int j = 0; j < GetIndexOfReflection()->GetSize(); j++)
		{
			double NodeValue[4];
			GetIndexOfReflection()->GetNodeValue(j, NodeValue);
			IndexOfReflection1D.AddNode(NodeValue[0], NodeValue[1]);
		}
		
		// Record last time stamp
		this->LastIndexOfReflectionTimeStamp = IndexOfReflection->GetMTime();
	}
	
	// Only copy when needed
	if (this->LastEmissionTimeStamp < Emission->GetMTime())
	{
		// Get reference to transfer function
		ColorTransferFunction1D& Emission1D = VolumeProperty.GetEmission1D();

		// Reset
		Emission1D.Reset();

		// Copy
		for (int j = 0; j < GetEmission()->GetSize(); j++)
		{
			double NodeValue[6];
			GetEmission()->GetNodeValue(j, NodeValue);
			const ColorXYZf XYZ = RGBfToXYZf(ColorRGBf(NodeValue[1], NodeValue[2], NodeValue[3]));
			Emission1D.AddNode(NodeValue[0], XYZ);
		}

		// Record last time stamp
		this->LastEmissionTimeStamp = Emission->GetMTime();
	}

	VolumeProperty.SetStepFactorPrimary(this->GetStepFactorPrimary());
	VolumeProperty.SetStepFactorShadow(this->GetStepFactorShadow());
	VolumeProperty.SetShadows(this->GetShadows());
	VolumeProperty.SetShadingType(this->GetShadingMode());
	VolumeProperty.SetDensityScale(this->GetDensityScale());
	VolumeProperty.SetOpacityModulated(this->GetOpacityModulated());
	VolumeProperty.SetGradientMode(this->GetGradientMode());
	VolumeProperty.SetGradientFactor(this->GetGradientFactor());
}
