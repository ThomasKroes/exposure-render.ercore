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
#include "vtkErVolumeMapper.h"

#include "vtkgl.h"

vtkStandardNewMacro(vtkErTracerData);
vtkCxxRevisionMacro(vtkErTracerData, "$Revision: 1.0 $");

vtkStandardNewMacro(vtkErTracer);
vtkCxxRevisionMacro(vtkErTracer, "$Revision: 1.0 $");

vtkErTracer::vtkErTracer(void)
{
	this->SetNumberOfInputPorts(4);
	this->SetNumberOfOutputPorts(0);

	glGenTextures(1, &TextureID);
}

vtkErTracer::~vtkErTracer(void)
{
	delete[] this->ImageBuffer;
}

int vtkErTracer::FillInputPortInformation(int Port, vtkInformation* Info)
{
	switch (Port)
	{
		case 0:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErVolumeData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 0);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
			return 1;
		}

		case 1:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErLightData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
			return 1;
		}

		case 2:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErObjectData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
			return 1;
		}

		case 3:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErClippingObjectData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
			return 1;
		}
	}

	return 0;
}

int vtkErTracer::FillOutputPortInformation(int Port, vtkInformation* Info)
{
	if (Port == 0)
	{
		Info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkErTracerData");
	}

	return 1;
}

bool vtkErTracer::BeforeRender(vtkRenderer* Renderer, vtkVolume* Volume)
{
	return true;
}

void vtkErTracer::Render(vtkRenderer* Renderer, vtkVolume* Volume)
{
	if (!this->BeforeRender(Renderer, Volume))
		return;

	int RenderSize[2];

	int* WindowSize = Renderer->GetRenderWindow()->GetSize();

	RenderSize[0] = WindowSize[0];
	RenderSize[1] = WindowSize[1];

	glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, TextureID);
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, RenderSize[0], RenderSize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, this->ImageBuffer);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	glColor4f(1.0f, 0.0f, 1.0f, 0.5f);

	double d = 0.5;
	
    Renderer->SetDisplayPoint(0,0,d);
    Renderer->DisplayToWorld();
    double coordinatesA[4];
    Renderer->GetWorldPoint(coordinatesA);

    Renderer->SetDisplayPoint(RenderSize[0],0,d);
    Renderer->DisplayToWorld();
    double coordinatesB[4];
    Renderer->GetWorldPoint(coordinatesB);

    Renderer->SetDisplayPoint(RenderSize[0], RenderSize[1],d);
    Renderer->DisplayToWorld();
    double coordinatesC[4];
    Renderer->GetWorldPoint(coordinatesC);

    Renderer->SetDisplayPoint(0,RenderSize[1],d);
    Renderer->DisplayToWorld();
    double coordinatesD[4];
    Renderer->GetWorldPoint(coordinatesD);
	
	glPushAttrib(GL_LIGHTING);
	glDisable(GL_LIGHTING);

	glBegin(GL_QUADS);
		glTexCoord2i(1,1);
		glVertex4dv(coordinatesA);
		glTexCoord2i(0,1);
		glVertex4dv(coordinatesB);
		glTexCoord2i(0,0);
		glVertex4dv(coordinatesC);
		glTexCoord2i(1,0);
		glVertex4dv(coordinatesD);
	glEnd();

	glPopAttrib();
}
