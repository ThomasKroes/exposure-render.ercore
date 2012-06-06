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
	IndexOfReflection->AddPoint(Min, 15.0f);
	IndexOfReflection->AddPoint(Max, 15.0f);
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
	if (this->LastOpacityTimeStamp < Opacity->GetMTime())
	{
		VolumeProperty.Opacity1D.Reset();
		
		for (int j = 0; j < GetOpacity()->GetSize(); j++)
		{
			double NodeValue[4];
			GetOpacity()->GetNodeValue(j, NodeValue);
			VolumeProperty.Opacity1D.AddNode(ExposureRender::ScalarNode(NodeValue[0], NodeValue[1]));
		}

		VolumeProperty.Opacity1D.TimeStamp.Modified();

		this->LastOpacityTimeStamp = Opacity->GetMTime();
	}

	if (this->LastDiffuseTimeStamp < Diffuse->GetMTime())
	{
		VolumeProperty.Diffuse1D.Reset();

		for (int j = 0; j < GetDiffuse()->GetSize(); j++)
		{
			double NodeValue[6];
			GetDiffuse()->GetNodeValue(j, NodeValue);
			VolumeProperty.Diffuse1D.AddNode(ExposureRender::ColorNode::FromRGB(NodeValue[0], ExposureRender::ColorRGBf(NodeValue[1], NodeValue[2], NodeValue[3])));
		}

		VolumeProperty.Diffuse1D.TimeStamp.Modified();

		this->LastDiffuseTimeStamp = Diffuse->GetMTime();
	}
	
	if (this->LastSpecularTimeStamp < Specular->GetMTime())
	{
		VolumeProperty.Specular1D.Reset();

		for (int j = 0; j < GetSpecular()->GetSize(); j++)
		{
			double NodeValue[6];
			GetSpecular()->GetNodeValue(j, NodeValue);
			VolumeProperty.Specular1D.AddNode(ExposureRender::ColorNode::FromRGB(NodeValue[0], ExposureRender::ColorRGBf(NodeValue[1], NodeValue[2], NodeValue[3])));
		}

		VolumeProperty.Specular1D.TimeStamp.Modified();

		this->LastSpecularTimeStamp = Specular->GetMTime();
	}

	if (this->LastGlossinessTimeStamp < Glossiness->GetMTime())
	{
		VolumeProperty.Glossiness1D.Reset();
		
		for (int j = 0; j < GetGlossiness()->GetSize(); j++)
		{
			double NodeValue[4];
			GetGlossiness()->GetNodeValue(j, NodeValue);
			VolumeProperty.Glossiness1D.AddNode(ExposureRender::ScalarNode(NodeValue[0], NodeValue[1]));
		}

		VolumeProperty.Glossiness1D.TimeStamp.Modified();

		this->LastGlossinessTimeStamp = Glossiness->GetMTime();
	}

	if (this->LastIndexOfReflectionTimeStamp < IndexOfReflection->GetMTime())
	{
		VolumeProperty.IndexOfReflection1D.Reset();
		
		for (int j = 0; j < GetIndexOfReflection()->GetSize(); j++)
		{
			double NodeValue[4];
			GetIndexOfReflection()->GetNodeValue(j, NodeValue);
			VolumeProperty.IndexOfReflection1D.AddNode(ExposureRender::ScalarNode(NodeValue[0], NodeValue[1]));
		}

		VolumeProperty.IndexOfReflection1D.TimeStamp.Modified();

		this->LastIndexOfReflectionTimeStamp = IndexOfReflection->GetMTime();
	}

	if (this->LastEmissionTimeStamp < Emission->GetMTime())
	{
		VolumeProperty.Emission1D.Reset();

		for (int j = 0; j < GetEmission()->GetSize(); j++)
		{
			double NodeValue[6];
			GetEmission()->GetNodeValue(j, NodeValue);
			VolumeProperty.Emission1D.AddNode(ExposureRender::ColorNode::FromRGB(NodeValue[0], ExposureRender::ColorRGBf(NodeValue[1], NodeValue[2], NodeValue[3])));
		}

		VolumeProperty.Emission1D.TimeStamp.Modified();

		this->LastEmissionTimeStamp = Emission->GetMTime();
	}

	VolumeProperty.StepFactorPrimary 	= GetStepFactorPrimary();
	VolumeProperty.StepFactorShadow		= GetStepFactorShadow();
	VolumeProperty.Shadows				= GetShadows();
	VolumeProperty.ShadingType			= GetShadingMode();
	VolumeProperty.DensityScale			= GetDensityScale();
	VolumeProperty.OpacityModulated		= GetOpacityModulated();
	VolumeProperty.GradientMode			= GetGradientMode();
	VolumeProperty.GradientThreshold	= GetGradientThreshold();
	VolumeProperty.GradientFactor		= GetGradientFactor();
}
