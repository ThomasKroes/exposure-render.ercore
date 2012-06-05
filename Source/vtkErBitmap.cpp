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
#include "vtkErBitmap.h"

vtkStandardNewMacro(vtkErBitmapData);
vtkCxxRevisionMacro(vtkErBitmapData, "$Revision: 1.0 $");

vtkStandardNewMacro(vtkErBitmap);
vtkCxxRevisionMacro(vtkErBitmap, "$Revision: 1.0 $");

vtkErBitmap::vtkErBitmap(void)
{
	this->SetNumberOfInputPorts(1);
	this->SetNumberOfOutputPorts(1);

	this->SetFilterMode(Enums::Linear);
}

vtkErBitmap::~vtkErBitmap(void)
{
}

int vtkErBitmap::FillInputPortInformation(int Port, vtkInformation* Info)
{
	if (Port == ImageDataPort)
	{
		Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
		Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 0);
		Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
	}

	return 1;
}

int vtkErBitmap::FillOutputPortInformation(int Port, vtkInformation* Info)
{
	if (Port == 0)
	{
		Info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkErBitmapData");
	}

	return 1;
}

int vtkErBitmap::RequestDataObject(vtkInformation* vtkNotUsed(request), vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* OutputVector)
{
	vtkInformation* OutInfo = OutputVector->GetInformationObject(0);
	vtkErBitmapData* Output = vtkErBitmapData::SafeDownCast(OutInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (!Output)
	{
		Output = vtkErBitmapData::New();
		OutInfo->Set(vtkDataObject::DATA_OBJECT(), Output);
		Output->FastDelete();
		Output->SetPipelineInformation(OutInfo);

		this->GetOutputPortInformation(0)->Set(vtkDataObject::DATA_EXTENT_TYPE(), Output->GetExtentType());
	}
 
	return 1;
}

int vtkErBitmap::RequestInformation(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
{
	return 1;
}

int vtkErBitmap::RequestData(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
{
	vtkInformation* InInfo = InputVector[0]->GetInformationObject(0);
	vtkInformation* OutInfo	= OutputVector->GetInformationObject(0);

	if (!InInfo || !OutInfo)
		return 0;
	
	vtkImageData* ImageDataIn = vtkImageData::SafeDownCast(InInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (!ImageDataIn)
		return 0;

	if (ImageDataIn->GetDataDimension() != 2)
	{
		vtkErrorMacro("vtkErBitmap onlys works with 2 dimensional image data!");
		return 0;
	}

	vtkErBitmapData* BitmapDataOut = vtkErBitmapData::SafeDownCast(OutInfo->Get(vtkDataObject::DATA_OBJECT()));
	
	if (!BitmapDataOut)
		return 0;

	const Vec2i Resolution(ImageDataIn->GetExtent()[1] + 1, ImageDataIn->GetExtent()[3] + 1);

	const int NoScalarComponents = ImageDataIn->GetNumberOfScalarComponents();

	if (NoScalarComponents != 3)
	{
		vtkErrorMacro("vtkErBitmap currently only supports 3scalar components per pixel!");
		return 0;
	}

	/*
	ColorRGBAuc* Pixels = new ColorRGBAuc[Resolution[0] * Resolution[1]];
	
	for (int x = 0; x < Resolution[0]; x++)
	{
		for (int y = 0; y < Resolution[1]; y++)
		{
			Pixels[y * Resolution[0] + x] = 255;
			
			const int PID = y * Resolution[0] + x;

			ColorRGBf Color;

			if (NoScalarComponents == 1)
				Color = ColorRGBf(ImageDataIn->GetScalarComponentAsFloat(x, y, 0, 0), 0.0f, 0.0f);

			if (NoScalarComponents == 2)
				Color = ColorRGBf(ImageDataIn->GetScalarComponentAsFloat(x, y, 0, 0), ImageDataIn->GetScalarComponentAsFloat(x, y, 0, 1), 0.0f);

			if (NoScalarComponents == 3)
				Color = ColorRGBf(ImageDataIn->GetScalarComponentAsFloat(x, y, 0, 0), ImageDataIn->GetScalarComponentAsFloat(x, y, 0, 1), ImageDataIn->GetScalarComponentAsFloat(x, y, 0, 2));

			if (NoScalarComponents == 4)
				Color = ColorRGBf(ImageDataIn->GetScalarComponentAsFloat(x, y, 0, 0), ImageDataIn->GetScalarComponentAsFloat(x, y, 0, 1), ImageDataIn->GetScalarComponentAsFloat(x, y, 0, 2));

			if (ImageDataIn->GetScalarType() == VTK_CHAR || ImageDataIn->GetScalarType() == VTK_SIGNED_CHAR || ImageDataIn->GetScalarType() == VTK_UNSIGNED_CHAR)
				Color /= 255.0f;

			Pixels[PID] = ColorXYZf::FromRGBf(Color);
		}
	}
	*/

	if (ImageDataIn->GetScalarType() == VTK_UNSIGNED_CHAR)
	{
		BitmapDataOut->Bindable.BindPixels(Resolution, (ColorRGBuc*)ImageDataIn->GetScalarPointer());
		BitmapDataOut->Bindable.Pixels.SetFilterMode(this->GetFilterMode());
	}

	BitmapDataOut->Bind();

//	delete[] Pixels;

	return 1;
}

int vtkErBitmap::RequestUpdateExtent(vtkInformation* vtkNotUsed(Request), vtkInformationVector** InputVector, vtkInformationVector* vtkNotUsed(OutputVector))
{
	return 1;
}

int vtkErBitmap::ProcessRequest(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
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
