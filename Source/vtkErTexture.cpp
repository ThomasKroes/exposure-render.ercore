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
#include "vtkErTexture.h"
#include "vtkErBitmap.h"

vtkStandardNewMacro(vtkErTextureData);
vtkCxxRevisionMacro(vtkErTextureData, "$Revision: 1.0 $");

vtkStandardNewMacro(vtkErTexture);
vtkCxxRevisionMacro(vtkErTexture, "$Revision: 1.0 $");

vtkErTexture::vtkErTexture(void)
{
	this->SetNumberOfInputPorts(1);
	this->SetNumberOfOutputPorts(1);

	this->SetTextureType(Enums::Procedural);
	this->SetOutputLevel(1.0f);
	this->SetProceduralType(Enums::Uniform);
	this->SetUniformColor(1.0f, 1.0f, 1.0f);
	this->SetCheckerColor1(0.8f, 0.8f, 0.8f);
	this->SetCheckerColor2(0.3f, 0.3f, 0.3f);
	this->Gradient = vtkColorTransferFunction::New();
	this->Gradient->AddRGBPoint(0, 0, 0, 0);
	this->Gradient->AddRGBPoint(1, 1, 1, 1);
	this->SetOffset(0.0f, 0.0f);
	this->SetRepeat(1.0f, 1.0f);
	this->SetFlip(0, 0);
	this->BitmapID = -1;
}

vtkErTexture::~vtkErTexture(void)
{
}

int vtkErTexture::FillInputPortInformation(int Port, vtkInformation* Info)
{
	if (Port == 0)
	{
		Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErBitmapData");
		Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 0);
		Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
	}

	return 1;
}

int vtkErTexture::FillOutputPortInformation(int Port, vtkInformation* Info)
{
	Info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkErTextureData");

	return 1;
}

int vtkErTexture::RequestDataObject(vtkInformation* vtkNotUsed(request), vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* OutputVector)
{
	vtkInformation* OutInfo = OutputVector->GetInformationObject(0);

	if (!OutInfo)
		return 0;

	vtkErTextureData* TextureDataOut = vtkErTextureData::SafeDownCast(OutInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (!TextureDataOut)
	{
		TextureDataOut = vtkErTextureData::New();
		OutInfo->Set(vtkDataObject::DATA_OBJECT(), TextureDataOut);
		TextureDataOut->FastDelete();
		TextureDataOut->SetPipelineInformation(OutInfo);

		this->GetOutputPortInformation(0)->Set(vtkDataObject::DATA_EXTENT_TYPE(), TextureDataOut->GetExtentType());
	}

	return 1;
}

int vtkErTexture::RequestInformation(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
{
	return 1;
}

int vtkErTexture::RequestData(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
{
	vtkInformation* InInfo = InputVector[0]->GetInformationObject(0);
	vtkInformation* OutInfo	= OutputVector->GetInformationObject(0);

	if (!OutInfo)
		return 0;
	
	vtkErTextureData* TextureDataOut = vtkErTextureData::SafeDownCast(OutInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (!TextureDataOut)
		return 0;

	TextureDataOut->Bindable.Type			= this->GetTextureType();
	TextureDataOut->Bindable.OutputLevel	= this->GetOutputLevel();
	
	ExposureRender::Procedural& Procedural = TextureDataOut->Bindable.Procedural;

	Procedural.Type				= this->GetProceduralType();
	Procedural.UniformColor		= ColorXYZf::FromRGBf(ColorRGBf(this->GetUniformColor()[0], this->GetUniformColor()[1], this->GetUniformColor()[2]));
	Procedural.CheckerColor1	= ColorXYZf::FromRGBf(ColorRGBf(this->GetCheckerColor1()[0], this->GetCheckerColor1()[1], this->GetCheckerColor1()[2]));
	Procedural.CheckerColor2	= ColorXYZf::FromRGBf(ColorRGBf(this->GetCheckerColor2()[0], this->GetCheckerColor2()[1], this->GetCheckerColor2()[2]));
	
	Procedural.Gradient.Reset();

	for (int i = 0; i < this->Gradient->GetSize(); i++)
	{
		double NodeValue[6];
		this->Gradient->GetNodeValue(i, NodeValue);
		Procedural.Gradient.AddNode(ExposureRender::ColorNode::FromRGB(NodeValue[0], ExposureRender::ColorRGBf(NodeValue[1], NodeValue[2], NodeValue[3])));
	}

	TextureDataOut->Bindable.Offset		= Vec2f(this->GetOffset()[0], this->GetOffset()[1]);
	TextureDataOut->Bindable.Repeat		= Vec2f(this->GetRepeat()[0], this->GetRepeat()[1]);
	TextureDataOut->Bindable.Flip		= Vec2i(this->GetFlip()[0], this->GetFlip()[1]);
	TextureDataOut->Bindable.BitmapID	= InInfo ? vtkErBitmapData::SafeDownCast(InInfo->Get(vtkDataObject::DATA_OBJECT()))->Bindable.ID : -1;

	TextureDataOut->Bind();

	return 1;
}

int vtkErTexture::RequestUpdateExtent(vtkInformation* vtkNotUsed(Request), vtkInformationVector** InputVector, vtkInformationVector* vtkNotUsed(OutputVector))
{
	return 1;
}

int vtkErTexture::ProcessRequest(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
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
