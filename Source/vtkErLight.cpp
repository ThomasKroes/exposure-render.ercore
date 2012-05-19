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
#include "vtkErLight.h"
#include "vtkErTexture.h"

vtkStandardNewMacro(vtkErLightData);
vtkCxxRevisionMacro(vtkErLightData, "$Revision: 1.0 $");

vtkStandardNewMacro(vtkErLight);
vtkCxxRevisionMacro(vtkErLight, "$Revision: 1.0 $");

vtkErLight::vtkErLight(void)
{
	this->SetNumberOfInputPorts(1);
	this->SetNumberOfOutputPorts(1);

	this->SetEnabled(true);
	this->SetVisible(true);
	this->SetMultiplier(100000.0f);
	this->SetEmissionUnit(Enums::Lux);
	this->TextureID = -1;
}

vtkErLight::~vtkErLight(void)
{
}

int vtkErLight::FillInputPortInformation(int Port, vtkInformation* Info)
{
	if (Port == 0)
	{
		Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErTextureData");
		Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 0);
		Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
	}

	return 1;
}

int vtkErLight::FillOutputPortInformation(int Port, vtkInformation* Info)
{
	if (Port == 0)
	{
		Info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkErLightData");
	}

	return 1;
}

int vtkErLight::RequestDataObject(vtkInformation* vtkNotUsed(request), vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* OutputVector)
{
	vtkInformation* OutInfo = OutputVector->GetInformationObject(0);
	vtkErLightData* Output = vtkErLightData::SafeDownCast(OutInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (!Output)
	{
		Output = vtkErLightData::New();
		OutInfo->Set(vtkDataObject::DATA_OBJECT(), Output);
		Output->FastDelete();
		Output->SetPipelineInformation(OutInfo);

		this->GetOutputPortInformation(0)->Set(vtkDataObject::DATA_EXTENT_TYPE(), Output->GetExtentType());
	}
 
	return 1;
}

int vtkErLight::RequestInformation(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
{
	return 1;
}

int vtkErLight::RequestData(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
{
	vtkInformation* InInfo = InputVector[0]->GetInformationObject(0);
	vtkInformation* OutInfo	= OutputVector->GetInformationObject(0);

	if (!InInfo || !OutInfo)
		return 0;
	
	vtkErTextureData* TextureDataIn	= vtkErTextureData::SafeDownCast(InInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (!TextureDataIn)
		return 0;

	vtkErLightData* LightDataOut = vtkErLightData::SafeDownCast(OutInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (!LightDataOut)
		return 0;

	ExposureRender::ErLight& ErLight = LightDataOut->Bindable;

	ErLight.Enabled						= this->GetEnabled();
	ErLight.Visible						= this->GetVisible();

	vtkErShape::RequestData(ErLight.Shape);

	ErLight.Multiplier					= this->GetMultiplier();
	ErLight.EmissionUnit				= this->GetEmissionUnit();
	ErLight.TextureID					= TextureDataIn->Bindable.ID;

	LightDataOut->Bind();

	return 1;
}

int vtkErLight::RequestUpdateExtent(vtkInformation* vtkNotUsed(Request), vtkInformationVector** InputVector, vtkInformationVector* vtkNotUsed(OutputVector))
{
	return 1;
}

int vtkErLight::ProcessRequest(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
{
	if (Request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT()))
		return this->RequestDataObject(Request, InputVector, OutputVector);
	
	if (Request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
		return this->RequestData(Request, InputVector, OutputVector);

	if (Request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
		return this->RequestUpdateExtent(Request, InputVector, OutputVector);

	if (Request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
		return this->RequestInformation(Request, InputVector, OutputVector);
	
	return this->Superclass::ProcessRequest(Request, InputVector, OutputVector);
}
