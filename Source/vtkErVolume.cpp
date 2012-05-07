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
#include "vtkErVolume.h"

namespace VtkExposureRender
{

vtkStandardNewMacro(vtkErVolume);

vtkErVolume::vtkErVolume(void)
{
}

vtkErVolume::~vtkErVolume(void)
{
}

int vtkErVolume::RequestData(vtkInformation* Request, vtkInformationVector** InputVector, vtkInformationVector* OutputVector)
{
	vtkImageAlgorithm::RequestData(Request, InputVector, OutputVector);

	// Get the info objects
	vtkInformation *inInfo = InputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = OutputVector->GetInformationObject(0);

	// Get the input and ouptut
	vtkImageData *input = vtkImageData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkImageData *output = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	vtkSmartPointer<vtkImageData> image =
	vtkSmartPointer<vtkImageData>::New();
	image->ShallowCopy(input);

	image->SetScalarComponentFromDouble(0,0,0,0, 5.0);

	output->ShallowCopy(image);

	// Without these lines, the output will appear real but will not work as the input to any other filters
	output->SetExtent(input->GetExtent());
	output->SetUpdateExtent(output->GetExtent());
	output->SetWholeExtent(output->GetExtent());

	return 1;
}

void vtkErVolume::ExecuteData(vtkDataObject* Output)
{
	vtkImageAlgorithm::ExecuteData(Output);

	vtkImageData* pImageData = this->AllocateOutputData(Output);

	if (!pImageData)
		return;

	const Vec3i Resolution(pImageData->GetExtent()[1], pImageData->GetExtent()[3], pImageData->GetExtent()[5]);
	const Vec3f Spacing(pImageData->GetSpacing()[1], pImageData->GetSpacing()[3], pImageData->GetSpacing()[5]);

	this->Bindable.BindVoxels(Resolution, Spacing, (unsigned short*)pImageData->GetScalarPointer(), true);

	this->ErBind();
}

}
