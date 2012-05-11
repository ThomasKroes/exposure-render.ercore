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
#include "vtkErObject.h"
#include "vtkErTexture.h"

vtkStandardNewMacro(vtkErObjectData);
vtkCxxRevisionMacro(vtkErObjectData, "$Revision: 1.0 $");

vtkStandardNewMacro(vtkErObject);
vtkCxxRevisionMacro(vtkErObject, "$Revision: 1.0 $");

vtkErObject::vtkErObject(void)
{
	this->SetNumberOfInputPorts(3);
	this->SetNumberOfOutputPorts(1);
}

vtkErObject::~vtkErObject(void)
{
}

int vtkErObject::FillInputPortInformation(int Port, vtkInformation* Info)
{
	if (Port == 0)
	{
		Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErTextureData");
		Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 0);
		Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
	}

	if (Port == 1)
	{
		Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErTextureData");
		Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 0);
		Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
	}

	if (Port == 2)
	{
		Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErTextureData");
		Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 0);
		Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
	}

	return 1;
}

int vtkErObject::FillOutputPortInformation(int Port, vtkInformation* Info)
{
	if (Port == 0)
	{
		Info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkErObjectData");
	}

	return 1;
}

int vtkErObject::RequestDataObject(vtkInformation* vtkNotUsed(request), vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* OutputVector)
{
	vtkInformation* OutInfo = OutputVector->GetInformationObject(0);
	vtkErObjectData* Output = vtkErObjectData::SafeDownCast(OutInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (!Output)
	{
		Output = vtkErObjectData::New();
		OutInfo->Set(vtkDataObject::DATA_OBJECT(), Output);
		Output->FastDelete();
		Output->SetPipelineInformation(OutInfo);

		this->GetOutputPortInformation(0)->Set(vtkDataObject::DATA_EXTENT_TYPE(), Output->GetExtentType());
	}
 
	return 1;
}

int vtkErObject::RequestInformation(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
{
	return 1;
}

int vtkErObject::RequestData(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
{
	vtkInformation* InInfo	= InputVector[0]->GetInformationObject(0);
	vtkInformation* OutInfo	= OutputVector->GetInformationObject(0);
	
	vtkErObjectData* ObjectDataOut = vtkErObjectData::SafeDownCast(OutInfo->Get(vtkDataObject::DATA_OBJECT()));
	
	ObjectDataOut->Bindable.Shape.OneSided				= this->GetOneSided();
	ObjectDataOut->Bindable.Shape.Type					= this->GetShapeType();
	ObjectDataOut->Bindable.Shape.Size					= this->GetSize();
	ObjectDataOut->Bindable.Shape.InnerRadius			= this->GetInnerRadius();
	ObjectDataOut->Bindable.Shape.OuterRadius			= this->GetOuterRadius();
	ObjectDataOut->Bindable.Shape.Alignment.Type		= this->GetAlignmentType();
	ObjectDataOut->Bindable.Shape.Alignment.Axis		= this->GetAxis();
	ObjectDataOut->Bindable.Shape.Alignment.AutoFlip	= this->GetAutoFlip();
	ObjectDataOut->Bindable.Shape.Alignment.Position	= Vec3f(this->GetPosition()[0], this->GetPosition()[1], this->GetPosition()[2]);
	ObjectDataOut->Bindable.Shape.Alignment.Target		= Vec3f(this->GetTarget()[0], this->GetTarget()[1], this->GetTarget()[2]);
	ObjectDataOut->Bindable.Shape.Alignment.Up			= Vec3f(this->GetUp()[0], this->GetUp()[1], this->GetUp()[2]);
	ObjectDataOut->Bindable.Shape.Alignment.Elevation	= this->GetElevation();
	ObjectDataOut->Bindable.Shape.Alignment.Azimuth		= this->GetAzimuth();
	ObjectDataOut->Bindable.Shape.Alignment.Offset		= this->GetOffset();

	vtkErTextureData* Diffuse		= vtkErTextureData::SafeDownCast(this->GetInputDataObject(0, 0));
	vtkErTextureData* Specular		= vtkErTextureData::SafeDownCast(this->GetInputDataObject(1, 0));
	vtkErTextureData* Glossiness	= vtkErTextureData::SafeDownCast(this->GetInputDataObject(2, 0));

	if (Diffuse)
		ObjectDataOut->Bindable.DiffuseTextureID = Diffuse->Bindable.ID;

	if (Specular)
		ObjectDataOut->Bindable.SpecularTextureID = Specular->Bindable.ID;

	if (Glossiness)
		ObjectDataOut->Bindable.GlossinessTextureID = Glossiness->Bindable.ID;
	
	ObjectDataOut->Bind();

	return 1;
}

int vtkErObject::RequestUpdateExtent(vtkInformation* vtkNotUsed(Request), vtkInformationVector** InputVector, vtkInformationVector* vtkNotUsed(OutputVector))
{
	return 1;
}

int vtkErObject::ProcessRequest(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
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
