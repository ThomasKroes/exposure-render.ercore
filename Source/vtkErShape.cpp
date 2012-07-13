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
#include "vtkErShape.h"

vtkStandardNewMacro(vtkErShape);
vtkCxxRevisionMacro(vtkErShape, "$Revision: 1.0 $");

vtkErShape::vtkErShape()
{
	this->SetOneSided(true);
	this->SetShapeType(Enums::Plane);
	this->SetSize(1.0f, 1.0f, 1.0f);
	this->SetRadius(1.0f);
	this->SetInnerRadius(0.5f);
	this->SetOuterRadius(1.0f);
}

void vtkErShape::RequestData(ExposureRender::Shape& Shape)
{
	vtkErAlignment::RequestData(Shape.Alignment);

	Shape.Type = this->GetShapeType();

	switch (this->GetShapeType())
	{
		case Enums::Plane:
		{
			Shape.Plane = Plane(Vec2f(Size[0], Size[1]), this->GetOneSided());
			break;
		}

		case Enums::Disk:
		{
			Shape.Disk = Disk(this->GetRadius(), this->GetOneSided());
			break;
		}

		case Enums::Ring:
		{
			Shape.Ring = Ring(this->GetInnerRadius(), this->GetOuterRadius(), this->GetOneSided());
			break;
		}

		case Enums::Sphere:
		{
			Shape.Sphere = Sphere(this->GetRadius());
			break;
		}

		case Enums::Box:
		{
			Shape.Box = Box(Vec3f(Size[0], Size[1], Size[2]));
			break;
		}
	}
}
