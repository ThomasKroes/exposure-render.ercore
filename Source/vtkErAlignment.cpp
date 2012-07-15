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
#include "vtkErAlignment.h"

vtkStandardNewMacro(vtkErAlignment);
vtkCxxRevisionMacro(vtkErAlignment, "$Revision: 1.0 $");

vtkErAlignment::vtkErAlignment()
{
	this->ManualTM = vtkSmartPointer<vtkMatrix4x4>::New();

	this->SetAlignmentType(Enums::AxisAlign);
	this->SetAxis(Enums::Y);
	this->SetAutoFlip(true);
	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetTarget(0.0f, 0.0f, 0.0f);
	this->SetUp(0.0f, 1.0f, 0.0f);
	this->SetElevation(45.0f);
	this->SetAzimuth(180.0f);
	this->SetOffset(1.0f);
	this->SetRelativeToCamera(0);
	this->SetUseCameraFocalPoint(1);
}

void vtkErAlignment::RequestData(ExposureRender::Alignment& Alignment)
{
	Alignment.SetType(this->GetAlignmentType());
	Alignment.SetAxis(this->GetAxis());
	Alignment.SetAutoFlip(this->GetAutoFlip());
	Alignment.SetPosition(Vec3f(this->GetPosition()[0], this->GetPosition()[1], this->GetPosition()[2]));
	Alignment.SetTarget(Vec3f(this->GetTarget()[0], this->GetTarget()[1], this->GetTarget()[2]));
	Alignment.SetUp(Vec3f(this->GetUp()[0], this->GetUp()[1], this->GetUp()[2]));
	Alignment.SetElevation(this->GetElevation());
	Alignment.SetAzimuth(this->GetAzimuth());
	Alignment.SetOffset(this->GetOffset());

	if (this->ManualTM.GetPointer())
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				Alignment.GetManualTM().SetElement(i, j, this->ManualTM->GetElement(i, j));
	}

	Alignment.SetUseOffset(this->GetRelativeToCamera());
}

void vtkErAlignment::GetCameraOffset(vtkCamera* Camera, Matrix44& Offset)
{
	if (this->RelativeToCamera)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				Offset.SetElement(i, j, Camera->GetViewTransformMatrix()->GetElement(i, j));

		if (this->UseCameraFocalPoint)
		{
			Offset.SetElement(0, 3, Camera->GetFocalPoint()[0]);
			Offset.SetElement(1, 3, Camera->GetFocalPoint()[1]);
			Offset.SetElement(2, 3, Camera->GetFocalPoint()[2]);
		}
	}
}
