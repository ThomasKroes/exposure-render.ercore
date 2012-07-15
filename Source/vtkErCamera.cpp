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
#include "vtkErCamera.h"

vtkStandardNewMacro(vtkErCamera);
vtkCxxRevisionMacro(vtkErCamera, "$Revision: 1.0 $");

vtkErCamera::vtkErCamera(void)
{
	this->SetFocalDistance(1.0f);
	this->SetExposure(1.0f);
	this->SetGamma(2.2f);
	this->SetApertureShape(Enums::Polygon);
	this->SetApertureSize(0.0f);
	this->SetNoApertureBlades(6);
	this->SetApertureAngle(0.0f);
}

vtkErCamera::~vtkErCamera(void)
{
}

void vtkErCamera::RequestData(ExposureRender::Camera& Camera)
{
	Camera.SetPos(Vec3f(this->GetPosition()[0], this->GetPosition()[1], this->GetPosition()[2]));
	Camera.SetTarget(Vec3f(this->GetFocalPoint()[0], this->GetFocalPoint()[1], this->GetFocalPoint()[2]));
	Camera.SetUp(Vec3f(this->GetViewUp()[0], this->GetViewUp()[1], this->GetViewUp()[2]));
	Camera.SetClipNear(0.0f);
	Camera.SetClipFar(1000.0f);
	Camera.SetFOV(this->GetViewAngle());
	Camera.SetFocalDistance(this->GetFocalDistance());
	Camera.SetExposure(this->GetExposure());
	Camera.SetGamma(this->GetGamma());
	Camera.SetApertureShape(this->GetApertureShape());
	Camera.SetApertureSize(this->GetApertureSize());
	Camera.SetNoApertureBlades(this->GetNoApertureBlades());
	Camera.SetApertureAngle(this->GetApertureAngle());
}
