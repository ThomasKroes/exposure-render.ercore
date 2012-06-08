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
#include "vtkErTracer.h"
#include "vtkErCamera.h"
#include "vtkErVolume.h"
#include "vtkErLight.h"
#include "vtkErObject.h"
#include "vtkErClippingObject.h"

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

	this->ImageBuffer				= NULL;
	this->LastRenderSize				= Vec2i(0);
	this->CameraTimeStamp			= 0;
	this->VolumeProperty			= vtkSmartPointer<vtkErVolumeProperty>::New();
	this->VolumePropertyTimeStamp	= 0;

	this->Tracer.SetDirty();
}

vtkErTracer::~vtkErTracer(void)
{
	delete[] this->ImageBuffer;
}

int vtkErTracer::FillInputPortInformation(int Port, vtkInformation* Info)
{
	switch (Port)
	{
		case VolumesPort:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErVolumeData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
			return 1;
		}

		case LightsPort:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErLightData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
			return 1;
		}

		case ObjectsPort:
		{
			Info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkErObjectData");
			Info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
			Info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
			return 1;
		}

		case ClippingObjectsPort:
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
	return 1;
}

void vtkErTracer::BeforeRender(vtkRenderer* Renderer, vtkVolume* Volume)
{
	if (this->VolumePropertyTimeStamp < this->VolumeProperty->GetMTime())
	{
		this->VolumeProperty->RequestData(this->Tracer.VolumeProperty);
		
		this->VolumePropertyTimeStamp = this->VolumeProperty->GetMTime();
		this->Tracer.SetDirty();
	}

	const Vec2i CurrentRenderSize = Vec2i(Renderer->GetRenderWindow()->GetSize()[0], Renderer->GetRenderWindow()->GetSize()[1]);

	if (this->LastRenderSize != CurrentRenderSize)
	{
		this->LastRenderSize = CurrentRenderSize;

		delete[] this->ImageBuffer;
		this->ImageBuffer = new ExposureRender::ColorRGBAuc[this->LastRenderSize[0] * this->LastRenderSize[1]];

		this->Tracer.Camera.FilmSize = this->LastRenderSize;
		this->Tracer.SetDirty();
	}

	vtkErCamera* Camera = dynamic_cast<vtkErCamera*>(Renderer->GetActiveCamera());
	
	if (Camera && Camera->GetMTime() != this->CameraTimeStamp)
	{
		Camera->RequestData(this->Tracer.Camera);

		this->CameraTimeStamp = Camera->GetMTime();
		this->Tracer.SetDirty();
	}
	
	const int NoVolumes = this->GetNumberOfInputConnections(VolumesPort);

	this->Tracer.VolumeIDs.Count = 0;

	for (int i = 0; i < NoVolumes; i++)
	{
		vtkErVolumeData* VolumeData = vtkErVolumeData::SafeDownCast(this->GetInputDataObject(VolumesPort, i));

		if (VolumeData)
		{
			this->Tracer.VolumeIDs[this->Tracer.VolumeIDs.Count] = VolumeData->Bindable.ID;
			this->Tracer.VolumeIDs.Count++;
		}
	}

	const int NoLights = this->GetNumberOfInputConnections(LightsPort);

	this->Tracer.LightIDs.Count = 0;

	for (int i = 0; i < NoLights; i++)
	{
		vtkErLightData* LightData = vtkErLightData::SafeDownCast(this->GetInputDataObject(LightsPort, i));

		if (LightData && LightData->Bindable.Enabled)
		{
			this->Tracer.LightIDs[this->Tracer.LightIDs.Count] = LightData->Bindable.ID;
			this->Tracer.LightIDs.Count++;

			LightData->Light->GetCameraOffset(Camera, LightData->Bindable.Shape.Alignment.OffsetTM);
			LightData->Bind();
		}
	}

	const int NoObjects = this->GetNumberOfInputConnections(ObjectsPort);

	this->Tracer.ObjectIDs.Count = 0;
	
	for (int i = 0; i < NoObjects; i++)
	{
		vtkErObjectData* ObjectData = vtkErObjectData::SafeDownCast(this->GetInputDataObject(ObjectsPort, i));

		if (ObjectData && ObjectData->Bindable.Enabled)
		{
			this->Tracer.ObjectIDs[this->Tracer.ObjectIDs.Count] = ObjectData->Bindable.ID;
			this->Tracer.ObjectIDs.Count++;

			ObjectData->Object->GetCameraOffset(Camera, ObjectData->Bindable.Shape.Alignment.OffsetTM);
			ObjectData->Bind();
		}
	}

	const int NoClippingObjects = this->GetNumberOfInputConnections(ClippingObjectsPort);

	this->Tracer.ClippingObjectIDs.Count = 0;
	
	for (int i = 0; i < NoClippingObjects; i++)
	{
		vtkErClippingObjectData* ClippingObjectData = vtkErClippingObjectData::SafeDownCast(this->GetInputDataObject(ClippingObjectsPort, i));

		if (ClippingObjectData && ClippingObjectData->Bindable.Enabled)
		{
			this->Tracer.ClippingObjectIDs[this->Tracer.ClippingObjectIDs.Count] = ClippingObjectData->Bindable.ID;
			this->Tracer.ClippingObjectIDs.Count++;

			ClippingObjectData->ClippingObject->GetCameraOffset(Camera, ClippingObjectData->Bindable.Shape.Alignment.OffsetTM);
			ClippingObjectData->Bind();
		}
	}

	if (this->Tracer.GetDirty())
	{
		ER_CALL(ExposureRender::BindTracer(this->Tracer));
		this->Tracer.SetDirty(false);
	}
}

void vtkErTracer::Render(vtkRenderer* Renderer, vtkVolume* Volume)
{
	this->InvokeEvent(vtkCommand::VolumeMapperRenderStartEvent,0);

	this->BeforeRender(Renderer, Volume);

	ER_CALL(ExposureRender::Render(this->Tracer.ID));
	ER_CALL(ExposureRender::GetDisplayEstimate(this->Tracer.ID, this->ImageBuffer));

	glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, TextureID);
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->LastRenderSize[0], this->LastRenderSize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)this->ImageBuffer);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	double d = 0.5;
	
    Renderer->SetDisplayPoint(0,0,d);
    Renderer->DisplayToWorld();
    double coordinatesA[4];
    Renderer->GetWorldPoint(coordinatesA);

    Renderer->SetDisplayPoint(this->LastRenderSize[0],0,d);
    Renderer->DisplayToWorld();
    double coordinatesB[4];
    Renderer->GetWorldPoint(coordinatesB);

    Renderer->SetDisplayPoint(this->LastRenderSize[0], this->LastRenderSize[1],d);
    Renderer->DisplayToWorld();
    double coordinatesC[4];
    Renderer->GetWorldPoint(coordinatesC);

    Renderer->SetDisplayPoint(0,this->LastRenderSize[1],d);
    Renderer->DisplayToWorld();
    double coordinatesD[4];
    Renderer->GetWorldPoint(coordinatesD);
	
	glPushAttrib(GL_LIGHTING);
	glDisable(GL_LIGHTING);

	glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex4dv(coordinatesA);
		glTexCoord2i(1, 0);
		glVertex4dv(coordinatesB);
		glTexCoord2i(1, 1);
		glVertex4dv(coordinatesC);
		glTexCoord2i(0, 1);
		glVertex4dv(coordinatesD);
	glEnd();

	glPopAttrib();

	this->InvokeEvent(vtkCommand::VolumeMapperRenderEndEvent,0);
}
